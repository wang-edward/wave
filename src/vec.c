#include "vec.h"
#include <stdlib.h>

OscVec *oscvec_create() {
    OscVec *vec = malloc(sizeof(OscVec));
    vec->data = malloc(OSCVEC_INITIAL_CAPACITY * sizeof(Osc *));
    vec->size = 0;
    vec->capacity = OSCVEC_INITIAL_CAPACITY;
    return vec;
}

void oscvec_push(OscVec *vec, Osc *osc) {
    if (vec->size >= vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, vec->capacity * sizeof(Osc *));
    }
    vec->data[vec->size++] = osc;
}

void oscvec_destroy(OscVec *vec) {
    // Free each oscillator's wavetable.
    for (size_t i = 0; i < vec->size; i++) {
        osc_destroy(vec->data[i]);
    }
    free(vec->data);
    free(vec);
}
