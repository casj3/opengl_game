#pragma once

#include <stdint.h>

/// Sets start values for all containers. Must be called before utilizing the
/// other allocator functions.
void InitAllocator();

/// Adds a float array to the array of float arrays. If all array slots have been taken
/// the array of arrays will be reallocated with an addition of ten elements.
///
/// @param size The size in elements of the new array.
///
/// @return the ID of the array, i.e. the index in the array of arrays.
uint32_t AddFloatArray(uint32_t size);

/// Adds a float3 array to the array of float3 arrays. If all array slots have been taken
/// the array of arrays will be reallocated with an addition of ten elements.
///
/// @param size The size in elements of the new array.
///
/// @return the ID of the array, i.e. the index in the array of arrays.
uint32_t AddFloat3Array(uint32_t size);

/// Adds a uint32 array to the array of uint32 arrays. If all array slots have been taken
/// the array of arrays will be reallocated with an addition of ten elements.
///
/// @param size The size in elements of the new array.
///
/// @return the ID of the array, i.e. the index in the array of arrays.
uint32_t AddUint32Array(uint32_t size);

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
