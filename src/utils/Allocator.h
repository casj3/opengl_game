#pragma once

#include <stdint.h>

/// Sets start values for all containers. Must be called before utilizing the
/// other allocator functions.
void InitAllocator();

/// Adds a float array to the array of float arrays.
///
/// @param size The size in elements of the new array.
///
/// @return the ID of the array, i.e. the index in the array of arrays.
uint32_t AddFloatArray(uint32_t size);

/// Adds a float3 array to the array of float3 arrays.
///
/// @param size The size in elements of the new array.
///
/// @return the ID of the array, i.e. the index in the array of arrays.
uint32_t AddFloat3Array(uint32_t size);

/// Adds a uint32 array to the array of uint32 arrays.
///
/// @param size The size in elements of the new array.
///
/// @return the ID of the array, i.e. the index in the array of arrays.
uint32_t AddUint32Array(uint32_t size);

/// Expands the capacity of the array of float arrays.
///
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
void ResizeFloatArrays(uint32_t elementsToAdd);

/// Expands the capacity of the array of float3 arrays.
///
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
void ResizeFloat3Arrays(uint32_t elementsToAdd);

/// Expands the capacity of the array of uint32 arrays.
///
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
void ResizeUint32Arrays(uint32_t elementsToAdd);

/// Adds a float element to a float array.
///
/// @param arrayId The ID of the array.
/// @param elementId The desired ID of the element in the array.
/// @param element The element value.
void AddFloat(uint32_t arrayId, uint32_t elementId, float element);

/// Adds a float3 element to a float3 array.
///
/// @param arrayId The ID of the array.
/// @param elementId The desired ID of the element in the array.
/// @param element The element value.
void AddFloat3(uint32_t arrayId, uint32_t elementId, float element[3]);

/// Adds a uint32 element to a uint32 array.
///
/// @param arrayId The ID of the array.
/// @param elementId The desired ID of the element in the array.
/// @param element The element value.
void AddUint32(uint32_t arrayId, uint32_t elementId, uint32_t element);

/// Expands the capacity of a float array.
///
/// @param arrayId The array to resize.
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
void ResizeFloatArray(uint32_t arrayId, uint32_t elementsToAdd);

/// Expands the capacity of the capacity of a float3 array.
///
/// @param arrayId The array to resize.
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
void ResizeFloat3Array(uint32_t arrayId, uint32_t elementsToAdd);

/// Expands the capacity of the capacity of a uint32 array.
///
/// @param arrayId The array to resize.
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
void ResizeUint32Array(uint32_t arrayId, uint32_t elementsToAdd);
