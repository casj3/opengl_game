#include "Array.h"

#include <string.h>
#include <stdlib.h>
#include <intrin.h>

int32_t* NewBusyMarkers(uint32_t numElements) {
  int numFullRows = numElements / 32;
  // In case we need more or less bits than what is divisible by the bit size
  int partRow = (numElements % 32 > 0);
  return (int32_t*)calloc(numFullRows + partRow, sizeof(int32_t));
}

uint8_t* NewArray(uint32_t sizeOfElement, uint32_t numElements) {
  return (uint8_t*)calloc(numElements, sizeOfElement);
}

unsigned long GetFreeIndex(int32_t* busyMarkers) {
  // Find the first busy spot row not holding only busy spots
  uint32_t busySpotRow = 0;

  // If busySpotRow hasn't incremented it means it has found an
  // indicator to use
  for (int i = 0; i == busySpotRow; i++) {
    // If flipping all the bits is 0 it means all spots are marked as busy
    busySpotRow += ~busyMarkers[i] == 0;
  }

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

void Add(uint8_t* array, uint8_t element[], uint32_t sizeOfElement, uint32_t freeIndex) {
  memcpy(&array[freeIndex * sizeOfElement], element, sizeOfElement);
}

void Remove(uint8_t* array, uint32_t sizeOfElement, uint32_t removeIndex, uint32_t backIndex) {
  // Overwrite it with the last element, we don't want to iterate through empty slots.
  memcpy(&array[removeIndex * sizeOfElement],
         &array[backIndex * sizeOfElement],
         sizeOfElement);
}

uint8_t* ResizeArray(uint8_t* array, uint32_t sizeOfElement, uint32_t numElements) {
  return (uint8_t*)realloc(array, sizeOfElement * numElements);
}
