import numpy as np
import librosa
import librosa.display
import scipy.signal
import argparse
import struct
import os
import matplotlib.pyplot as plt
from scipy import interpolate

def normalize_audio(audio):
    """
    Normalizes the audio array so that its maximum absolute value is 1.
    """
    max_val = np.max(np.abs(audio))
    if max_val > 0:
        return audio / max_val
    return audio

def extract_wavetable(audio_file, downsample_size=512, table_size=1024):
    """
    Loads an audio file, normalizes it, trims silence, extracts one period of a tone at the
    detected fundamental frequency, and resamples that cycle into a wavetable.
    
    Parameters:
      audio_file (str): Path to the input audio file.
      downsample_size (int): Size to downsample to before upsampling.
      table_size (int): Final wavetable size after upsampling.
      
    Returns:
      wavetable (np.ndarray): The wavetable with table_size samples.
      sr (int): The sample rate of the audio file.
      detected_freq (float): The detected fundamental frequency.
    """
    # Load the audio file (mono) and normalize immediately
    y, sr = librosa.load(audio_file, sr=None, mono=True)
    y = normalize_audio(y)
    
    # Trim silence from beginning and end (adjust top_db if needed)
    y_trimmed, _ = librosa.effects.trim(y, top_db=20)
    
    # Detect the fundamental frequency using PYIN
    fmin = librosa.note_to_hz('C2')  # Lower bound for trumpet
    fmax = librosa.note_to_hz('C7')  # Upper bound for trumpet
    f0, voiced_flag, voiced_prob = librosa.pyin(y_trimmed, fmin=fmin, fmax=fmax)
    
    # Get the most common non-zero frequency
    f0 = f0[voiced_flag]  # Only consider voiced segments
    if len(f0) == 0:
        raise ValueError("Could not detect a fundamental frequency in the audio.")
    
    # Use the median frequency to be robust against outliers
    detected_freq = np.median(f0)
    print(f"Detected fundamental frequency: {detected_freq:.2f} Hz")
    
    # Calculate the period length in samples for the detected frequency
    period_length = sr / detected_freq
    period_samples = int(round(period_length))
    
    # Extract a cycle from the center of the trimmed audio
    center = len(y_trimmed) // 2
    start = center - period_samples // 2
    end = start + period_samples
    
    if start < 0 or end > len(y_trimmed):
        raise ValueError("Not enough samples in the trimmed audio to extract one period.")
    
    cycle = y_trimmed[start:end]
    
    # First resample to the lower resolution
    wavetable_low = scipy.signal.resample(cycle, downsample_size)
    
    # Then upsample using nearest neighbor interpolation
    x_low = np.linspace(0, 1, downsample_size)
    x_high = np.linspace(0, 1, table_size)
    f_upsample = interpolate.interp1d(x_low, wavetable_low, kind='nearest')
    wavetable = f_upsample(x_high)
    
    # Normalize again
    wavetable = normalize_audio(wavetable)
    
    return wavetable, sr, detected_freq

def save_wavetable_to_binary(wavetable, filename):
    """
    Saves the wavetable to a binary file.
    
    The file format is:
      - 4 bytes: unsigned int (little-endian) representing the number of samples.
      - 4 bytes per sample: float32 samples.
      
    Parameters:
      wavetable (np.ndarray): The wavetable array.
      filename (str): The output binary file name.
    """
    # Ensure the wavetable is float32
    wavetable = wavetable.astype(np.float32)
    
    # Write format
    with open(filename, "wb") as f:
        # First thing is the table length
        f.write(struct.pack("<I", len(wavetable)))
        # Next is the samples
        f.write(wavetable.tobytes())

def plot_audio_analysis(y, sr):
    """
    Plots a time series graph of the original normalized sound, its spectrogram,
    and the perceived frequency (f0) over time using librosa's pyin.
    """
    fig, axs = plt.subplots(3, 1, figsize=(12, 12))

    # Subplot 1: Time Series of Original Sound
    t = np.linspace(0, len(y) / sr, num=len(y))
    axs[0].plot(t, y)
    axs[0].set_title("Time Series of Normalized Sound")
    axs[0].set_xlabel("Time (s)")
    axs[0].set_ylabel("Amplitude")

    # Subplot 2: Spectrogram
    S = np.abs(librosa.stft(y))
    S_db = librosa.amplitude_to_db(S, ref=np.max)
    img = librosa.display.specshow(S_db, sr=sr, x_axis='time', y_axis='hz', ax=axs[1])
    axs[1].set_title("Spectrogram of Normalized Sound")
    fig.colorbar(img, ax=axs[1], format="%+2.0f dB")

    # Subplot 3: Perceived Frequency (f0)
    # Define a reasonable pitch range for estimation.
    fmin = librosa.note_to_hz('C2')
    fmax = librosa.note_to_hz('C7')
    f0, voiced_flag, voiced_prob = librosa.pyin(y, fmin=fmin, fmax=fmax)
    times = librosa.times_like(f0, sr=sr)
    axs[2].plot(times, f0, label="f0", color="b")
    axs[2].set_title("Perceived Frequency (f0) Over Time")
    axs[2].set_xlabel("Time (s)")
    axs[2].set_ylabel("Frequency (Hz)")
    axs[2].legend()

    plt.tight_layout()
    plt.show()

def main():
    parser = argparse.ArgumentParser(
        description="Convert an audio file into a wavetable, save as a binary file, and plot analysis graphs."
    )
    parser.add_argument("audio_file", type=str, help="Path to the audio file (WAV, MP3, etc.).")
    parser.add_argument("--downsample_size", type=int, default=512, help="Size to downsample to before upsampling (default: 512).")
    parser.add_argument("--table_size", type=int, default=1024, help="Final wavetable size (default: 1024).")
    parser.add_argument("--output", type=str, default=None,
                        help="Output binary file. If not specified, uses input filename with .bin extension.")
    args = parser.parse_args()

    # If no output filename is provided, convert the input filename to .bin
    if not args.output:
        base, _ = os.path.splitext(os.path.basename(args.audio_file))
        args.output = "bin_samples/" + base + ".bin"

    # Plot analysis of the original audio
    y_orig, sr_orig = librosa.load(args.audio_file, sr=None, mono=True)
    y_orig = normalize_audio(y_orig)
    plot_audio_analysis(y_orig, sr_orig)

    # Extract the wavetable
    try:
        wavetable, sr, detected_freq = extract_wavetable(args.audio_file, 
                                                        downsample_size=args.downsample_size,
                                                        table_size=args.table_size)
        
        # Save the wavetable to a binary file
        save_wavetable_to_binary(wavetable, args.output)
        print(f"Binary wavetable saved as {args.output}")
        print(f"Detected note: {librosa.hz_to_note(detected_freq)} ({detected_freq:.2f} Hz)")
        
        # Plot the final wavetable
        plt.figure(figsize=(10, 4))
        plt.plot(wavetable)
        plt.title(f"Extracted Wavetable (Resampled from {args.downsample_size} to {args.table_size})")
        plt.xlabel("Sample Index")
        plt.ylabel("Amplitude")
        plt.grid(True)
        plt.show()
        
    except Exception as e:
        print("Error processing file:", e)
        return

if __name__ == "__main__":
    main()
