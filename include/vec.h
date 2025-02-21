#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

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

/*
 * DECLARE_VEC_TYPE(Type, Name, DestroyFunc)
 *
 *  - Type: the actual type to be stored (e.g., Osc* or int)
 *  - Name: the short name appended to "Vec_" to create the new type name.
 *  - DestroyFunc: the function to destroy an element of type Type.
 *
 * This macro creates:
 *  - A typedef for a structure wrapping a generic Vec pointer.
 *  - Inline functions for creating, destroying, pushing back, and getting elements.
 */
#define DECLARE_VEC_TYPE(Type, Name, DestroyFunc)                         \
typedef struct {                                                            \
    Vec *vec;                                                               \
} Vec_##Name;                                                               \
                                                                            \
static inline Vec_##Name *Vec_##Name##_create(void) {                       \
    Vec_##Name *typed = malloc(sizeof(Vec_##Name));                         \
    assert(typed && "Failed to allocate typed vector");                     \
    typed->vec = Vec_create(sizeof(Type), DestroyFunc);                     \
    return typed;                                                           \
}                                                                           \
                                                                            \
static inline void Vec_##Name##_destroy(Vec_##Name *typed) {                \
    Vec_destroy(typed->vec);                                                \
    free(typed);                                                            \
}                                                                           \
                                                                            \
static inline void Vec_##Name##_push_back(Vec_##Name *typed, Type item) {     \
    Vec_push_back(typed->vec, &item);                                         \
}                                                                           \
                                                                            \
static inline Type Vec_##Name##_get(const Vec_##Name *typed, size_t index) {  \
    Type temp;                                                              \
    Vec_get(typed->vec, index, &temp);                                        \
    return temp;                                                            \
}
