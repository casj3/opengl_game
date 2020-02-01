#pragma once

#include <stdint.h>

#include "Animation.h"

/// Sets start values for all containers. Must be called before utilizing the
/// other allocator functions.
void InitAllocator();

struct Float3 {
    float x;
    float y;
    float z;
};

/// Adds a float3 array to the array of float3 arrays.
///
/// @param size The size in elements of the new array.
///
/// @return the ID of the array, i.e. the index in the array of arrays.
const uint32_t AddFloat3Array(uint32_t size);

/// @return the current max capacity of a float3 array.
const uint32_t GetFloat3ArrayCapacity(uint32_t arrayId);

/// Expands the capacity of the array of float3 arrays.
///
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
void ResizeFloat3Arrays(uint32_t elementsToAdd);

/// Adds a float3 element to a float3 array.
///
/// @param arrayId The ID of the array.
/// @param elementId The desired ID of the element in the array.
/// @param element The element value.
void AddFloat3(uint32_t arrayId, uint32_t elementId, struct Float3 element);


/// @param arrayId The ID of the array.
/// @param elementId The desired ID of the element in the array.
///
/// @return a copy of the element.
struct Float3 GetFloat3(uint32_t arrayId, uint32_t elementId);

/// Expands the capacity of the capacity of a float3 array.
///
/// @param arrayId The array to resize.
/// @param elementsToAdd The number of slots to expand the capacity of the array with.
void ResizeFloat3Array(uint32_t arrayId, uint32_t elementsToAdd);

// The subsequent types mentioned below should function similarly to the ones above
// so I won't bother to document them. I'll just add a macro function.
#define AddArrayTypeDeclaration(functionName, type)                     \
    const uint32_t Add##functionName##Array(uint32_t size);             \
    const uint32_t Get##functionName##ArrayCapacity(uint32_t arrayId);  \
    void Resize##functionName##Arrays(uint32_t elementsToAdd);          \
    void Add##functionName##(uint32_t arrayId, uint32_t elementId, type element); \
    type Get##functionName##(uint32_t arrayId, uint32_t elementId);     \
    void Resize##functionName##Array(uint32_t arrayId, uint32_t elementsToAdd);

AddArrayTypeDeclaration(Float, float)
AddArrayTypeDeclaration(Uint32, uint32_t)

// Types from Animation.h
AddArrayTypeDeclaration(SkeletonPositions, SkeletonPositions)
AddArrayTypeDeclaration(SkeletonRotations, SkeletonRotations)
AddArrayTypeDeclaration(SkeletonScales, SkeletonScales)
