#include "Array.h"

#include <string.h>
#include <stdlib.h>
#include <intrin.h>

#define INT32_BIT_SIZE 32

namespace Array {
int32_t* NewBusyMarkers(uint32_t numElements) {
    int numFullRows = numElements / INT32_BIT_SIZE;
    // In case we need more or less bits than what is divisible by the bit size
    int partRow = (numElements % INT32_BIT_SIZE) > 0;
    return (int32_t*)calloc(numFullRows + partRow, sizeof(int32_t));
}

uint32_t GetBusyMarkersArraySize(int32_t numElements) {
    int numFullRows = numElements / INT32_BIT_SIZE;
    // In case we need more or less bits than what is divisible by the bit size
    int partRow = (numElements % INT32_BIT_SIZE) > 0;
    return numFullRows + partRow;
}

unsigned long GetFreeIndex(int32_t* busyMarkers, uint32_t numElements) {
    // Find the first busy spot row not holding only busy spots
    uint32_t busySpotRow = 0;

    // If flipping all bits is 0 then all spots are marked busy. If n
    int32_t i = 0;
    int32_t busyMarkersArraySize = GetBusyMarkersArraySize(numElements);
    while (i < busyMarkersArraySize && ~busyMarkers[i] == 0) {
        i++;
    }

    // If there is no free index in the available rows, return the size of the array
    // to let the caller know that the corresponding array needs to be enlarged.
    if (i == busyMarkersArraySize) {
        return numElements;
    }
  
    busySpotRow = i;

    // Get the rightmost unset bit to find the first available not busy spot
    int32_t freeBit = (busyMarkers[busySpotRow] + 1) & ~busyMarkers[busySpotRow];
    unsigned long index;
    // 2019-08-04: Currently available for these platforms - x86, ARM, x64
    // If _BitScanReverse becomes outdated we could always use log2
    _BitScanReverse(&index, freeBit);

    // Mark the bit representing the element as busy
    busyMarkers[busySpotRow] |= freeBit;

    return index;
}

void ReleaseBusySpot(int32_t* busyMarkers, uint32_t releaseIndex) {
    // Mark the bit representing the element as not busy.
    busyMarkers[releaseIndex/INT32_BIT_SIZE] &= ~(1 << (releaseIndex % INT32_BIT_SIZE));
}

int32_t* ResizeBusyMarkers(int32_t* busyMarkers, uint32_t numElements) {
    int numFullRows = numElements / INT32_BIT_SIZE;
    // In case we need more or less bits than what is divisible by the bit size
    int partRow = (numElements % INT32_BIT_SIZE) > 0;
    return (int32_t*)realloc(busyMarkers, sizeof(int32_t) * (numFullRows + partRow));
}
}
