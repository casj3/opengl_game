
#include "ArrayPool.h"

#include <assert.h>
#include <stdbool.h>

#include <intrin.h>
#include "Animation.h"
#include "LoadVAO.h"
#include "Transform.h"
#include "VAO_Data.h"
#include "MathFunctions.h"

#define INT32_BIT_SIZE (sizeof(int32_t) * 8)

#define ARRAY_SIZE_FRACTION 0.5f
#define ALLOC_TOGETHER 0

struct Array_Markers {
    int32_t* free_spots;
    size_t size;
};

struct Array_Pool {
    void** arrays;
    // Sizes of the arrays, in bytes
    size_t* sizes;
    // The number of allocated slots may be larger than the amount of used elements for iterating.
    size_t* capacities;
};

static Array_Markers array_markers;
static size_t num_arrays;
static Array_Pool* array_pools;

// Begin Array_Markers functions

void AddElement(void* array, void* element, size_t freeIndex, size_t typeSize) {
    memcpy((char*) array + (freeIndex * typeSize), element, typeSize);
}

void ReplaceWithBack(void* array, size_t replaceIndex, size_t backIndex, size_t typeSize) {
    // Overwrite it with the last element, we don't want to iterate through empty slots.
    memcpy((char*) array + (replaceIndex * typeSize),(char*) array + (backIndex * typeSize), typeSize);
}
size_t GetArrayMarkersArraySize(size_t numElements) {
    size_t numFullRows = numElements / INT32_BIT_SIZE;
    // In case we need more or less bits than what is divisible by the bit size
    size_t partRow = (numElements % INT32_BIT_SIZE) > 0;
    return numFullRows + partRow;
}

Array_Markers NewArrayMarkers(size_t numElements) {
    size_t size = GetArrayMarkersArraySize(numElements);
    return { (int32_t*)calloc(size, sizeof(int32_t)), size };
}

/// Returns a resized array markers array if the current one is not large enough.
/// Handles new indeterminate values in case of realloc. Do not cast return value to anything.
///
/// @param arrayMarkers    The flag array of allocated and free spots in a corresponding, non-flag array.
/// @param numElements     The number of elements in the new int array.
///                        This value should always be larger than the current size.
Array_Markers ResizeArrayMarkers(Array_Markers arrayMarkers, size_t numElements) {
    int32_t* newMarkers = (int32_t*)realloc(arrayMarkers.free_spots, sizeof(int32_t) * numElements);

    // Overwrite the indeterminate value produced by realloc with 0, as it's meant to be.
    memset(newMarkers + arrayMarkers.size, 0, sizeof(int32_t) * (numElements - arrayMarkers.size));

    return { newMarkers, numElements};
}

size_t GetFreeIndex(Array_Markers* arrayMarkersPtr, size_t from) {
    Array_Markers arrayMarkers = *arrayMarkersPtr;

    // Flip all bits
    int32_t ignoreBits = ~0;
    // Shift the bits such that the bits before the 'from' bit are set as taken.
    ignoreBits = ~(ignoreBits << (from % INT32_BIT_SIZE));
    // If flipping all bits is 0 then all spots are marked busy.
    size_t busySpotRow = from/INT32_BIT_SIZE;
    while (busySpotRow < arrayMarkers.size && ~(arrayMarkers.free_spots[busySpotRow] | ignoreBits) == 0) {
        // Reset the ignore bits, they're only relevant for one row.
        ignoreBits = 0;
        busySpotRow++;
    }

    // If there is no free index in the available rows, enlarge the markers array.
    if (busySpotRow >= arrayMarkers.size) {
        arrayMarkers = ResizeArrayMarkers(arrayMarkers, busySpotRow + 1);
        *arrayMarkersPtr = arrayMarkers;
    }

    // Include the free_spot bits.
    int32_t freeBit = arrayMarkers.free_spots[busySpotRow] | ignoreBits;
    // Get the rightmost unset bit to find the first available not busy spot
    freeBit = (freeBit + 1) & ~freeBit;
    // int32_t freeBit = (arrayMarkers.free_spots[busySpotRow] + 1) & ~arrayMarkers.free_spots[busySpotRow];

    unsigned long index;
    // 2019-08-04: Currently available for these platforms - x86, ARM, x64
    // If _BitScanReverse becomes outdated we could always use log2
    _BitScanReverse(&index, freeBit);

    return index + busySpotRow * INT32_BIT_SIZE;
}

int GetBitIndex(size_t index) {
    return 1 << (index % INT32_BIT_SIZE);
}

void TakeFreeSpot(Array_Markers arrayMarkers, size_t freeIndex) {
    // Mark the bit representing the element as busy
    arrayMarkers.free_spots[freeIndex/INT32_BIT_SIZE] |= GetBitIndex(freeIndex);
}

void ReleaseBusySpot(Array_Markers arrayMarkers, size_t releaseIndex) {
    // Mark the bit representing the element as not busy.
    arrayMarkers.free_spots[releaseIndex/INT32_BIT_SIZE] &= ~GetBitIndex(releaseIndex);
}

// End Array_Markers functions

// Begin Array_Pool functions

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

// End Array_Pool functions

// Begin Append_Array functions

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

// End Append_Array functions
