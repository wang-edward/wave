import numpy as np
import struct
import os
from scipy import interpolate

def save_wavetable_to_binary(wavetable, filename):
    """
    Saves the wavetable to a binary file.
    
    The file format is:
      - 4 bytes: unsigned int (little-endian) representing the number of samples.
      - 4 bytes per sample: float32 samples.
    """
    # Ensure the wavetable is float32
    wavetable = wavetable.astype(np.float32)
    
    # Create bin_samples directory if it doesn't exist
    os.makedirs("bin_samples", exist_ok=True)
    
    # Write format
    with open(filename, "wb") as f:
        # First thing is the table length
        f.write(struct.pack("<I", len(wavetable)))
        # Next is the samples
        f.write(wavetable.tobytes())

def generate_sine(size):
    """Generate one period of a sine wave with given number of samples."""
    t = np.linspace(0, 2*np.pi, size, endpoint=False)
    return np.sin(t)

def upsample_nearest(signal, target_size):
    """Upsample signal to target_size using nearest neighbor interpolation."""
    x_original = np.linspace(0, 1, len(signal))
    x_upsampled = np.linspace(0, 1, target_size)
    f_upsample = interpolate.interp1d(x_original, signal, kind='nearest')
    return f_upsample(x_upsampled)

# Generate sine waves at different initial resolutions
initial_resolutions = [16, 32, 61, 64, 128, 1024]
final_size = 1024

for size in initial_resolutions:
    # Generate sine wave at initial resolution
    sine = generate_sine(size)
    
    # If not already at final size, upsample to 1024 using nearest neighbor
    if size != final_size:
        sine = upsample_nearest(sine, final_size)
    
    # Save to binary file
    filename = f"bin_samples/sine_{size}.bin"
    save_wavetable_to_binary(sine, filename)
    print(f"Generated {size}-resolution sine wave (upsampled to {final_size}): {filename}")

    # Optional: Plot for verification
    import matplotlib.pyplot as plt
    plt.figure(figsize=(10, 4))
    plt.plot(sine)
    plt.title(f"Sine wave: {size} resolution upsampled to {final_size}")
    plt.grid(True)
    plt.savefig(f"bin_samples/sine_{size}.png")
    plt.close() 