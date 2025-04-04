#pragma once

typedef struct {
    float b0, b1, b2; // Numerator coefficients
    float a1, a2;     // Denominator coefficients
    float z1, z2;     // State variables
} BiquadFilter;

void Biquad_init(BiquadFilter *filter, float b0, float b1, float b2, float a1, float a2);
float Biquad_process(BiquadFilter *filter, float input);
void Biquad_design_lowpass(BiquadFilter *filter, float cutoff, float Q);
void Biquad_design_highpass(BiquadFilter *filter, float cutoff, float Q);

typedef struct {
    BiquadFilter biquad;
    float cutoff;
    float q;
} LowpassFilter;

float Lowpass_process(LowpassFilter *filter, float input);
void Lowpass_set_cutoff(LowpassFilter *filter, float cutoff);
void Lowpass_set_q(LowpassFilter *filter, float Q);