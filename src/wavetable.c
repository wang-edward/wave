#include "wavetable.h"
#include "wavetable.h"
#include <assert.h>
#include <criterion/criterion.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

Wavetable *wavetable_create(Waveform type, size_t length) {
    cr_assert(length > 0);

    Wavetable *wt = malloc(sizeof(Wavetable));
    if (!wt) {
        fprintf(stderr, "Failed to allocate memory for Wavetable\n");
        exit(1);
    }
    wt->data = malloc(length * sizeof(float));
    if (!wt->data) {
        fprintf(stderr, "Failed to allocate memory for Wavetable data\n");
        free(wt);
        exit(1);
    }
    wt->length = length;
    wt->type = type;

    for (size_t i = 0; i < length; i++) {
        float value = 0.0f;
        switch (type) {
        case WAVEFORM_SINE:
            value = (float)sin(2.0 * M_PI * i / length);
            break;
        case WAVEFORM_SQUARE:
            value = (i < length / 2) ? 1.0f : -1.0f;
            break;
        case WAVEFORM_SAW:
            value = 2.0f * i / length - 1.0f;
            break;
        case WAVEFORM_TRIANGLE:
            if (i < length / 2)
                value = 2.0f * i / (length / 2) - 1.0f;
            else
                value = 1.0f - 2.0f * (i - length / 2) / (length / 2);
            break;
        default:
            cr_assert(false);
            break;
        }
        wt->data[i] = value;
    }
    return wt;
}

void wavetable_destroy(Wavetable *wt) {
    cr_assert(wt);
    free(wt->data);
    free(wt);
}
