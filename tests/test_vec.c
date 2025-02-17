#include <criterion/criterion.h>
#include "vec.h"
#include "osc.h"

Test(oscvec, push_and_destroy) {
    OscVec *vec = oscvec_create();
    cr_assert_not_null(vec, "oscvec_create returned NULL");
    cr_assert_eq(vec->size, 0, "Initial oscvec size should be 0");

    Osc *osc1 = osc_create(WAVEFORM_SAW, 256, 220.0);
    Osc *osc2 = osc_create(WAVEFORM_TRIANGLE, 512, 330.0);

    oscvec_push(vec, osc1);
    cr_assert_eq(vec->size, 1, "oscvec size should be 1 after first push");
    oscvec_push(vec, osc2);
    cr_assert_eq(vec->size, 2, "oscvec size should be 2 after second push");
    oscvec_push(vec, osc1);
    cr_assert_eq(vec->size, 3, "oscvec size should be 3 after third push");
    oscvec_push(vec, osc1);
    cr_assert_eq(vec->size, 4, "oscvec size should be 4 after fourth push");
    oscvec_push(vec, osc1);
    cr_assert_eq(vec->size, 5, "oscvec size should be 5 after fifth push");
    oscvec_push(vec, osc1);
    cr_assert_eq(vec->size, 6, "oscvec size should be 6 after sixth push");
    oscvec_push(vec, osc2);
    cr_assert_eq(vec->size, 7, "oscvec size should be 7 after seventh push");

    cr_assert_eq(vec->data[0], osc1, "First element of oscvec should be osc1");
    cr_assert_eq(vec->data[1], osc2, "Second element of oscvec should be osc2");
    cr_assert_eq(vec->data[2], osc1, "Third element of oscvec should be osc1");
    cr_assert_eq(vec->data[3], osc1, "Fourth element of oscvec should be osc1");
    cr_assert_eq(vec->data[4], osc1, "Fifth element of oscvec should be osc1");
    cr_assert_eq(vec->data[5], osc1, "Sixth element of oscvec should be osc1");
    cr_assert_eq(vec->data[6], osc2, "Seventh element of oscvec should be osc1");

    /* oscvec_destroy frees the oscillators and vector storage */
    oscvec_destroy(vec);
}
