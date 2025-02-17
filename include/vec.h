#pragma once
#include <stddef.h>
#include "osc.h"

typedef struct {
    struct Osc *data;
    size_t size;
    size_t capacity;
} OscVec;
