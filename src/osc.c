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

OscVec *OscVec_create(void) {
    OscVec *ov = malloc(sizeof(OscVec));
    ov->vec = Vec_create(sizeof(Osc *), (ElemDestroyFunc)Osc_destroy);
    return ov;
}

void OscVec_push(OscVec *ov, Osc *osc) { Vec_push_back(ov->vec, &osc); }

Osc *OscVec_get(const OscVec *ov, size_t index) { return ((Osc **)(ov->vec->data))[index]; }

size_t OscVec_size(const OscVec *ov) { return ov->vec->size; }

void OscVec_destroy(OscVec *ov) {
    Vec_destroy(ov->vec);
    free(ov);
}
