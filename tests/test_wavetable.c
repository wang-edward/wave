#include <criterion/criterion.h>
#include <math.h>
#include "wavetable.h"

Test(wavetable, sine_generation) {
    size_t length = 100;
    Wavetable *wt = wavetable_create(WAVEFORM_SINE, length);
    cr_assert_not_null(wt, "wavetable_create returned NULL for sine");
    cr_assert_eq(wt->length, length, "Expected wavetable length %zu, got %zu", length, wt->length);

    for (size_t i = 0; i < length; i++) {
        float expected = (float)sin(2.0 * M_PI * i / length);
        cr_assert_float_eq(wt->data[i], expected, 0.0001, "Sine sample at index %zu incorrect", i);
    }
    wavetable_destroy(wt);
}

Test(wavetable, square_generation) {
    size_t length = 10;
    Wavetable *wt = wavetable_create(WAVEFORM_SQUARE, length);
    cr_assert_not_null(wt, "wavetable_create returned NULL for square");
    cr_assert_eq(wt->length, length, "Expected wavetable length %zu", length);
    
    for (size_t i = 0; i < length; i++) {
        float expected = (i < length / 2) ? 1.0f : -1.0f;
        cr_assert_float_eq(wt->data[i], expected, 0.0001, "Square sample at index %zu incorrect", i);
    }
    wavetable_destroy(wt);
}

Test(wavetable, saw_generation) {
    size_t length = 10;
    Wavetable *wt = wavetable_create(WAVEFORM_SAW, length);
    cr_assert_not_null(wt, "wavetable_create returned NULL for saw");
    cr_assert_eq(wt->length, length, "Expected wavetable length %zu", length);
    
    for (size_t i = 0; i < length; i++) {
        float expected = 2.0f * i / length - 1.0f;
        cr_assert_float_eq(wt->data[i], expected, 0.0001, "Saw sample at index %zu incorrect", i);
    }
    wavetable_destroy(wt);
}

Test(wavetable, triangle_generation) {
    size_t length = 10;
    Wavetable *wt = wavetable_create(WAVEFORM_TRIANGLE, length);
    cr_assert_not_null(wt, "wavetable_create returned NULL for triangle");
    cr_assert_eq(wt->length, length, "Expected wavetable length %zu", length);
    
    for (size_t i = 0; i < length; i++) {
        float expected;
        if (i < length / 2) {
            expected = 2.0f * i / (length / 2) - 1.0f;
        } else {
            expected = 1.0f - 2.0f * (i - length / 2) / (length / 2);
        }
        cr_assert_float_eq(wt->data[i], expected, 0.0001, "Triangle sample at index %zu incorrect", i);
    }
    wavetable_destroy(wt);
}

// Test(wavetable, custom_waveform) {
//     size_t length = 5;
//     Wavetable *wt = wavetable_create(WAVEFORM_SINE, length);
//     /* Assume wavetable_set_custom replaces the data and sets type to WAVEFORM_CUSTOM */
//     float custom_data[5] = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
//     wavetable_set_custom(wt, custom_data, length);

//     for (size_t i = 0; i < length; i++) {
//         cr_assert_float_eq(wt->data[i], custom_data[i], 0.0001, "Custom sample at index %zu incorrect", i);
//     }
//     cr_assert_eq(wt->type, WAVEFORM_CUSTOM, "Expected wavetable type WAVEFORM_CUSTOM");
//     
//     wavetable_destroy(wt);
// }

// Test the WtVec wrapper: create a vector, push wavetables, retrieve and check them.
Test(wtvec, push_get_and_destroy) {
    // Create the WtVec container.
    WtVec *wv = WtVec_create();
    cr_assert_not_null(wv, "WtVec_create returned NULL");
    cr_assert_eq(WtVec_size(wv), 0, "Initial WtVec size should be 0");

    // Create a sine wavetable (length 100) and push it.
    size_t length1 = 100;
    Wavetable *wt1 = wavetable_create(WAVEFORM_SINE, length1);
    cr_assert_not_null(wt1, "wavetable_create returned NULL for sine");
    WtVec_push(wv, wt1);
    cr_assert_eq(WtVec_size(wv), 1, "After pushing sine wavetable, size should be 1");

    // Create a saw wavetable (length 50) and push it.
    size_t length2 = 50;
    Wavetable *wt2 = wavetable_create(WAVEFORM_SAW, length2);
    cr_assert_not_null(wt2, "wavetable_create returned NULL for saw");
    WtVec_push(wv, wt2);
    cr_assert_eq(WtVec_size(wv), 2, "After pushing saw wavetable, size should be 2");

    // Retrieve the first wavetable and verify its properties.
    Wavetable *retrieved1 = WtVec_get(wv, 0);
    cr_assert_not_null(retrieved1, "Retrieved wavetable 1 is NULL");
    cr_assert_eq(retrieved1->type, WAVEFORM_SINE, "Retrieved wavetable 1 type mismatch");
    cr_assert_eq(retrieved1->length, length1, "Retrieved wavetable 1 length mismatch");

    // Verify a couple of sample values in the sine wavetable.
    for (size_t i = 0; i < length1; i++) {
        float expected = (float)sin(2.0 * M_PI * i / length1);
        cr_assert_float_eq(retrieved1->data[i], expected, 0.0001,
                           "Sine wavetable sample at index %zu incorrect", i);
    }

    // Retrieve the second wavetable and verify its properties.
    Wavetable *retrieved2 = WtVec_get(wv, 1);
    cr_assert_not_null(retrieved2, "Retrieved wavetable 2 is NULL");
    cr_assert_eq(retrieved2->type, WAVEFORM_SAW, "Retrieved wavetable 2 type mismatch");
    cr_assert_eq(retrieved2->length, length2, "Retrieved wavetable 2 length mismatch");

    // Verify a couple of sample values in the saw wavetable.
    for (size_t i = 0; i < length2; i++) {
        float expected = 2.0f * i / length2 - 1.0f;
        cr_assert_float_eq(retrieved2->data[i], expected, 0.0001,
                           "Saw wavetable sample at index %zu incorrect", i);
    }

    // Clean up. This should free both wavetables.
    WtVec_destroy(wv);
}
