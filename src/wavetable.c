#include "wavetable.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

Wavetable *Wavetable_create(Waveform type, size_t length) {
    assert(length > 0);
    Wavetable *wt = malloc(sizeof(Wavetable));
    if (!wt)
        exit(EXIT_FAILURE);
    wt->data = malloc(length * sizeof(float));
    if (!wt->data) {
        free(wt);
        exit(EXIT_FAILURE);
    }
    wt->length = length;
    wt->type = type;
    for (size_t i = 0; i < length; i++) {
        float value = 0.0f;
        switch (type) {
        case WAVEFORM_SINE:
            value = (float)sin(2.0 * M_PI * i / length);
            break;
        case WAVEFORM_SAW:
            value = 2.0f * i / length - 1.0f;
            break;
        case WAVEFORM_SQUARE:
            value = (i < length / 2) ? 1.0f : -1.0f;
            break;
        case WAVEFORM_TRIANGLE:
            if (i < length / 2)
                value = 2.0f * i / (length / 2) - 1.0f;
            else
                value = 1.0f - 2.0f * (i - length / 2) / (length / 2);
            break;
        }
        wt->data[i] = value;
    }
    return wt;
}

void Wavetable_destroy(Wavetable *wt) {
    if (!wt)
        return;
    free(wt->data);
    free(wt);
}

int Wavetable_load(Wavetable *wt, const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return -1;
    uint32_t length;
    if (fread(&length, sizeof(uint32_t), 1, f) != 1) {
        fclose(f);
        return -1;
    }
    wt->length = length;
    wt->data = (float*)malloc(length * sizeof(float));
    if (!wt->data) {
        fclose(f);
        return -1;
    }
    if (fread(wt->data, sizeof(float), length, f) != length) {
        free(wt->data);
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}