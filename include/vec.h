#pragma once
#include "osc.h"
#include <stddef.h>

#define OSCVEC_INITIAL_CAPACITY 4

typedef struct {
    Osc **data;
    size_t size;
    size_t capacity;
} OscVec;

OscVec *oscvec_create();
void oscvec_push(OscVec *vec, Osc *osc);
void oscvec_destroy(OscVec *vec);
