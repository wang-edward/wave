#include <criterion/criterion.h>
#include "vec.h"
#include "osc.h"

Test(vec, int) {
    Vec *vec = Vec_create(sizeof(int), NULL);
    cr_assert_not_null(vec, "Vec_create returned NULl");
    cr_assert_eq(vec->size, 0, "initial vec size should be 0");

    const int numInts = 10;
    for (int i = 0; i < numInts; i++) {
        Vec_push_back(vec, &i);
        cr_assert_eq(vec->size, (size_t)(i + 1), "After push %d, vec size should be %d", i + 1, i + 1);
    }

    // Verify that each stored int matches its expected value.
    for (size_t i = 0; i < vec->size; i++) {
        int value = 0;
        Vec_get(vec, i, &value);
        cr_assert_eq(value, (int)i, "Expected element at index %zu to be %d", i, (int)i);
    }

    Vec_destroy(vec);
}

Test(vec, osc) {
    // Create a vector for Osc pointers.
    // We are storing Osc* elements, so the element size is sizeof(Osc*).
    // The destroy function is set to Osc_destroy so that each Osc is cleaned up.
    Vec *vec = Vec_create(sizeof(Osc*), (ElemDestroyFunc)Osc_destroy);
    cr_assert_not_null(vec, "Vec_create returned NULL for Osc vector");
    cr_assert_eq(vec->size, 0, "Initial Osc vector size should be 0");

    const int numOsc = 1;
    for (int i = 0; i < numOsc; i++) {
        // Create an oscillator with varying parameters.
        Osc *osc = Osc_create(WAVEFORM_SINE + (i % 2), 256 * (i + 1), 220.0 + (i * 10));
        Vec_push_back(vec, &osc);
        cr_assert_eq(vec->size, (size_t)(i + 1), "After push %d, Osc vector size should be %d", i + 1, i + 1);
    }


    // Verify that each stored Osc pointer is not NULL.
    for (size_t i = 0; i < vec->size; i++) {
        Osc *osc_ptr = NULL;
        Vec_get(vec, i, &osc_ptr);
        cr_assert_not_null(osc_ptr, "Osc element at index %zu should not be NULL", i);
        // Optionally, you can add more detailed tests of the Osc's properties here.
    }

    Vec_destroy(vec);
}
