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

# Create figure with three subplots
plt.figure(figsize=(15, 12))

# Plot original spectrogram (red)
plt.subplot(3, 1, 1)
librosa.display.specshow(D1, sr=sr1, x_axis='time', y_axis='log', cmap='Reds')
plt.colorbar(format='%+2.0f dB')
plt.title('Original Trumpet Sample (Bflat3)')

# Plot wavetable spectrogram (blue)
plt.subplot(3, 1, 2)
librosa.display.specshow(D2, sr=sr2, x_axis='time', y_axis='log', cmap='Blues')
plt.colorbar(format='%+2.0f dB')
plt.title('Wavetable Recreation of Trumpet (Bflat3)')

# Plot combined spectrogram
plt.subplot(3, 1, 3)
librosa.display.specshow(D1, sr=sr1, x_axis='time', y_axis='log', cmap='Reds', alpha=0.5)
librosa.display.specshow(D2, sr=sr2, x_axis='time', y_axis='log', cmap='Blues', alpha=0.5)
plt.colorbar(format='%+2.0f dB')
plt.title('Combined View (Original in Red, Wavetable in Blue)')

plt.tight_layout()

# Save the figure
plt.savefig('report/trumpet_comparison.png')

# Show the figure
plt.show()