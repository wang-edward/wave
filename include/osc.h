#pragma once

struct Osc {
    double phase;      // current phase (in table index units)
    double phase_inc;  // phase increment per sample
    float *wavetable;  // pointer to this oscillator's wavetable
};

void osc_set_freq(struct Osc *osc, double freq);
void osc_init_wavetable(struct Osc *osc);
