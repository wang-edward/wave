#pragma once
#include <stdlib.h>

typedef enum { WAVEFORM_SINE, WAVEFORM_SAW, WAVEFORM_SQUARE, WAVEFORM_TRIANGLE } Waveform;

typedef struct {
    float *data;
    size_t length;
    Waveform type;
} Wavetable;

Wavetable *Wavetable_create(Waveform type, size_t length);
void Wavetable_destroy(Wavetable *wt);
