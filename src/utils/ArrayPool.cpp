
#include "ArrayPool.h"

#include <assert.h>
#include <stdbool.h>

#include "Animation.h"
#include "LoadVAO.h"
#include "Transform.h"
#include "VAO_Data.h"
#include "MathFunctions.h"

#define ARRAY_SIZE_FRACTION 0.5f

#define ALLOC_TOGETHER 0

struct Array_Pool {
    void** arrays;
    // Sizes of the arrays, in bytes
    size_t* sizes;
    // The number of allocated slots may be larger than the amount of used elements for iterating.
    size_t* capacities;
};

static ArrayMarkers array_markers;
static size_t num_arrays;

static Array_Pool* array_pools;

void InitArrayPools(const size_t numArrayPools) {
    array_markers = NewArrayMarkers(numArrayPools * POOL_CAPACITY);
    num_arrays = numArrayPools * POOL_CAPACITY;
    array_pools = (Array_Pool*) calloc(numArrayPools, sizeof(Array_Pool));

#if ALLOC_TOGETHER
    const size_t arraysSize = sizeof(void*) * POOL_CAPACITY;
    const size_t sizesSize = sizeof(size_t) * POOL_CAPACITY;
    const size_t capacitiesSize = sizeof(size_t) * POOL_CAPACITY;
    const size_t arrayPoolBuffersSize = arraysSize + sizesSize + capacitiesSize;
#endif

    for (size_t i = 0; i < numArrayPools; i++) {
#if ALLOC_TOGETHER
        char* arrayPoolBuffers = (char*) calloc(arrayPoolBuffersSize, sizeof(char));
        array_pools[i].arrays = (void**) arrayPoolBuffers;
        array_pools[i].sizes = (size_t*) (arrayPoolBuffers + arraysSize);
        array_pools[i].capacities = (size_t*) (arrayPoolBuffers + (arraysSize + capacitiesSize));
#else
        array_pools[i].arrays = (void**) calloc(POOL_CAPACITY, sizeof(void**));
        array_pools[i].sizes = (size_t*) calloc(POOL_CAPACITY, sizeof(size_t));
        array_pools[i].capacities = (size_t*) calloc(POOL_CAPACITY, sizeof(size_t));
#endif
    }
}

/// Adds another array pool.
void AddArrayPool() {
    num_arrays += POOL_CAPACITY;

    array_pools = (Array_Pool*) realloc(array_pools, (num_arrays / POOL_CAPACITY) * sizeof(Array_Pool));

    const size_t poolIndex = (num_arrays / POOL_CAPACITY) - 1;

#if ALLOC_TOGETHER
    const size_t arraysSize = sizeof(void*) * POOL_CAPACITY;
    const size_t sizesSize = sizeof(size_t) * POOL_CAPACITY;
    const size_t capacitiesSize = sizeof(size_t) * POOL_CAPACITY;
    const size_t arrayPoolBuffersSize = arraysSize + sizesSize + capacitiesSize;

    char* arrayPoolBuffers = (char*) calloc(arrayPoolBuffersSize, sizeof(char));
    array_pools[poolIndex].arrays = (void**) arrayPoolBuffers;
    array_pools[poolIndex].sizes = (size_t*) (arrayPoolBuffers + arraysSize);
    array_pools[poolIndex].capacities = (size_t*) (arrayPoolBuffers + (arraysSize + capacitiesSize));
#else
    array_pools[poolIndex].arrays = (void**) calloc(POOL_CAPACITY, sizeof(void**));
    array_pools[poolIndex].sizes = (size_t*) calloc(POOL_CAPACITY, sizeof(size_t));
    array_pools[poolIndex].capacities = (size_t*) calloc(POOL_CAPACITY, sizeof(size_t));
#endif
}

bool SizeNotOk(size_t needed, size_t allocated) {
    ul_double capacity_fract = GetFraction((ul_double) needed,
                                           (ul_double) allocated);

    return capacity_fract < ARRAY_SIZE_FRACTION || capacity_fract > 1.0;
}

