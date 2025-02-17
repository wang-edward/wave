#include "osc.h"
#include "config.h"

void osc_set_freq(struct Osc *osc, double freq) {
    osc->phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
}
