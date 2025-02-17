#pragma once
#include <stddef.h>
#include "osc.h"

typedef struct {
    struct Osc *data;
    size_t size;
    size_t capacity;
} OscVec;

OscVec *oscvec_create(size_t initial_capacity);

void oscvec_push(OscVec *vec, struct Osc osc);

void oscvec_destroy(OscVec *vec);
