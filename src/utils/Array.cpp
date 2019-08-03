#include "Array.h"

#include <string.h>
#include <stdlib.h>
#include <intrin.h>

int32_t* NewBusySpotRows(uint32_t numElements) {
  int numFullRows = numElements / 32;
  // In case we need more or less bits than what is divisible by the bit size
  int partRow = (numElements % 32 > 0);
  return (int32_t*)calloc(numFullRows + partRow, sizeof(int32_t));
}

Array NewArray(uint32_t sizeOfElement, uint32_t numElements) {
  return Array{
    (uint8_t*)calloc(numElements, sizeOfElement),
    sizeOfElement,
    numElements,
    NewBusySpotRows(numElements),
  };
}

uint32_t Add(Array array, uint8_t element[]) {
  // Find the first busy spot row not holding only busy spots
  uint32_t busySpotRow = 0;

  // If busySpotRow hasn't incremented it means it has found an
  // indicator to use
  for (int i = 0; i == busySpotRow; i++) {
    // If flipping all the bits is 0 it means all spots are marked as busy
    busySpotRow += ~array.busy_spot_rows[i] == 0;
  }

  // Get the rightmost unset bit to find the first available not busy spot
  int32_t freeBit = (array.busy_spot_rows[busySpotRow] + 1) & ~array.busy_spot_rows[busySpotRow];
  unsigned long index;
  // 2019-08-04: Currently available for these platforms - x86, ARM, x64
  // If _BitScanReverse becomes outdated we could always use log2
  _BitScanReverse(&index, freeBit);

  memcpy(&array.elements[index * array.sizeof_element], element, array.sizeof_element);

  // Mark the bit representing the element as busy
  array.busy_spot_rows[busySpotRow] |= freeBit;

  return index;
}

void RemoveElement(Array array, uint32_t index)
{
  // Mark the bit representing the element as not busy
  array.busy_spot_rows[index / 32] &= ~(1 << (index % 32));
}

Array ResizeArray(Array array, uint32_t numElements) {
  array.elements = (uint8_t*)realloc(array.elements, array.sizeof_element * numElements);

  int numFullRows = numElements / 32;
  // In case we need more or less bits than what is divisible by the bit size
  int partRow = (numElements % 32) > 0;
  int numRows = numFullRows + partRow;
  array.busy_spot_rows = (int32_t*)realloc(array.busy_spot_rows, sizeof(int32_t) * numRows);

  array.max = numElements;

  return array;
}

void FreeArray(Array array) {
  free(array.elements);
  free(array.busy_spot_rows);
}
