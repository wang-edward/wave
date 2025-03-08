#pragma once
#include "config.h"

typedef struct {
    double phase;     // current phase (in table index units)
    double phase_inc; // phase increment per sample
    int wt_index;     // index into the shared wavetable array
} Osc;

// Create an oscillator (here, allocation is done in State_create)
Osc Osc_create(int wt_index, double freq);

// Update an oscillator's frequency.
void Osc_set_freq(Osc *osc, double freq);

// (No Osc_destroy is needed since the array is owned by State.)
