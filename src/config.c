#include "config.h"

float clamp_SR(float freq) {
    if (freq < 0.0f)
        return 0.0f;
    if (freq > SAMPLE_RATE / 2.0f)
        return SAMPLE_RATE / 2.0f;
    return freq;
}

float clamp_unit(float value) {
    if (value < 0.0f)
        return 0.0f;
    if (value > 1.0f)
        return 1.0f;
    return value;
}

float scale_unit(float value, float min, float max) {
    return value / (max - min);
}