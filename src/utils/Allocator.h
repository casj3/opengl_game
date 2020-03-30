#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <glm/glm.hpp>

#include "Array.h"

namespace Allocator {
// Different columns which will only be known to the allocator.
// It is the owner and sole distributor of allocated memory.
// It keeps pointers to arrays of pointers to other arrays.

// TODO: This needs padding
template<typename T>
struct Arrays {
    T** elements;
    int32_t* busy_flags;
    uint32_t* sizes;
    // The number of allocated slots may be larger than the amount of used elements for iterating.
    uint32_t* capacities;
    uint32_t num_arrays;
};

template<typename T>
Arrays<T> arrays;

template<typename T>
struct ArrayHandle {
    uint32_t id;
};

/// Adds a array to the collection of arrays.
///
/// @param size The size in elements of the new array.
///
/// @return the handle of the array, i.e. the index in the array of arrays.
template<typename T>
const ArrayHandle<T> AddArray(uint32_t size) {
    uint32_t freeIndex = Array::GetFreeIndex(arrays<T>.busy_flags, arrays<T>.num_arrays);
    const ArrayHandle<T> handle = { freeIndex };
    
    if (freeIndex < arrays<T>.num_arrays &&
        arrays<T>.elements[handle.id] != NULL &&
        size <= arrays<T>.capacities[handle.id]) {
    } else if (freeIndex >= arrays<T>.num_arrays) {
        freeIndex = arrays<T>.num_arrays;
        ResizeArrays<T>(10);
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
    Array::ReleaseBusySpot(arrays.busy_flags, handle.id);
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
    arrays<T>.num_arrays += elementsToAdd;

    arrays<T>.elements = Array::ResizeArray<T*>(arrays<T>.elements, arrays<T>.num_arrays);
    arrays<T>.sizes = Array::ResizeArray<uint32_t>(arrays<T>.sizes, arrays<T>.num_arrays);
    arrays<T>.capacities = Array::ResizeArray<uint32_t>(arrays<T>.capacities, arrays<T>.num_arrays);
    arrays<T>.busy_flags = Array::ResizeBusyMarkers(arrays<T>.busy_flags, arrays<T>.num_arrays);
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
    arrays<T>.elements[handle.id][elementId] = T;
}

template<typename T>
void InitArrays(uint32_t capacity) {
    arrays<T>.elements = Array::NewArray<T*>(capacity);
    arrays<T>.sizes = Array::NewArray<uint32_t>(capacity);
    arrays<T>.capacities = Array::NewArray<uint32_t>(capacity);
    arrays<T>.busy_flags = Array::NewBusyMarkers(capacity);
    arrays<T>.num_arrays = capacity;
}    

/// Initializes the allocator.
void InitAllocator();
}
