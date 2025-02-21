#pragma once
#include "vec.h"
#include "wavetable.h"

typedef struct {
    double phase;     // current phase (in table index units)
    double phase_inc; // phase increment per sample
    Wavetable *wt;    // pointer to this oscillator's wavetable
} Osc;

Osc *Osc_create(Waveform type, size_t length, double freq);
void Osc_set_freq(Osc *osc, double freq);
void Osc_destroy(Osc *osc);

DECLARE_VEC_TYPE(Osc *, OscPtr, (ElemDestroyFunc)Osc_destroy)
