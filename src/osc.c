#include "osc.h"
#include "config.h"
#include <stdlib.h>

Osc *osc_create(Waveform type, size_t length, double freq) {
    Osc *osc = malloc(sizeof(Osc));
    osc->phase = 0.0;
    osc->phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
    osc->wt = wavetable_create(type, length);
    return osc;
}

void osc_set_freq(Osc *osc, double freq) { osc->phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE; }

void osc_destroy(Osc *osc) {
    wavetable_destroy(osc->wt);
    free(osc);
}
