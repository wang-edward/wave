#pragma once

typedef struct {
    float b0, b1, b2; // Numerator coefficients
    float a1, a2;     // Denominator coefficients
    float z1, z2;     // State variables
} BiquadFilter;

// Initialize the biquad filter
void Biquad_init(BiquadFilter *filter, float b0, float b1, float b2, float a1, float a2);

// Process a single sample through the biquad filter
float Biquad_process(BiquadFilter *filter, float input);

// Configure filter as a low-pass
void Biquad_design_lowpass(BiquadFilter *filter, float sampleRate, float cutoff, float Q);

// Configure filter as a high-pass
void Biquad_design_highpass(BiquadFilter *filter, float sampleRate, float cutoff, float Q);