void** GetArrayHandle(size_t size, size_t typeSize) {
    const size_t byteSize = size * typeSize;

    size_t from = 0;
    size_t freeIndex;
    size_t poolIndex;
    size_t localIndex;
    do {
        freeIndex = GetFreeIndex(&array_markers, from);
        from = freeIndex + 1;

        poolIndex = freeIndex / POOL_CAPACITY;
        localIndex = freeIndex % POOL_CAPACITY;
    } while (freeIndex < num_arrays && // Break if outside of array confines
             array_pools[poolIndex].arrays[localIndex] != NULL && // Break if no allocated arrays are free
             SizeNotOk(byteSize, array_pools[poolIndex].capacities[localIndex])); // Break if match with an OK size is found

    TakeFreeSpot(array_markers, freeIndex);

    if (freeIndex >= num_arrays) {
        AddArrayPool();
        array_pools[poolIndex].arrays[localIndex] = calloc(size, typeSize);
        array_pools[poolIndex].capacities[localIndex] = byteSize;
    } else if (array_pools[poolIndex].arrays[localIndex] == NULL) {
        array_pools[poolIndex].arrays[localIndex] = calloc(size, typeSize);
        array_pools[poolIndex].capacities[localIndex] = byteSize;
    }

    array_pools[poolIndex].sizes[localIndex] = byteSize;

    assert(array_pools[poolIndex].arrays[localIndex] != NULL);

    return array_pools[poolIndex].arrays + localIndex;
}

const size_t GetPoolIndex(void** handle) {
    size_t poolIndex = 0;

    bool indexFound = false;
    const size_t numArrayPools = num_arrays / POOL_CAPACITY;

    for (size_t i = 0; i < numArrayPools && !indexFound; i++) {
        indexFound = array_pools[i].arrays <= handle && array_pools[i].arrays + POOL_CAPACITY > handle;
        poolIndex = indexFound ? i : numArrayPools;
    }

    assert(poolIndex < numArrayPools);
    return poolIndex;
}

void ResizeArray(void** handle, size_t elementsToAdd, size_t typeSize) {
    // The pointer pointed to by the void** handle holds an adress whose difference with regards
    // to the first element of the array it comes from, is the index or ID of that pointer, in
    // the array of pointers, i.e. array_pools[].arrays.
    size_t poolIndex = GetPoolIndex(handle);
    size_t arrayId = handle - array_pools[poolIndex].arrays;
    array_pools[poolIndex].capacities[arrayId] += elementsToAdd * typeSize;
    array_pools[poolIndex].arrays[arrayId] = realloc(array_pools[poolIndex].arrays[arrayId],
                                                     array_pools[poolIndex].capacities[arrayId]);
}

void ReturnArray(void** handle) {
    // The pointer pointed to by the void** handle holds an adress whose difference with regards
    // to the first element of the array it comes from, is the index or ID of that pointer, in
    // the array of pointers, i.e. array_pools[].arrays.
    size_t poolIndex = GetPoolIndex(handle);
    size_t arrayId = handle - array_pools[poolIndex].arrays;
    ReleaseBusySpot(array_markers, arrayId);
}

const size_t GetArraySize(void** handle, size_t typeSize) {
    size_t poolIndex = GetPoolIndex(handle);
    size_t arrayId = handle - array_pools[poolIndex].arrays;
    return array_pools[poolIndex].sizes[arrayId] / typeSize;
}

void AppendElement(Append_Array* array, void* element, size_t typeSize) {
    size_t poolIndex = GetPoolIndex(array->handle);
    size_t arrayId = array->handle - array_pools[poolIndex].arrays;
    memcpy((char*) array_pools[poolIndex].arrays[arrayId] + typeSize * array->counter++, (char*) element, typeSize);
}

void ReplaceWithBack(Append_Array* array, size_t elementId, size_t typeSize) {
    size_t poolIndex = GetPoolIndex(array->handle);
    size_t arrayId = array->handle - array_pools[poolIndex].arrays;
    // If the the element to be replaced is the last, the array should simply be decremented in size.
    if (array->counter - 1 != elementId) {
        // Element to replace ...
        char* to = (char*) array_pools[poolIndex].arrays[arrayId] + typeSize * elementId;
        // ... with back.
        char* from = (char*) array_pools[poolIndex].arrays[arrayId] + typeSize * (array->counter - 1);
        memcpy(to, from, typeSize);
    }
    array->counter--;
}
