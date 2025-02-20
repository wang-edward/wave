#include "osc.h"
#include "config.h"
#include <assert.h>
#include <stdlib.h>

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

OscVec *oscvec_create(void) {
    OscVec *ov = malloc(sizeof(OscVec));
    ov->vec = vec_create(sizeof(Osc *), (ElemDestroyFunc)osc_destroy);
    return ov;
}

void oscvec_push(OscVec *ov, Osc *osc) { vec_push_back(ov->vec, &osc); }

Osc *oscvec_get(const OscVec *ov, size_t index) { return ((Osc **)(ov->vec->data))[index]; }

size_t oscvec_size(const OscVec *ov) { return ov->vec->size; }

void oscvec_destroy(OscVec *ov) {
    vec_destroy(ov->vec);
    free(ov);
}
