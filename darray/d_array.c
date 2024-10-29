#include "d_array.h"

// Function to create a new dynamic array
darray* darray_create() {
    darray *arr = (darray *)malloc(sizeof(darray));
    arr->data = (void **)malloc(DEFAULT_SIZE * sizeof(void *));
    arr->size = DEFAULT_SIZE;
    arr->count = 0;
    return arr;
}

// Function to free the dynamic array
void darray_destroy(darray *arr) {
    free(arr->data);
    free(arr);
}

static void darray_resize(darray *arr) {
    void **new_data = (void **)realloc(arr->data, arr->size * 2 * sizeof(void *));
    if (new_data == NULL) {
        printf("Error reallocating memory for darray.\n");
        exit(EXIT_FAILURE);
    }
    arr->data = new_data;
    arr->size *= 2;
}

// Function to add an element to the dynamic array
void darray_add(darray *arr, void *element) {
    if (arr->count == arr->size) {
        darray_resize(arr);
    }
    arr->data[arr->count++] = element;
}

// Function to get an element from the dynamic array
void* darray_get(darray *arr, int index) {
    if (index < 0 || index >= arr->count) {
        return NULL; // Index out of bounds
    }
    return arr->data[index];
}

// Function to set an element in the dynamic array
void darray_set(darray *arr, int index, void *element) {
    if (index >= 0 && index < arr->count) {
        arr->data[index] = element;
    }
}

// Function to remove an element from the dynamic array
void* darray_remove(darray *arr, int index) {
    if (index < 0 || index >= arr->count) {
        return NULL; // Index out of bounds
    }
    
    void *removed_element = arr->data[index];

    for (int i = index + 1; i < arr->count; i++) {
        arr->data[i - 1] = arr->data[i];
    }
    arr->count--;

    return removed_element;
}

// Returns the number of elements currently in the dynamic array
int darray_size(const darray *arr) {
    return arr ? arr->count : 0;
}

// Function to print the elements of the dynamic array
void darray_print(darray *arr) {
    printf("darray[\n");
    for (int i = 0; i < arr->count; i++) {
        printf("  %p\n", arr->data[i]); // Print address of the elements
    }
    printf("]\n");
}
