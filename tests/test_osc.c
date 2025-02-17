#include <criterion/criterion.h>
#include <math.h>
#include "osc.h"
#include "config.h"

Test(oscillator, create_and_frequency) {
    double freq = 440.0;
    size_t wt_length = 1024;
    Osc *osc = osc_create(WAVEFORM_SINE, wt_length, freq);
    cr_assert_not_null(osc, "osc_create returned NULL");
    cr_assert_float_eq(osc->phase, 0.0, 0.0001, "Initial phase should be 0");
    
    double expected_phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
    cr_assert_float_eq(osc->phase_inc, expected_phase_inc, 0.0001, "Phase increment incorrect");

    cr_assert_not_null(osc->wt, "Oscillator wavetable is NULL");
    cr_assert_eq(osc->wt->type, WAVEFORM_SINE, "Expected wavetable type WAVEFORM_SINE");
    cr_assert_eq(osc->wt->length, wt_length, "Wavetable length incorrect");

    /* Update oscillator frequency */
    osc_set_freq(osc, 880.0);
    double new_expected_phase_inc = (TABLE_SIZE * 880.0) / SAMPLE_RATE;
    cr_assert_float_eq(osc->phase_inc, new_expected_phase_inc, 0.0001, "Phase increment after frequency update incorrect");

    osc_destroy(osc);
}
