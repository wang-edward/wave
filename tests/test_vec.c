#include <criterion/criterion.h>
#include "vec.h"
#include "osc.h"

Test(oscvec, push_resize_and_destroy) {
    OscVec *vec = oscvec_create();
    cr_assert_not_null(vec, "oscvec_create returned NULL");
    cr_assert_eq(vec->size, 0, "Initial oscvec size should be 0");

    // We'll push 10 oscillators to force a resize (initial capacity is 4)
    const int numOsc = 10;
    for (int i = 0; i < numOsc; i++) {
        // Use different parameters to differentiate each oscillator.
        Osc *osc = osc_create(WAVEFORM_SAW + (i % 3), 256 * (i + 1), 220.0 + (i * 10));
        oscvec_push(vec, osc);
        cr_assert_eq(vec->size, i + 1, "After push %d, oscvec size should be %d", i + 1, i + 1);
    }

    // Verify that the oscillators were stored correctly
    for (int i = 0; i < numOsc; i++) {
        // You could add more detailed tests here if osc has identifiable properties.
        cr_assert_not_null(vec->data[i], "Element %d of oscvec should not be NULL", i);
    }

    // Clean up (oscvec_destroy is expected to free all contained oscillators and the vector)
    oscvec_destroy(vec);
}
