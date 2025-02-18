#include <assert.h>
#include <stdlib.h>
#include "osc.h"
#include "config.h"

Osc *osc_create(Waveform type, size_t length, double freq) {
    Osc *osc = malloc(sizeof(Osc));
    osc->phase = 0.0;
    osc->phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
    osc->wt = wavetable_create(type, length);
    return osc;
}

void osc_set_freq(Osc *osc, double freq) { 
    assert(freq > 0);
    assert(freq < SAMPLE_RATE / 2.0); // nyquist
    osc->phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE; 
}

void osc_destroy(Osc *osc) {
    assert(osc);
    wavetable_destroy(osc->wt);
    free(osc);
}
