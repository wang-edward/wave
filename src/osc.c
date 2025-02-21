#include "osc.h"
#include "config.h"
#include <assert.h>
#include <stdlib.h>

Osc *Osc_create(Waveform type, size_t length, double freq) {
    Osc *osc = malloc(sizeof(Osc));
    osc->phase = 0.0;
    osc->phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
    osc->wt = Wavetable_create(type, length);
    return osc;
}

void Osc_set_freq(Osc *osc, double freq) {
    assert(freq > 0);
    assert(freq < SAMPLE_RATE / 2.0); // nyquist
    osc->phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
}

void Osc_destroy(Osc *osc) {
    assert(osc);
    Wavetable_destroy(osc->wt);
    free(osc);
}
