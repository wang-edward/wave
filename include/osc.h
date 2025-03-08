#pragma once

typedef struct {
    double phase;     // current phase (in table index units)
    double phase_inc; // phase increment per sample
    int wt_index;     // index into the shared wavetable array
} Osc;

Osc Osc_create(int wt_index, double freq);

void Osc_set_freq(Osc *osc, double freq);
