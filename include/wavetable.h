#pragma once
#include <stdlib.h>

typedef enum { WAVEFORM_SINE, WAVEFORM_SAW, WAVEFORM_SQUARE, WAVEFORM_TRIANGLE, WAVEFORM_CUSTOM } Waveform;

typedef struct {
    float *data;
    size_t length;
    Waveform type;
} Wavetable;

Wavetable *Wavetable_create(Waveform type, size_t length);
void Wavetable_destroy(Wavetable *wt);

int Wavetable_load(Wavetable *wt, const char *filename);