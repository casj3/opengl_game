#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <glm/glm.hpp>

#include "Array.h"

#define NUM_NEW_ARRAYS 10
#define NUM_NEW_ELEMENTS 10

template<typename T>
struct ArrayHandle {
    uint32_t id;

    /// Retrieves a reference to the element with elementId.
    T& operator[](uint32_t elementId) {
        return arrays<T>.elements[id][elementId];
    }
};

template<typename T>
struct AppendArray {
    ArrayHandle<T> handle;
    uint32_t counter = 0;
};

template<typename T>
struct DestructorArray {
    DestructorArray(ArrayHandle<T> arrayHandle) {
        array_handle = arrayHandle;
    }

    ~DestructorArray() {
        RemoveArray(array_handle);
    }

    ArrayHandle<T> array_handle;
};

/// Contains information of all arrays of one kind, T. Only meant for use in this file.
template<typename T>
struct Arrays {
    T** elements;
    Array::BusyMarkers busy_markers;
    uint32_t* sizes;
    // The number of allocated slots may be larger than the amount of used elements for iterating.
    uint32_t* capacities;
    uint32_t num_arrays;
};

/// All arrays. Do not use. For utilizing the arrays, use the ArrayHandle and the functions
/// which process it.
template<typename T>
extern Arrays<T> arrays;

/// Adds a array to the collection of arrays.
///
/// @param size The size in elements of the new array.
///
/// @return the handle of the array, i.e. the index in the array of arrays.
template<typename T>
const ArrayHandle<T> AddArray(uint32_t size) {
    uint32_t freeIndex = Array::GetFreeIndex(&arrays<T>.busy_markers);
    const ArrayHandle<T> handle = { freeIndex };
    const uint32_t num_arrays = arrays<T>.num_arrays;

    // TODO: Add function for finding non-busy arrays without too big size discrepancies.

    if (freeIndex >= num_arrays) {
        ResizeArrays<T>(NUM_NEW_ARRAYS);
        arrays<T>.elements[handle.id] = Array::NewArray<T>(size);
        arrays<T>.capacities[handle.id] = size;
    } else if (arrays<T>.elements[handle.id] == NULL) {
        arrays<T>.elements[handle.id] = Array::NewArray<T>(size);
        arrays<T>.capacities[handle.id] = size;
    } else if (arrays<T>.capacities[handle.id] < size) {
        ResizeArray<T>(handle, size - arrays<T>.capacities[handle.id]);
    }

    arrays<T>.sizes[handle.id] = size;

    return handle;
}

/// Removes array from the collection of arrays.
///
/// @param handle The handle to the array.
template<typename T>
void RemoveArray(const ArrayHandle<T> handle) {
    Array::ReleaseBusySpot(arrays<T>.busy_markers, handle.id);
}

/// Expands the capacity of an array.
///
/// @param handle The handle to the array.
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
template<typename T>
void ResizeArray(const ArrayHandle<T> handle, uint32_t elementsToAdd) {
    arrays<T>.capacities[handle.id] += elementsToAdd;
    arrays<T>.elements[handle.id] = Array::ResizeArray<T>(arrays<T>.elements[handle.id], arrays<T>.capacities[handle.id]);
}

/// Expands the capacity of the array of arrays.
///
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
template<typename T>
void ResizeArrays(uint32_t elementsToAdd) {
    uint32_t firstNewElement = arrays<T>.num_arrays;
    arrays<T>.num_arrays += elementsToAdd;

    arrays<T>.elements = Array::ResizeArray<T*>(arrays<T>.elements, arrays<T>.num_arrays);
    arrays<T>.sizes = Array::ResizeArray<uint32_t>(arrays<T>.sizes, arrays<T>.num_arrays);
    arrays<T>.capacities = Array::ResizeArray<uint32_t>(arrays<T>.capacities, arrays<T>.num_arrays);

    // Overwrite the new elements, which hold indeterminate values, with NULL
    // to keep track of which pointers have been assigned arrays.
    memset(arrays<T>.elements + firstNewElement, NULL, elementsToAdd * sizeof(T*));
}

template<typename T>
const uint32_t GetArraySize(const ArrayHandle<T> handle) {
    return arrays<T>.sizes[handle.id];
}

/// @param handle The handle to the array.
/// @param elementId The desired ID of the element in the array.
///
/// @return a copy of the element.
template<typename T>
T GetElement(const ArrayHandle<T> handle, uint32_t elementId) {
    return arrays<T>.elements[handle.id][elementId];
}

/// Set value of an element in the array.
///
/// @param handle The handle to the array.
/// @param elementId The desired ID of the element in the array.
/// @param element The value to assign the element.
template<typename T>
void SetElement(const ArrayHandle<T> handle, uint32_t elementId, T element) {
    arrays<T>.elements[handle.id][elementId] = element;
}

/// Appends a value to an array.
///
/// @param array The AppendArray.
/// @param element The value to assign the element.
///
/// @returns the updated AppendArray.
template<typename T>
AppendArray<T> AppendElement(AppendArray<T> array, T element) {
    arrays<T>.elements[array.handle.id][array.counter++] = element;
    return array;
}

/// @returns the id of the last appended element in the AppendArray. A negative value is returned if nothing has been appended.
template<typename T>
int GetBack(AppendArray<T> array) {
    return array.counter - 1;
}

/// Replaces the specified element with the back element and decrements the appended elements count of the array.
///
/// @param array The AppendArray.
/// @param elementId The id of the element to replace with the back element.
///
/// @returns the updated AppendArray.
template<typename T>
void ReplaceWithBack(AppendArray<T> array, uint32_t elementId) {
    // If the the element to be replaced is the last, the array should simply be decremented in size.
    if (array.counter - 1 != elementId) {
        arrays<T>.elements[array.handle.id][elementId] = arrays<T>.elements[handle.id][array.counter - 1];
    }
    array.counter--;
}

template<typename T>
void InitArrays(uint32_t capacity) {
    arrays<T>.elements = Array::NewArray<T*>(capacity);
    arrays<T>.sizes = Array::NewArray<uint32_t>(capacity);
    arrays<T>.capacities = Array::NewArray<uint32_t>(capacity);
    arrays<T>.busy_markers = Array::NewBusyMarkers(capacity);
    arrays<T>.num_arrays = capacity;
}

/// Initializes the allocator.
void InitArrayManager(uint32_t capacity);
