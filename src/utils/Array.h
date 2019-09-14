#pragma once

#include <stdint.h>

/// Creates a new array. Meant to be cast to the chosen type.
///
/// @param sizeOfElement The size in bytes of the element type.
/// @param numelElements The number of elements to allocate space for.
uint8_t* NewArray(uint32_t sizeOfElement, uint32_t numElements);

/// Adds an element to the array at the specified location.
///
/// @param array Pointer to array.
/// @param element The element to add.
/// @param sizeOfElement The size in bytes of the element type.
/// @param freeIndex The index in the array where the element will be added.
void Add(uint8_t* array, uint8_t element[], uint32_t sizeOfElement, uint32_t freeIndex);

/// Removes the element at the given index by replacing it with the last written element
/// in the array. The last element is therefore moved to the place of the removed element.
///
/// @param array Pointer to array.
/// @param sizeOfElement The size in bytes of the element type.
/// @param removeIndex The index of the element which will be replaced by the last written element.
/// @param backIndex The index of the element in the back of the array which will replace the
///                  removed element.
void Remove(uint8_t* array, uint32_t sizeOfElement, uint32_t removeIndex, uint32_t backIndex);

/// Returns a resized array. Meant to be cast to the chosen type.
///
/// @param array Pointer to array.
/// @param sizeOfElement The size in bytes of the element type.
/// @param numelElements The number of elements to allocate space for.
uint8_t* ResizeArray(uint8_t* array, uint32_t sizeOfElement, uint32_t numElements);

/// Returns a new array of bits signifying busy or not busy
/// array indicies by being 1 or 0.
///
/// @param numElements The number of elements to create a collection of
///                    busy markers for.
int32_t* NewBuysMarkers(uint32_t numElements);

/// Finds the first not busy index in the array by finding the first
/// 0 bit. Its position signifies the first not busy position in the
/// array it's corresponding against.
///
/// @return busyMarkers The collection of busy bits.
unsigned long GetFreeIndex(int32_t* busyMarkers);
