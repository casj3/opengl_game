
#include "ArrayPool.h"

#include <assert.h>

#include "Animation.h"
#include "LoadVAO.h"
#include "Transform.h"
#include "VAO_Data.h"
#include "MathFunctions.h"

static const float kArraySizeFraction = 0.5f;

struct Array_Pool {
    void** arrays;
    ArrayMarkers array_markers;
    // Sizes of the arrays, in bytes
    size_t* sizes;
    // Sizes of the per array types.
    size_t* type_sizes;
    // The number of allocated slots may be larger than the amount of used elements for iterating.
    size_t* capacities;
    size_t num_arrays;
};

static Array_Pool array_pool;

void InitArrayPool(size_t capacity) {
    array_pool.arrays = (void**) calloc(capacity, sizeof(void*));
    array_pool.sizes = (size_t*) calloc(capacity, sizeof(size_t));
    array_pool.type_sizes = (size_t*) calloc(capacity, sizeof(size_t));
    array_pool.capacities = (size_t*) calloc(capacity, sizeof(size_t));
    array_pool.array_markers = NewArrayMarkers(capacity);
    array_pool.num_arrays = capacity;
}

/// Expands the capacity of the array of arrays.
///
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
void ResizeArrays(size_t elementsToAdd) {
    size_t firstNewElement = array_pool.num_arrays;
    array_pool.num_arrays += elementsToAdd;

    array_pool.arrays = (void**) realloc(array_pool.arrays, array_pool.num_arrays * sizeof(void*));
    array_pool.sizes = (size_t*) realloc(array_pool.sizes, array_pool.num_arrays * sizeof(size_t));
    array_pool.type_sizes = (size_t*) realloc(array_pool.type_sizes, array_pool.num_arrays * sizeof(size_t));
    array_pool.capacities = (size_t*) realloc(array_pool.capacities, array_pool.num_arrays * sizeof(size_t));

    // Overwrite the new elements, which hold indeterminate values, with NULL
    // to keep track of which pointers have been assigned arrays.
    memset(array_pool.arrays + firstNewElement, NULL, elementsToAdd * sizeof(void*));
}

bool SizeNotOk(size_t needed, size_t allocated) {
    ul_double capacity_fract = GetFraction((ul_double) needed,
                                           (ul_double) allocated);
    
    return capacity_fract < kArraySizeFraction || capacity_fract > 1.0;
}

void** GetArrayHandle(size_t size, size_t typeSize) {
    size_t from = 0;
    size_t freeIndex;
    const size_t byteSize = size * typeSize;
    do {
        freeIndex = GetFreeIndex(&array_pool.array_markers, from);
        from = freeIndex + 1;
    } while (freeIndex < array_pool.num_arrays && // Break if outside of array confines
             array_pool.arrays[freeIndex] != NULL && // Break if no allocated arrays are free
             SizeNotOk(byteSize, array_pool.capacities[freeIndex])); // Break if match with an OK size is found

    TakeFreeSpot(array_pool.array_markers, freeIndex);

    if (freeIndex >= array_pool.num_arrays) {
        ResizeArrays(NUM_NEW_ARRAYS);
        array_pool.arrays[freeIndex] = calloc(size, typeSize);
        array_pool.capacities[freeIndex] = byteSize;
    } else if (array_pool.arrays[freeIndex] == NULL) {
        array_pool.arrays[freeIndex] = calloc(size, typeSize);
        array_pool.capacities[freeIndex] = byteSize;
    }
    
    array_pool.sizes[freeIndex] = byteSize;
    array_pool.type_sizes[freeIndex] = typeSize;

    assert(array_pool.arrays[freeIndex] != NULL);

    return array_pool.arrays + freeIndex;
}

void ResizeArray(void** handle, size_t elementsToAdd) {
    // TODO: Consider asserting that the handle pointer is within the array range.

    // The pointer pointed to by the void** handle holds an adress whose difference with regards
    // to the first element of the array it comes from, is the index or ID of that pointer, in
    // the array of pointers, i.e. array_pool.arrays.
    size_t arrayId = handle - array_pool.arrays;
    array_pool.capacities[arrayId] += elementsToAdd * array_pool.type_sizes[arrayId];
    array_pool.arrays[arrayId] = realloc(array_pool.arrays[arrayId], array_pool.capacities[arrayId]);
}

void ReturnArray(void** handle) {
    // The pointer pointed to by the void** handle holds an adress whose difference with regards
    // to the first element of the array it comes from, is the index or ID of that pointer, in
    // the array of pointers, i.e. array_pool.arrays.
    size_t arrayId = handle - array_pool.arrays;
    ReleaseBusySpot(array_pool.array_markers, arrayId);
}

const size_t GetArraySize(void** handle) {
    size_t arrayId = handle - array_pool.arrays;
    return array_pool.sizes[arrayId] / array_pool.type_sizes[arrayId];
}

void AppendElement(Append_Array* array, void* element) {
    size_t arrayId = array->handle - array_pool.arrays;
    size_t typeSize = array_pool.type_sizes[arrayId];
    memcpy((char*) array_pool.arrays[arrayId] + typeSize * array->counter++, (char*) element, typeSize);
}

void ReplaceWithBack(Append_Array* array, size_t elementId) {
    size_t arrayId = array->handle - array_pool.arrays;
    size_t typeSize = array_pool.type_sizes[arrayId];
    // If the the element to be replaced is the last, the array should simply be decremented in size.
    if (array->counter - 1 != elementId) {
        // Element to replace ...
        char* to = (char*) array_pool.arrays[arrayId] + typeSize * elementId;
        // ... with back.
        char* from = (char*) array_pool.arrays[arrayId] + typeSize * (array->counter - 1);
        memcpy(to, from, typeSize);
    }
    array->counter--;
}
