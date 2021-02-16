#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <glm/glm.hpp>

#include "Array.h"

#ifndef POOL_CAPACITY
#define POOL_CAPACITY 50
#endif

/// A pointer to a pointer to an element of the designated type.
/// Can be used for casting to and from array handles retrieved by
/// GetArrayHandle() or GET_ARRAY_HANDLE().
///
/// @param The designated type.
#define ARRAY_HANDLE(type) type**

/// Retrieves a handle to an array of the designated type and casts it to the desired type.
///
/// @param type The type of the array handle, which is a pointer to a pointer to the array in memory.
/// @param size The desired number of elements for the array.
///
/// @returns an array handle.
#define GET_ARRAY_HANDLE(type, size)                    \
    (type **) GetArrayHandle(size, sizeof(type))

/// Returns an array to the collection of arrays that are marked as non-taken,
/// ready to be used again by another client.
///
/// @param handle An array handle. Only values that have been retrieved via GET_ARRAY_HANDLE()
///               or GetArrayHandle() may be returned.
#define RETURN_ARRAY(handle) \
    ReturnArray((void**) handle)

/// @param handle The handle to the array.
///
/// @returns the size of the array.
#define GET_ARRAY_SIZE(handle) \
    GetArraySize((void**) handle, sizeof(**handle))

/// Couples an array handle with a counter.
struct Append_Array {
    void** handle;
    size_t counter = 0;
};

/// Template version of Append_Array. Is aligned like Append_Array and can be
/// cast to it therefore.
template <typename T>
struct alignas(Append_Array) T_Append_Array {
    T** handle;
    size_t counter = 0;
};

/// Initializes the array pools. Must be called before any of the other declared functions in the header.
///
/// @param numArrayPools The number of array pools to allocate space for.
void InitArrayPools(const size_t numArrayPools);

/// Get an array from the array pool.
///
/// @param size The size in elements of the new array.
/// @param typeSize The size in bytes of the elements type.
///
/// @returns the handle to the array, i.e. the index in the array pool.
void** GetArrayHandle(size_t size, size_t typeSize);

/// Returns an array to the collection of arrays that are marked as non-taken,
/// ready to be used again by another client.
///
/// @param handle The handle to the array.
void ReturnArray(void** handle);

/// Expands the capacity of an array.
///
/// @param handle The handle to the array.
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
/// @param typeSize The size in bytes of the elements type.
void ResizeArray(void** handle, size_t elementsToAdd, size_t typeSize);

/// @param handle The handle to the array.
///
/// @param typeSize The size in bytes of the elements type.
///
/// @returns the size of the array.
const size_t GetArraySize(void** handle, size_t typeSize);

/// Get an appending array of a template type.
///
/// @size The elements amount of the array.
///
/// @returns an appending array.
template <typename T>
T_Append_Array<T> TGetAppendArray(size_t size) {
    return { (T**) GetArrayHandle(size, sizeof(T)), 0 };
}

/// Appends a value to an appending array.
///
/// @param array The appending array.
/// @param element The value to assign the element.
/// @param typeSize The size in bytes of the elements type.
void AppendElement(Append_Array* array, void* element, size_t typeSize);

/// Appends a value to an appending array of a template type.
///
/// @param[] array The appending array.
/// @param element The value to assign the element.
template <typename T>
void TAppendElement(T_Append_Array<T>* array, T element) {
    AppendElement((Append_Array*) array, (void*) &element, sizeof(T));
}

/// Replaces the specified element with the back element and decrements the appended elements count of the array.
///
/// @param array The Append_Array.
/// @param elementId The id of the element to replace with the back element.
/// @param typeSize The size in bytes of the elements type.
void ReplaceWithBack(Append_Array* array, size_t elementId, size_t typeSize);

/// @param array The Append_Array.
/// @returns the id of the last appended element in the Append_Array. A negative value is returned if nothing has been appended.
inline size_t GetBack(Append_Array array) {
    return array.counter - 1;
}

template <typename T>
inline size_t TGetBack(T_Append_Array<T> array) {
    return array.counter - 1;
}
