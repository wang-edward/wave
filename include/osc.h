#pragma once
#include "wavetable.h"

typedef struct {
    double phase;     // current phase (in table index units)
    double phase_inc; // phase increment per sample
    Wavetable *wt;    // pointer to this oscillator's wavetable
} Osc;

Osc *osc_create(Waveform type, size_t length, double freq);
void osc_set_freq(Osc *osc, double freq);
void osc_destroy(Osc *osc);
