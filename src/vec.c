#include "vec.h"
#include <stdlib.h>

OscVec *oscvec_create(size_t initial_capacity) {
    OscVec *vec = malloc(sizeof(OscVec));
    vec->data = malloc(initial_capacity * sizeof(struct Osc));
    vec->size = 0;
    vec->capacity = initial_capacity;
    return vec;
}

void oscvec_push(OscVec *vec, struct Osc osc) {
    if (vec->size >= vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, vec->capacity * sizeof(struct Osc));
    }
    vec->data[vec->size++] = osc;
}

void oscvec_destroy(OscVec *vec) {
    // Free each oscillator's wavetable.
    for (size_t i = 0; i < vec->size; i++) {
        free(vec->data[i].wavetable);
    }
    free(vec->data);
    free(vec);
}
