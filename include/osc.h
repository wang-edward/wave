#pragma once
struct Osc {
    double phase;      // current phase (in table index units)
    double phase_inc;  // phase increment per sample
};

void osc_set_freq(struct Osc *osc, double freq);
