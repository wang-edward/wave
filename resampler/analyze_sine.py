import numpy as np
import matplotlib.pyplot as plt
import librosa
import librosa.display
import struct
from scipy import signal
from pathlib import Path

def load_wavetable(filename):
    """Load a wavetable from our binary format."""
    with open(filename, 'rb') as f:
        # Read table length (4 bytes, uint32)
        table_length = struct.unpack('<I', f.read(4))[0]
        # Read samples (4 bytes each, float32)
        data = np.frombuffer(f.read(), dtype=np.float32)
    return data

def analyze_sine(filename, resolution):
    """Generate comprehensive analysis plots for a sine wave."""
    # Load the wavetable
    wave = load_wavetable(filename)
    
    # Create figure with subplots
    fig = plt.figure(figsize=(15, 12))
    
    # 1. Time Series Plot (top left)
    ax1 = plt.subplot(3, 2, 1)
    ax1.plot(wave[:128], label='First 128 samples')  # Show first 128 samples for detail
    ax1.set_title(f'Time Series (First 128 samples)\nResolution: {resolution}')
    ax1.set_xlabel('Sample Index')
    ax1.set_ylabel('Amplitude')
    ax1.grid(True)
    
    # 2. Spectrogram (top right)
    ax2 = plt.subplot(3, 2, 2)
    D = librosa.amplitude_to_db(np.abs(librosa.stft(wave)), ref=np.max)
    librosa.display.specshow(D, y_axis='log', x_axis='time', ax=ax2)
    ax2.set_title('Spectrogram')
    plt.colorbar(ax=ax2.images[0], format='%+2.0f dB')
    
    # 3. FFT Magnitude Response - Linear Scale (middle left)
    ax3 = plt.subplot(3, 2, 3)
    fft = np.fft.fft(wave)
    freqs = np.fft.fftfreq(len(wave))
    ax3.plot(freqs[:len(freqs)//2], np.abs(fft)[:len(fft)//2])
    ax3.set_title('FFT Magnitude (Linear Scale)')
    ax3.set_xlabel('Normalized Frequency')
    ax3.set_ylabel('Magnitude')
    ax3.grid(True)
    
    # 4. FFT Magnitude Response - Log Scale (middle right)
    ax4 = plt.subplot(3, 2, 4)
    magnitude_db = 20 * np.log10(np.abs(fft) + 1e-10)  # Add small number to avoid log(0)
    ax4.plot(freqs[:len(freqs)//2], magnitude_db[:len(fft)//2])
    ax4.set_title('FFT Magnitude (Log Scale)')
    ax4.set_xlabel('Normalized Frequency')
    ax4.set_ylabel('Magnitude (dB)')
    ax4.grid(True)
    
    # 5. Welch's Power Spectral Density (bottom left)
    ax5 = plt.subplot(3, 2, 5)
    f, Pxx = signal.welch(wave, nperseg=256)
    ax5.semilogy(f, Pxx)
    ax5.set_title("Welch's Power Spectral Density")
    ax5.set_xlabel('Normalized Frequency')
    ax5.set_ylabel('Power/Frequency')
    ax5.grid(True)
    
    # 6. Phase Plot (bottom right)
    ax6 = plt.subplot(3, 2, 6)
    phase = np.angle(fft)
    ax6.plot(freqs[:len(freqs)//2], phase[:len(fft)//2])
    ax6.set_title('Phase Response')
    ax6.set_xlabel('Normalized Frequency')
    ax6.set_ylabel('Phase (radians)')
    ax6.grid(True)
    
    plt.tight_layout()
    
    # Save the figure
    output_dir = Path('analysis_output')
    output_dir.mkdir(exist_ok=True)
    plt.savefig(output_dir / f'sine_analysis_{resolution}.png', dpi=300, bbox_inches='tight')
    plt.close()

def main():
    # Analyze each resolution
    resolutions = [16, 32, 61, 64, 128, 1024]
    
    for res in resolutions:
        filename = f'bin_samples/sine_{res}.bin'
        print(f"Analyzing {filename}...")
        analyze_sine(filename, res)
        print(f"Analysis saved as analysis_output/sine_analysis_{res}.png")

if __name__ == "__main__":
    main() 