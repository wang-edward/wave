#pragma once
#include "stddef.h"
#include "vec.h"

typedef enum {
    WAVEFORM_SINE,
    WAVEFORM_SQUARE,
    WAVEFORM_SAW,
    WAVEFORM_TRIANGLE,
    WAVEFORM_CUSTOM // Option to use custom waveform data
} Waveform;

typedef struct {
    float *data;
    size_t length; // Number of samples in the wavetable.
    Waveform type;
} Wavetable;

Wavetable *wavetable_create(Waveform type, size_t length);
void wavetable_set_custom(Wavetable *wt, float *data, size_t length);
void wavetable_destroy(Wavetable *wt);

typedef struct {
    Vec *vec;
} WtVec;

WtVec *WtVec_create(void);
void WtVec_push(WtVec *wv, Wavetable *wt);
Wavetable *WtVec_get(const WtVec *wv, size_t index);
size_t WtVec_size(const WtVec *wv);
void WtVec_destroy(WtVec *wv);
