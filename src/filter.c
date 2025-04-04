#include <math.h>
#include "filter.h"
#include "config.h"

// Initialize the biquad filter
void Biquad_init(BiquadFilter *filter, float b0, float b1, float b2, float a1, float a2) {
    filter->b0 = b0;
    filter->b1 = b1;
    filter->b2 = b2;
    filter->a1 = a1;
    filter->a2 = a2;
    filter->z1 = 0.0f;
    filter->z2 = 0.0f;
}

float Biquad_process(BiquadFilter *filter, float input) {
    float output = filter->b0 * input + filter->z1;
    filter->z1 = filter->b1 * input + filter->z2 - filter->a1 * output;
    filter->z2 = filter->b2 * input - filter->a2 * output;
    return output;
}

void Biquad_design_lowpass(BiquadFilter *filter, float cutoff, float Q) {
    float omega = 2.0f * M_PI * cutoff / SAMPLE_RATE;
    float sn = sinf(omega);
    float cs = cosf(omega);
    float alpha = sn / (2.0f * Q);

    float b0 = (1.0f - cs) / 2.0f;
    float b1 = 1.0f - cs;
    float b2 = (1.0f - cs) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cs;
    float a2 = 1.0f - alpha;

    filter->b0 = b0 / a0;
    filter->b1 = b1 / a0;
    filter->b2 = b2 / a0;
    filter->a1 = a1 / a0;
    filter->a2 = a2 / a0;
    // Reset state variables
    filter->z1 = 0.0f;
    filter->z2 = 0.0f;
}

void Biquad_design_highpass(BiquadFilter *filter, float cutoff, float Q) {
    float omega = 2.0f * M_PI * cutoff / SAMPLE_RATE;
    float sn = sinf(omega);
    float cs = cosf(omega);
    float alpha = sn / (2.0f * Q);

    float b0 = (1.0f + cs) / 2.0f;
    float b1 = -(1.0f + cs);
    float b2 = (1.0f + cs) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cs;
    float a2 = 1.0f - alpha;

    filter->b0 = b0 / a0;
    filter->b1 = b1 / a0;
    filter->b2 = b2 / a0;
    filter->a1 = a1 / a0;
    filter->a2 = a2 / a0;
    // Reset state variables
    filter->z1 = 0.0f;
    filter->z2 = 0.0f;
}

float Lowpass_process(LowpassFilter *filter, float input) {
    return Biquad_process(&filter->biquad, input);
}

void Lowpass_set_cutoff(LowpassFilter *filter, float cutoff) {
    filter->cutoff = cutoff;
    Biquad_design_lowpass(&filter->biquad, filter->cutoff, filter->q);
}

void Lowpass_set_q(LowpassFilter *filter, float Q) {
    
}