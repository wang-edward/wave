import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as signal
from scipy.io import wavfile
import os
from pathlib import Path

def analyze_wavs(wav_files, output_dir):
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Sort files so that 61 is last
    def sort_key(path):
        num = int(Path(path).stem.split('_')[1])
        return (num == 61, num)  # (is_61, number) ensures 61 is last
    
    wav_files = sorted(wav_files, key=sort_key)
    n_files = len(wav_files)
    
    # Set up three figures
    fig_specs = plt.figure(figsize=(20, 4*n_files))
    fig_specs.suptitle('Spectrogram Comparison Across Resolutions', fontsize=16)
    
    fig_fft_single = plt.figure(figsize=(20, 12))
    fig_fft_single.suptitle('FFT Magnitude Comparison - Single Note C3 (First 25%)', fontsize=16)
    
    fig_fft_chord = plt.figure(figsize=(20, 12))
    fig_fft_chord.suptitle('FFT Magnitude Comparison - CEG3 Chord (Last 75%)', fontsize=16)
    
    # Process each file
    for i, wav_path in enumerate(wav_files, 1):
        print(f'Analyzing {wav_path}...')
        
        # Read and process WAV file
        sample_rate, data = wavfile.read(wav_path)
        data = data.astype(np.float32) / np.iinfo(np.int16).max
        
        # Split data
        split_point = len(data) // 4
        single_note = data[:split_point]
        chord = data[split_point:]
        
        resolution = Path(wav_path).stem.split('_')[1]
        
        # 1. Spectrogram
        plt.figure(fig_specs.number)
        plt.subplot(n_files, 1, i)
        f, t, Sxx = signal.spectrogram(data, sample_rate, nperseg=1024, noverlap=512)
        plt.pcolormesh(t, f, 10 * np.log10(Sxx + 1e-10), shading='gouraud')
        plt.title(f'{resolution} samples/cycle')
        plt.ylabel('Frequency (Hz)')
        if i == n_files:  # Only show xlabel for bottom plot
            plt.xlabel('Time (s)')
        plt.colorbar(label='Intensity (dB)')
        
        # 2. FFT Single Note
        plt.figure(fig_fft_single.number)
        fft_single = np.fft.fft(single_note)
        freqs_single = np.fft.fftfreq(len(single_note), 1/sample_rate)
        positive_mask = (freqs_single >= 0) & (freqs_single <= 1000)
        plt.plot(freqs_single[positive_mask], 
                np.abs(fft_single)[positive_mask],
                label=f'{resolution} samples/cycle')
        plt.xscale('log')
        plt.grid(True)
        plt.legend()
        plt.xlabel('Frequency (Hz)')
        plt.ylabel('Magnitude')
        
        # 3. FFT Chord
        plt.figure(fig_fft_chord.number)
        fft_chord = np.fft.fft(chord)
        freqs_chord = np.fft.fftfreq(len(chord), 1/sample_rate)
        positive_mask = (freqs_chord >= 0) & (freqs_chord <= 1000)
        plt.plot(freqs_chord[positive_mask], 
                np.abs(fft_chord)[positive_mask],
                label=f'{resolution} samples/cycle')
        plt.xscale('log')
        plt.grid(True)
        plt.legend()
        plt.xlabel('Frequency (Hz)')
        plt.ylabel('Magnitude')
    
    # Save figures
    plt.figure(fig_specs.number)
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig(os.path.join(output_dir, 'spectrogram_comparison.png'), dpi=300, bbox_inches='tight')
    
    plt.figure(fig_fft_single.number)
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig(os.path.join(output_dir, 'fft_single_note_comparison.png'), dpi=300, bbox_inches='tight')
    
    plt.figure(fig_fft_chord.number)
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig(os.path.join(output_dir, 'fft_chord_comparison.png'), dpi=300, bbox_inches='tight')

    plt.show()
    
    plt.close('all')

def main():
    # Create output directory
    output_dir = 'analysis_output'
    os.makedirs(output_dir, exist_ok=True)
    
    # Get all sine wave WAV files
    wav_files = list(Path('report').glob('sine_*.wav'))
    
    # Process all files together
    analyze_wavs(wav_files, output_dir)
    print('Analysis complete!')

if __name__ == '__main__':
    main() 