#pragma once

#include <stdint.h>

struct Array {
  uint8_t* elements;
  uint32_t sizeof_element;
  uint32_t max;
  int32_t* busy_spot_rows;
};

Array NewArray(uint32_t sizeOfElement, uint32_t numElements);

/// Returns the index of the element location in the array
/// Marks the element index as busy
uint32_t Add(Array array, uint8_t element[]);

/// Marks the element at index as not busy, meaning it is
/// allowed to be overwritten
void Remove(Array array, uint32_t index);

/// Returns a resized array
Array ResizeArray(Array array, uint32_t numElements);
void FreeArray(Array array);
