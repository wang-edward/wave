#include <criterion/criterion.h>
#include "config.h"
#include "osc.h"
#include "vec.h"

Test(VecOscPtr, create_push_get_destroy) {
    Vec_OscPtr *ov = Vec_OscPtr_create();
    cr_assert_not_null(ov, "Vec_OscPtr_create returned NULL");
    cr_assert_eq(ov->vec->size, 0, "Initial vector size should be 0");

    // Create two oscillators with different properties.
    Osc *osc1 = Osc_create(WAVEFORM_SINE, 256, 440.0);
    cr_assert_not_null(osc1, "Osc_create returned NULL for osc1");
    Osc *osc2 = Osc_create(WAVEFORM_SAW, 512, 220.0);
    cr_assert_not_null(osc2, "Osc_create returned NULL for osc2");

    // Push the oscillators into the typed vector.
    Vec_OscPtr_push_back(ov, osc1);
    cr_assert_eq(ov->vec->size, 1, "After pushing osc1, size should be 1");
    Vec_OscPtr_push_back(ov, osc2);
    cr_assert_eq(ov->vec->size, 2, "After pushing osc2, size should be 2");

    // Retrieve the stored oscillators.
    Osc *retrieved1 = Vec_OscPtr_get(ov, 0);
    Osc *retrieved2 = Vec_OscPtr_get(ov, 1);
    cr_assert_not_null(retrieved1, "Retrieved oscillator 1 should not be NULL");
    cr_assert_not_null(retrieved2, "Retrieved oscillator 2 should not be NULL");

    // Check that the retrieved oscillators have the expected properties.
    cr_assert_eq(retrieved1->wt->type, WAVEFORM_SINE, "Expected osc1 to have WAVEFORM_SINE");
    cr_assert_eq(retrieved1->wt->length, 256, "Expected osc1 wavetable length to be 256");

    cr_assert_eq(retrieved2->wt->type, WAVEFORM_SAW, "Expected osc2 to have WAVEFORM_SAW");
    cr_assert_eq(retrieved2->wt->length, 512, "Expected osc2 wavetable length to be 512");

    // Verify phase and phase_inc for osc1.
    cr_assert_float_eq(retrieved1->phase, 0.0, 0.0001, "Initial phase for osc1 should be 0");
    double expected_phase_inc1 = (TABLE_SIZE * 440.0) / SAMPLE_RATE;
    cr_assert_float_eq(retrieved1->phase_inc, expected_phase_inc1, 0.0001,
                         "Phase increment for osc1 is incorrect");

    Vec_OscPtr_destroy(ov);
}
