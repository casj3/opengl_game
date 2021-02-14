#include "Array.h"

#include <intrin.h>

#include "Print.h"

#define INT32_BIT_SIZE (sizeof(int32_t) * 8)

size_t GetArrayMarkersArraySize(size_t numElements) {
    size_t numFullRows = numElements / INT32_BIT_SIZE;
    // In case we need more or less bits than what is divisible by the bit size
    size_t partRow = (numElements % INT32_BIT_SIZE) > 0;
    return numFullRows + partRow;
}

ArrayMarkers NewArrayMarkers(size_t numElements) {
    size_t size = GetArrayMarkersArraySize(numElements);
    return { (int32_t*)calloc(size, sizeof(int32_t)), size };
}

/// Returns a resized array markers array if the current one is not large enough.
/// Handles new indeterminate values in case of realloc. Do not cast return value to anything.
///
/// @param arrayMarkers    The flag array of allocated and free spots in a corresponding, non-flag array.
/// @param numElements     The number of elements in the new int array.
///                        This value should always be larger than the current size.
ArrayMarkers ResizeArrayMarkers(ArrayMarkers arrayMarkers, size_t numElements) {
    int32_t* newMarkers = (int32_t*)realloc(arrayMarkers.free_spots, sizeof(int32_t) * numElements);

    // Overwrite the indeterminate value produced by realloc with 0, as it's meant to be.
    memset(newMarkers + arrayMarkers.size, 0, sizeof(int32_t) * (numElements - arrayMarkers.size));

    return { newMarkers, numElements};
}

size_t GetFreeIndex(ArrayMarkers* arrayMarkersPtr, size_t from) {
    ArrayMarkers arrayMarkers = *arrayMarkersPtr;

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

void TakeFreeSpot(ArrayMarkers arrayMarkers, size_t freeIndex) {
    // Mark the bit representing the element as busy
    arrayMarkers.free_spots[freeIndex/INT32_BIT_SIZE] |= GetBitIndex(freeIndex);
}

void ReleaseBusySpot(ArrayMarkers arrayMarkers, size_t releaseIndex) {
    // Mark the bit representing the element as not busy.
    arrayMarkers.free_spots[releaseIndex/INT32_BIT_SIZE] &= ~GetBitIndex(releaseIndex);
}

int GetBitIndex(size_t index) {
    return 1 << (index % INT32_BIT_SIZE);
}
