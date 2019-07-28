#pragma once

#include <stdint.h>

struct Array {
  uint8_t* elements;
  uint32_t sizeof_element;
  uint32_t size;
  uint32_t  max;
};

Array NewArray(uint32_t sizeof_element, uint32_t num_elements);
void Push(Array* array, uint8_t element[], uint32_t sizeof_element);
void Pop(Array* array);
void ResizeArray(Array* array, uint32_t num_elements);
