#include "osc.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void osc_set_freq(struct Osc *osc, double freq) {
    osc->phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
}

void osc_init_wavetable(struct Osc *osc) {
    osc->wavetable = malloc(TABLE_SIZE * sizeof(float));
    if (!osc->wavetable) {
        fprintf(stderr, "Failed to allocate memory for wavetable\n");
        exit(1);
    }
    // For now, initialize the wavetable as a sine wave.
    for (int i = 0; i < TABLE_SIZE; i++) {
        osc->wavetable[i] = (float)sin(2.0 * M_PI * i / TABLE_SIZE);
    }
}
