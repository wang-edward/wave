#include <criterion/criterion.h>
#include "config.h"
#include "vec.h"
#include "osc.h"

Test(vec, int) {
    Vec *vec = vec_create(sizeof(int), NULL);
    cr_assert_not_null(vec, "vec_create returned NULl");
    cr_assert_eq(vec->size, 0, "initial vec size should be 0");

    const int numInts = 10;
    for (int i = 0; i < numInts; i++) {
        vec_push_back(vec, &i);
        cr_assert_eq(vec->size, (size_t)(i + 1), "After push %d, vec size should be %d", i + 1, i + 1);
    }

    // Verify that each stored int matches its expected value.
    for (size_t i = 0; i < vec->size; i++) {
        int value = 0;
        vec_get(vec, i, &value);
        cr_assert_eq(value, (int)i, "Expected element at index %zu to be %d", i, (int)i);
    }

    vec_destroy(vec);
}

Test(vec, osc) {
    // Create a vector for Osc pointers.
    // We are storing Osc* elements, so the element size is sizeof(Osc*).
    // The destroy function is set to osc_destroy so that each Osc is cleaned up.
    Vec *vec = vec_create(sizeof(Osc*), (ElemDestroyFunc)osc_destroy);
    cr_assert_not_null(vec, "vec_create returned NULL for Osc vector");
    cr_assert_eq(vec->size, 0, "Initial Osc vector size should be 0");

    const int numOsc = 1;
    for (int i = 0; i < numOsc; i++) {
        // Create an oscillator with varying parameters.
        Osc *osc = osc_create(WAVEFORM_SINE + (i % 2), 256 * (i + 1), 220.0 + (i * 10));
        vec_push_back(vec, &osc);
        cr_assert_eq(vec->size, (size_t)(i + 1), "After push %d, Osc vector size should be %d", i + 1, i + 1);
    }


    // Verify that each stored Osc pointer is not NULL.
    for (size_t i = 0; i < vec->size; i++) {
        Osc *osc_ptr = NULL;
        vec_get(vec, i, &osc_ptr);
        cr_assert_not_null(osc_ptr, "Osc element at index %zu should not be NULL", i);
        // Optionally, you can add more detailed tests of the Osc's properties here.
    }

    vec_destroy(vec);
}

Test(vec, oscvec) {
    // Create the oscillator vector.
    Vec *vec = oscvec_create();
    cr_assert_not_null(vec, "oscvec_create returned NULL");
    cr_assert_eq(oscvec_size(vec), 0, "Initial oscvec size should be 0");

    // Create first oscillator: Sine, wavetable length 256, frequency 440 Hz.
    Osc *osc1 = osc_create(WAVEFORM_SINE, 256, 440.0);
    cr_assert_not_null(osc1, "osc_create returned NULL for osc1");
    oscvec_push(vec, osc1);
    cr_assert_eq(oscvec_size(vec), 1, "After pushing osc1, size should be 1");

    // Retrieve the first oscillator.
    Osc *retrieved1 = oscvec_get(vec, 0);
    cr_assert_not_null(retrieved1, "Retrieved oscillator is NULL");
    cr_assert_float_eq(retrieved1->phase, 0.0, 0.0001, "Oscillator initial phase should be 0");
    double expected_phase_inc1 = (TABLE_SIZE * 440.0) / SAMPLE_RATE;
    cr_assert_float_eq(retrieved1->phase_inc, expected_phase_inc1, 0.0001,
                         "Phase increment for osc1 is incorrect");
    cr_assert_not_null(retrieved1->wt, "Wavetable in osc1 is NULL");
    cr_assert_eq(retrieved1->wt->type, WAVEFORM_SINE, "Wavetable type for osc1 is not WAVEFORM_SINE");
    cr_assert_eq(retrieved1->wt->length, 256, "Wavetable length for osc1 is not 256");

    // Create second oscillator: Saw, wavetable length 512, frequency 220 Hz.
    Osc *osc2 = osc_create(WAVEFORM_SAW, 512, 220.0);
    cr_assert_not_null(osc2, "osc_create returned NULL for osc2");
    oscvec_push(vec, osc2);
    cr_assert_eq(oscvec_size(vec), 2, "After pushing osc2, size should be 2");

    // Retrieve the second oscillator.
    Osc *retrieved2 = oscvec_get(vec, 1);
    cr_assert_not_null(retrieved2, "Retrieved second oscillator is NULL");
    cr_assert_eq(retrieved2->wt->type, WAVEFORM_SAW, "Wavetable type for osc2 is not WAVEFORM_SAW");
    cr_assert_eq(retrieved2->wt->length, 512, "Wavetable length for osc2 is not 512");

    // Clean up: this should free both oscillators.
    oscvec_destroy(vec);
}
