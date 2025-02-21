#include "osc.h"

Osc Osc_create(int wt_index, double freq) {
    Osc osc;
    osc.phase = 0.0;
    osc.wt_index = wt_index;
    osc.phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
    return osc;
}

void Osc_set_freq(Osc *osc, double freq) {
    if (freq <= 0) {
        osc->phase_inc = 0.0;
    } else {
        osc->phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
    }
}
