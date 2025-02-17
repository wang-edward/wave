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

Test(wavetable, custom_waveform) {
    size_t length = 5;
    Wavetable *wt = wavetable_create(WAVEFORM_SINE, length);
    /* Assume wavetable_set_custom replaces the data and sets type to WAVEFORM_CUSTOM */
    float custom_data[5] = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
    wavetable_set_custom(wt, custom_data, length);

    for (size_t i = 0; i < length; i++) {
        cr_assert_float_eq(wt->data[i], custom_data[i], 0.0001, "Custom sample at index %zu incorrect", i);
    }
    cr_assert_eq(wt->type, WAVEFORM_CUSTOM, "Expected wavetable type WAVEFORM_CUSTOM");
    
    wavetable_destroy(wt);
}
