#include "Array.h"

#include <string.h>
#include <stdlib.h>
#include <intrin.h>

#define INT32_BIT_SIZE 32

namespace Array {

uint32_t GetBusyMarkersArraySize(int32_t numElements) {
    int numFullRows = numElements / INT32_BIT_SIZE;
    // In case we need more or less bits than what is divisible by the bit size
    int partRow = (numElements % INT32_BIT_SIZE) > 0;
    return numFullRows + partRow;
}

BusyMarkers NewBusyMarkers(uint32_t numElements) {
    uint32_t size = GetBusyMarkersArraySize(numElements);
    return { (int32_t*)calloc(size, sizeof(int32_t)), size };
}

/// Returns a resized busy markers array if the current one is not large enough.
/// Handles new indeterminate values in case of realloc. Do not cast return value to anything.
///
/// @param busyMarkers    The array of busy flags.
/// @param numElements    The number of elements in the new int32_t array.
///                       This value should always be larger than the current size.
BusyMarkers ResizeBusyMarkers(BusyMarkers busyMarkers, uint32_t numElements) {
    int32_t* newBusyMarkers = (int32_t*)realloc(busyMarkers.rows, sizeof(int32_t) * numElements);

    // Overwrite the indeterminate value produced by realloc with 0, as it's meant to be.
    memset(newBusyMarkers + busyMarkers.size, 0, sizeof(int32_t) * (numElements - busyMarkers.size));

    return { newBusyMarkers, numElements};
}

unsigned long GetFreeIndex(BusyMarkers* busyMarkersPtr) {
    BusyMarkers busyMarkers = *busyMarkersPtr;

    // Find the first busy spot row not holding only busy spots
    uint32_t busySpotRow = 0;

    // If flipping all bits is 0 then all spots are marked busy.
    uint32_t i = 0;
    while (i < busyMarkers.size && ~busyMarkers.rows[i] == 0) {
        i++;
    }

    // If there is no free index in the available rows, return the size of the array
    // to let the caller know that the corresponding array needs to be enlarged.
    if (i == busyMarkers.size) {
        busyMarkers = ResizeBusyMarkers(busyMarkers, busyMarkers.size + 1);
        *busyMarkersPtr = busyMarkers;
    }

    busySpotRow = i;

    // Get the rightmost unset bit to find the first available not busy spot
    int32_t freeBit = (busyMarkers.rows[busySpotRow] + 1) & ~busyMarkers.rows[busySpotRow];
    unsigned long index;
    // 2019-08-04: Currently available for these platforms - x86, ARM, x64
    // If _BitScanReverse becomes outdated we could always use log2
    _BitScanReverse(&index, freeBit);

    // Mark the bit representing the element as busy
    busyMarkers.rows[busySpotRow] |= freeBit;

    return index + busySpotRow * INT32_BIT_SIZE;
}

void ReleaseBusySpot(BusyMarkers busyMarkers, uint32_t releaseIndex) {
    // Mark the bit representing the element as not busy.
    busyMarkers.rows[releaseIndex/INT32_BIT_SIZE] &= ~(1 << (releaseIndex % INT32_BIT_SIZE));
}

}
