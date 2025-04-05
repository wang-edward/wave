import librosa
import librosa.display
import matplotlib.pyplot as plt
import numpy as np

def compute_spectrogram(audio_path):
    # Load audio file
    y, sr = librosa.load(audio_path)
    # Compute spectrogram
    D = librosa.amplitude_to_db(np.abs(librosa.stft(y)), ref=np.max)
    return D, sr

# Compute spectrograms for both files
D1, sr1 = compute_spectrogram('samples/Trumpet.wav')
D2, sr2 = compute_spectrogram('report/Trumpet_wavetable.wav')

# Create figure with 2x2 grid
plt.figure(figsize=(15, 10))

# Plot original spectrogram (magma)
plt.subplot(2, 2, 1)
librosa.display.specshow(D1, sr=sr1, x_axis='time', y_axis='log', cmap='magma')
plt.colorbar(format='%+2.0f dB')
plt.title('Original Trumpet Sample (Bflat3)')

# Plot wavetable spectrogram (viridis)
plt.subplot(2, 2, 2)
librosa.display.specshow(D2, sr=sr2, x_axis='time', y_axis='log', cmap='viridis')
plt.colorbar(format='%+2.0f dB')
plt.title('Wavetable Recreation of Trumpet (Bflat3)')

# Plot combined spectrogram (spanning bottom row)
plt.subplot(2, 1, 2)
# Plot original first with higher alpha
librosa.display.specshow(D1, sr=sr1, x_axis='time', y_axis='log', cmap='magma', alpha=0.7)
# Plot wavetable second with lower alpha
librosa.display.specshow(D2, sr=sr2, x_axis='time', y_axis='log', cmap='viridis', alpha=0.3)
plt.colorbar(format='%+2.0f dB')
plt.title('Combined View (Original in Magma, Wavetable in Viridis)')

plt.tight_layout()

# Save the figure
plt.savefig('report/trumpet_comparison.png')

# Show the figure
plt.show()