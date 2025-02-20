#pragma once
#include <stddef.h>

#define VEC_INIT_CAPACITY 4

typedef void (*ElemDestroyFunc)(void *);

typedef struct {
    void *data;                   // Pointer to the array's data
    size_t element_size;          // Size of each element
    size_t size;                  // Number of elements currently stored
    size_t capacity;              // Allocated capacity
    ElemDestroyFunc destroy_func; // Function pointer for element deletion
} Vec;

Vec *Vec_create(size_t element_size, ElemDestroyFunc destroy_func);
void Vec_push_back(Vec *vec, const void *element);
void Vec_get(const Vec *vec, size_t index, void *out_element);
void Vec_set(Vec *vec, size_t index, const void *element);
void Vec_pop_back(Vec *vec);
void Vec_destroy(Vec *vec);
