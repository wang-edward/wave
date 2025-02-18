#include "vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

Vec *vec_create(size_t element_size, ElemDestroyFunc destroy_func) {
    assert(element_size > 0);
    assert(destroy_func);

    Vec *vec = malloc(sizeof(Vec));
    if (!vec) {
        fprintf(stderr, "Failed to allocate Vec\n");
        exit(EXIT_FAILURE);
    }
    vec->element_size = element_size;
    vec->size = 0;
    vec->capacity = VEC_INIT_CAPACITY;
    vec->data = malloc(vec->capacity * element_size);
    if (!vec->data) {
        fprintf(stderr, "Failed to allocate Vec data\n");
        free(vec);
        exit(EXIT_FAILURE);
    }
    vec->destroy_func = destroy_func;
    return vec;
}

void vec_delete(Vec *vec) {
    assert(vec);

    if (vec->destroy_func) {
        for (size_t i = 0; i < vec->size; i++) {
            void *elem_ptr = (char *)vec->data + i * vec->element_size;
            vec->destroy_func(elem_ptr);
        }
    }
    free(vec->data);
    free(vec);
}

void vec_push_back(Vec *vec, const void *element) {
    assert(vec);
    assert(element);

    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        void *new_data = realloc(vec->data, vec->capacity * vec->element_size);
        if (!new_data) {
            fprintf(stderr, "Failed to reallocate memory\n");
            exit(EXIT_FAILURE);
        }
        vec->data = new_data;
    }
    // Copy the element into the allocated space.
    memcpy((char *)vec->data + vec->size * vec->element_size, element, vec->element_size);
    vec->size++;
}

void vec_get(const Vec *vec, size_t index, void *out_element) {
    assert(vec);
    assert(index < vec->size);
    assert(out_element);

    memcpy(out_element, (char *)vec->data + index * vec->element_size, vec->element_size);
}

void vec_set(Vec *vec, size_t index, const void *element) {
    assert(vec);
    assert(element);
    assert(index < vec->size);

    memcpy((char *)vec->data + index * vec->element_size, element, vec->element_size);
}

void vec_pop_back(Vec *vec) {
    assert(vec);
    assert(vec->size > 0);
    vec->size--;
}
