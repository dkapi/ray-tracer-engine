#ifndef DARRAY_H
#define DARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SIZE 10

typedef struct {
    void **data;  // Array of void pointers for generic data types
    int size;     // Current allocated size
    int count;    // Number of elements currently in the array
} darray;

// Function prototypes
darray* darray_create();
void darray_destroy(darray *arr);
void darray_add(darray *arr, void *element);
void* darray_get(darray *arr, int index);
void darray_set(darray *arr, int index, void *element);
void* darray_remove(darray *arr, int index);
void darray_print(darray *arr);
int darray_size(const darray *arr);  // Add this line for the size function

#endif /* DARRAY_H */
