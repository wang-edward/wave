#pragma once
#include "vec.h"
#include "wavetable.h"

typedef struct {
    double phase;     // current phase (in table index units)
    double phase_inc; // phase increment per sample
    Wavetable *wt;    // pointer to this oscillator's wavetable
} Osc;

Osc *osc_create(Waveform type, size_t length, double freq);
void osc_set_freq(Osc *osc, double freq);
void osc_destroy(Osc *osc);

#define oscvec_create() vec_create(sizeof(Osc *), (ElemDestroyFunc)osc_destroy)
#define oscvec_push(vec, osc) vec_push_back((vec), &(osc))
#define oscvec_get(vec, index) (((Osc **)((vec)->data))[index])
#define oscvec_size(vec) ((vec)->size)
#define oscvec_destroy(vec) vec_destroy(vec)
