
#include "Allocator.h"

#include "Array.h"
#include <glm/glm.hpp>

// Different columns which will only be known to the allocator.
// It is the owner and sole distributor of allocated memory.
// It keeps pointers to arrays of pointers to other arrays.

// TODO: This needs padding
struct ArraysMetaData {
  int32_t* busy_flags;
  uint32_t* array_sizes;
  uint32_t num_arrays;
};

#define AddArrayType(functionName, name, type)                          \
    type** name##_arrays;                                               \
    struct ArraysMetaData name##_arrays_meta_data;                      \
                                                                        \
    const uint32_t Add##functionName##Array(uint32_t size) {            \
        uint32_t freeIndex = GetFreeIndex(name##_arrays_meta_data.busy_flags); \
        type* name##_array = (type*)NewArray(sizeof(type), size);       \
        name##_arrays[freeIndex] = name##_array;                        \
        name##_arrays_meta_data.array_sizes[freeIndex] = size;          \
                                                                        \
        return freeIndex;                                               \
    }                                                                   \
                                                                        \
    const uint32_t Get##functionName##ArrayCapacity(uint32_t arrayId) { \
        return name##_arrays_meta_data.array_sizes[arrayId];            \
    }                                                                   \
                                                                        \
    void Resize##functionName##Arrays(uint32_t elementsToAdd) {         \
        name##_arrays_meta_data.num_arrays += elementsToAdd;            \
                                                                        \
        name##_arrays = (type**)ResizeArray((uint8_t*)name##_arrays, sizeof(type**), \
                                            name##_arrays_meta_data.num_arrays); \
                                                                        \
        name##_arrays_meta_data.array_sizes = (uint32_t*)ResizeArray((uint8_t*)name##_arrays_meta_data.array_sizes, \
                                                                     sizeof(uint32_t), \
                                                                     name##_arrays_meta_data.num_arrays); \
                                                                        \
        name##_arrays_meta_data.busy_flags = ResizeBusyMarkers(name##_arrays_meta_data.busy_flags, \
                                                               name##_arrays_meta_data.num_arrays); \
    }                                                                   \
                                                                        \
    void Add##functionName##(uint32_t arrayId, uint32_t elementId, type element) { \
        name##_arrays[arrayId][elementId] = element;                    \
    }                                                                   \
                                                                        \
    type Get##functionName##(uint32_t arrayId, uint32_t elementId) {    \
        return name##_arrays[arrayId][elementId];                      \
    }                                                                   \
                                                                        \
    void Resize##functionName##Array(uint32_t arrayId, uint32_t elementsToAdd) { \
        name##_arrays_meta_data.array_sizes[arrayId] += elementsToAdd;  \
        ResizeArray((uint8_t*)name##_arrays[arrayId], sizeof(type), name##_arrays_meta_data.array_sizes[arrayId]); \
    }                                                                   \
                                                                        \
    void Init##functionName##Arrays(uint32_t capacity) {                \
        name##_arrays = (type**)NewArray(sizeof(type**), capacity);     \
        name##_arrays_meta_data.busy_flags = NewBusyMarkers(capacity);  \
        name##_arrays_meta_data.array_sizes = (uint32_t*)NewArray(sizeof(uint32_t), capacity); \
        name##_arrays_meta_data.num_arrays = capacity;                  \
    }

AddArrayType(Float, float, float)
AddArrayType(Float3, float3, struct Float3)
AddArrayType(Uint32, uint32, uint32_t)

// Types from Animation.h
AddArrayType(SkeletonPositions, skeleton_positions, struct SkeletonPositions)
AddArrayType(SkeletonRotations, skeleton_rotations, struct SkeletonRotations)
AddArrayType(SkeletonScales, skeleton_scales, struct SkeletonScales)

// // float3 needs to be treated differently so the macro function does not apply for it.
// float** float3_arrays;
// struct ArraysMetaData float3_arrays_meta_data;

// const uint32_t AddFloat3Array(uint32_t size){
//   uint32_t freeIndex = GetFreeIndex(float3_arrays_meta_data.busy_flags);
//   float* float3_array = (float*)NewArray(sizeof(float[3]), size);
//   float3_arrays[freeIndex] = float3_array;
//   float3_arrays_meta_data.array_sizes[freeIndex] = size;

//   return freeIndex;
// }

// const uint32_t GetFloat3ArrayCapacity(uint32_t arrayId) {
//     return float3_arrays_meta_data.array_sizes[arrayId];
// }

// void ResizeFloat3Arrays(uint32_t elementsToAdd) {
//   float3_arrays_meta_data.num_arrays += elementsToAdd;

//   float3_arrays = (float**)ResizeArray((uint8_t*)float3_arrays, sizeof(float**),
//                    float3_arrays_meta_data.num_arrays);

//   float3_arrays_meta_data.array_sizes = (uint32_t*)ResizeArray((uint8_t*)float3_arrays_meta_data.array_sizes,
//                                                                sizeof(uint32_t),
//                                                                float3_arrays_meta_data.num_arrays);

//   float3_arrays_meta_data.busy_flags = ResizeBusyMarkers(float3_arrays_meta_data.busy_flags,
//                                                          float3_arrays_meta_data.num_arrays);
// }

// void AddFloat3(uint32_t arrayId, uint32_t elementId, float element[3]) {
//   float3_arrays[arrayId][elementId++] = element[0];
//   float3_arrays[arrayId][elementId++] = element[1];
//   float3_arrays[arrayId][elementId]   = element[2];
// }

// float GetFloat3(uint32_t arrayId, uint32_t elementId) {
    
// }

// void ResizeFloat3Array(uint32_t arrayId, uint32_t elementsToAdd) {
//   float3_arrays_meta_data.array_sizes[arrayId] += elementsToAdd;
//   ResizeArray((uint8_t*)float3_arrays[arrayId], sizeof(float[3]), float3_arrays_meta_data.array_sizes[arrayId]);
// }

// void InitFloat3Arrays(uint32_t capacity) {
//   float3_arrays = (float**)NewArray(sizeof(float**), capacity);
//   float3_arrays_meta_data.busy_flags = NewBusyMarkers(capacity);
//   float3_arrays_meta_data.array_sizes = (uint32_t*)NewArray(sizeof(uint32_t), capacity);
//   float3_arrays_meta_data.num_arrays = capacity;
// }

void InitAllocator(uint32_t capacity) {
  InitFloatArrays(capacity);
  InitFloat3Arrays(capacity);
  InitUint32Arrays(capacity);
}
