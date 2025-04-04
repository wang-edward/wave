import numpy as np
import librosa
import librosa.display
import scipy.signal
import argparse
import struct
import os
import matplotlib.pyplot as plt

def normalize_audio(audio):
    """
    Normalizes the audio array so that its maximum absolute value is 1.
    """
    max_val = np.max(np.abs(audio))
    if max_val > 0:
        return audio / max_val
    return audio

def extract_wavetable(audio_file, target_freq=440.0, table_size=1024):
    """
    Loads an audio file, normalizes it, trims silence, extracts one period of a tone at the
    target frequency, and resamples that cycle into a wavetable of the given table_size.
    
    Parameters:
      audio_file (str): Path to the input audio file.
      target_freq (float): Expected frequency of the tone in Hz.
      table_size (int): Desired wavetable size.
      
    Returns:
      wavetable (np.ndarray): The wavetable with table_size samples.
      sr (int): The sample rate of the audio file.
    """
    # Load the audio file (mono) and normalize immediately
    y, sr = librosa.load(audio_file, sr=None, mono=True)
    y = normalize_audio(y)
    
    # Trim silence from beginning and end (adjust top_db if needed)
    y_trimmed, _ = librosa.effects.trim(y, top_db=20)
    
    # Calculate the period length in samples for the target frequency
    period_length = sr / target_freq
    period_samples = int(round(period_length))
    
    # Extract a cycle from the center of the trimmed audio
    center = len(y_trimmed) // 2
    start = center - period_samples // 2
    end = start + period_samples
    
    if start < 0 or end > len(y_trimmed):
        raise ValueError("Not enough samples in the trimmed audio to extract one period.")
    
    cycle = y_trimmed[start:end]
    
    # Resample the extracted cycle to the desired table size (1024)
    wavetable = scipy.signal.resample(cycle, table_size)
    
    # Normalize again
    wavetable = normalize_audio(wavetable)
    
    return wavetable, sr

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
    parser.add_argument("--freq", type=float, default=440.0, help="Target frequency in Hz (default: 440 Hz).")
    parser.add_argument("--table_size", type=int, default=1024, help="Wavetable size (default: 1024).")
    parser.add_argument("--output", type=str, default=None,
                        help="Output binary file. If not specified, uses input filename with .bin extension.")
    args = parser.parse_args()

    # If no output filename is provided, convert the input filename to .bin
    if not args.output:
        base, _ = os.path.splitext(os.path.basename(args.audio_file))
        args.output = base + ".bin"

    # Plot analysis of the original audio
    y_orig, sr_orig = librosa.load(args.audio_file, sr=None, mono=True)
    y_orig = normalize_audio(y_orig)
    plot_audio_analysis(y_orig, sr_orig)

    # Extract the wavetable
    try:
        wavetable, sr = extract_wavetable(args.audio_file, target_freq=args.freq, table_size=args.table_size)
    except Exception as e:
        print("Error processing file:", e)
        return

    # Save the wavetable to a binary file
    save_wavetable_to_binary(wavetable, args.output)
    print("Binary wavetable saved as", args.output)

    plt.figure(figsize=(10, 4))
    plt.plot(wavetable)
    plt.title("Extracted Wavetable (Normalized)")
    plt.xlabel("Sample Index")
    plt.ylabel("Amplitude")
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    main()
