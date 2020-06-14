
#include <stdint.h>

namespace Array {

struct BusyMarkers {
    int32_t* rows;
    uint32_t size;
};

/// Creates a new array.
template<typename T>
T* NewArray(uint32_t numElements) {
    return (T*)calloc(numElements, sizeof(T));
}

/// Adds an element to the array at the specified location.
template<typename T>
void AddElement(T* array, T element, uint32_t freeIndex) {
    memcpy(&array[freeIndex], element, sizeof(T));
}

/// Removes the element at the given index by replacing it with the last written element
/// in the array. The last element is therefore moved to the place of the removed element.
template<typename T>
void ReplaceWithBack(T* array, uint32_t replaceIndex, uint32_t backIndex) {
    // Overwrite it with the last element, we don't want to iterate through empty slots.
    memcpy(&array[replaceIndex], &array[backIndex], sizeof(T));
}

/// Sets a bit signifying an index in an array to 0, meaning that
/// the array spot is not busy.
///
/// @param busyMarkers The collection of busy flags.
/// @param releaseIndex The position of the bit to assign 0.
void ReleaseBusySpot(BusyMarkers busyMarkers, uint32_t releaseIndex);

/// Returns a resized array.
///
/// @param array Pointer to array.
/// @param sizeOfElement The size in bytes of the element type.
/// @param numElements The number of elements to allocate space for.
template<typename T>
T* ResizeArray(T* array, uint32_t numElements) {
    return (T*)realloc(array, sizeof(T) * numElements);
}

/// Returns a new array of bits signifying busy or not busy
/// array indicies by being 1 or 0.
///
/// @param numElements The number of elements to create a collection of
///                    busy markers for.
BusyMarkers NewBusyMarkers(uint32_t numElements);

/// Finds the first not busy index in the array by finding the first 0 bit. Its position
/// signifies the first not busy position in the array it's corresponding against. When
/// returning, the 0 bit will be switched to 1.
///
/// @param busyMarkersPtr Pointer to busy bits. Enables the function to resize the busy markers array if needed.
///
/// @returns the first not busy index in the array or a larger index if no free index is found.
unsigned long GetFreeIndex(BusyMarkers* busyMarkersPtr);
}
