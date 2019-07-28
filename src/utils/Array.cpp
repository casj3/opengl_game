#include "Array.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

Array NewArray(uint32_t sizeof_element, uint32_t num_elements) {
  return Array{(uint8_t*)calloc(num_elements, sizeof_element), sizeof_element, 0, num_elements};
}

void Push(Array* array, uint8_t element[], uint32_t sizeof_element) {
  assert(array->sizeof_element == sizeof_element);

  memcpy(&array->elements[array->size], element, sizeof_element);

  array->size++;
}

void Pop(Array* array)
{
  assert(array->size > 0);
	array->size--;
}

void ResizeArray(Array* array, uint32_t num_elements) {
  array->elements = (uint8_t*)realloc(array->elements, array->sizeof_element * num_elements);

  array->max = num_elements;
}
