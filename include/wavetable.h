#include "stddef.h"

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
