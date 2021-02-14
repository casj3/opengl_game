#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct ArrayMarkers {
    int32_t* free_spots;
    size_t size;
};

/// Adds an element to the array at the specified location.
inline void AddElement(void* array, void* element, size_t freeIndex, size_t typeSize) {
    memcpy((char*) array + (freeIndex * typeSize), element, typeSize);
}

/// Removes the element at the given index by replacing it with the last written element
/// in the array. The last element is therefore moved to the place of the removed element.
inline void ReplaceWithBack(void* array, size_t replaceIndex, size_t backIndex, size_t typeSize) {
    // Overwrite it with the last element, we don't want to iterate through empty slots.
    memcpy((char*) array + (replaceIndex * typeSize),(char*) array + (backIndex * typeSize), typeSize);
}

/// Sets a bit signifying an index in an array to 0, meaning that
/// the array spot is not busy.
///
/// @param arrayMarkers The collection of bits representing busy or free elements in an array.
/// @param releaseIndex The position of the bit to assign 0.
void ReleaseBusySpot(ArrayMarkers arrayMarkers, size_t releaseIndex);

/// Returns a new array of bits signifying busy or not busy
/// array indicies by being 1 or 0.
///
/// @param numElements The number of elements to create a collection of
///                    array markers for.
ArrayMarkers NewArrayMarkers(size_t numElements);

/// Finds the first not busy index in the array by finding the first 0 bit. Its position
/// signifies the first not busy position in the array it's corresponding against. When
/// returning, the 0 bit will be switched to 1.
///
/// @param arrayMarkersPtr Pointer to busy bits. Enables the function to resize the markers array if needed.
/// @param from The n-th bit, to use as a starting point for finding the next free bit.
///
/// @returns the first not busy index in the array or a larger index if no free index is found.
size_t GetFreeIndex(ArrayMarkers* arrayMarkersPtr, size_t from);

/// @returns converted array index to set bit at index position in the number.
int32_t GetBitIndex(size_t index);

/// Sets a bit signifying an index in an array to 1, meaning that
/// the array spot is taken.
///
/// @param arrayMarkers The collection of busy flags.
/// @param freeIndex The position of the bit to assign 1.
void TakeFreeSpot(ArrayMarkers arrayMarkers, size_t freeIndex);
