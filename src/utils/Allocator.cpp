
#include "Allocator.h"

#include "Array.h"

// Different columns which will only be known to the allocator.
// It is the owner and sole distributor of allocated memory.
// It keeps pointers to arrays of pointers to other arrays.

// TODO: This needs padding
struct ArraysMetaData {
  int32_t* busy_flags;
  uint32_t* array_sizes;
  uint32_t num_arrays;
};

float** float_arrays;
ArraysMetaData float_arrays_meta_data;

float** float3_arrays;
ArraysMetaData float3_arrays_meta_data;

uint32_t** uint32_arrays;
ArraysMetaData uint32_arrays_meta_data;

void InitAllocator(uint32_t capacity) {
  float_arrays = (float**)NewArray(sizeof(float**), capacity);
  float_arrays_meta_data.busy_flags = NewBusyMarkers(capacity);
  float_arrays_meta_data.array_sizes = (uint32_t*)NewArray(sizeof(uint32_t), capacity);
  float_arrays_meta_data.num_arrays = capacity;

  float3_arrays = (float**)NewArray(sizeof(float**), capacity);
  float3_arrays_meta_data.busy_flags = NewBusyMarkers(capacity);
  float3_arrays_meta_data.array_sizes = (uint32_t*)NewArray(sizeof(uint32_t), capacity);
  float3_arrays_meta_data.num_arrays = capacity;

  uint32_arrays = (uint32_t**)NewArray(sizeof(uint32_t**), capacity);
  uint32_arrays_meta_data.busy_flags = NewBusyMarkers(capacity);
  uint32_arrays_meta_data.array_sizes = (uint32_t*)NewArray(sizeof(uint32_t), capacity);
  uint32_arrays_meta_data.num_arrays = capacity;
}

uint32_t AddFloatArray(uint32_t size) {
  uint32_t freeIndex = GetFreeIndex(float_arrays_meta_data.busy_flags);
  float* float_array = (float*)NewArray(sizeof(float), size);
  float_arrays[freeIndex] = float_array;
  float_arrays_meta_data.array_sizes[freeIndex] = size;

  return freeIndex;
}

uint32_t AddFloat3Array(uint32_t size){
  uint32_t freeIndex = GetFreeIndex(float3_arrays_meta_data.busy_flags);
  float* float3_array = (float*)NewArray(sizeof(float[3]), size);
  float3_arrays[freeIndex] = float3_array;
  float3_arrays_meta_data.array_sizes[freeIndex] = size;

  return freeIndex;
}

uint32_t AddUint32Array(uint32_t size) {
  uint32_t freeIndex = GetFreeIndex(uint32_arrays_meta_data.busy_flags);
  uint32_t* uint32_array = (uint32_t*)NewArray(sizeof(uint32_t), size);
  uint32_arrays[freeIndex] = uint32_array;
  uint32_arrays_meta_data.array_sizes[freeIndex] = size;

  return freeIndex;
}

void ResizeFloatArrays(uint32_t elementsToAdd) {
  float_arrays_meta_data.num_arrays += elementsToAdd;

  float_arrays = (float**)ResizeArray((uint8_t*)float_arrays, sizeof(float**),
                  float_arrays_meta_data.num_arrays);

  float_arrays_meta_data.array_sizes = (uint32_t*)ResizeArray((uint8_t*)float_arrays_meta_data.array_sizes,
                                                              sizeof(uint32_t),
                                                              float_arrays_meta_data.num_arrays);

  float_arrays_meta_data.busy_flags = ResizeBusyMarkers(float_arrays_meta_data.busy_flags,
                                                        float_arrays_meta_data.num_arrays);
}

void ResizeFloat3Arrays(uint32_t elementsToAdd) {
  float3_arrays_meta_data.num_arrays += elementsToAdd;

  float3_arrays = (float**)ResizeArray((uint8_t*)float3_arrays, sizeof(float**),
                   float3_arrays_meta_data.num_arrays);

  float3_arrays_meta_data.array_sizes = (uint32_t*)ResizeArray((uint8_t*)float3_arrays_meta_data.array_sizes,
                                                               sizeof(uint32_t),
                                                               float3_arrays_meta_data.num_arrays);

  float3_arrays_meta_data.busy_flags = ResizeBusyMarkers(float3_arrays_meta_data.busy_flags,
                                                         float3_arrays_meta_data.num_arrays);
}

void ResizeUint32Arrays(uint32_t elementsToAdd) {
  uint32_arrays_meta_data.num_arrays += elementsToAdd;

  uint32_arrays = (uint32_t**)ResizeArray((uint8_t*)uint32_arrays, sizeof(uint32_t**),
                                          uint32_arrays_meta_data.num_arrays);

  uint32_arrays_meta_data.array_sizes = (uint32_t*)ResizeArray((uint8_t*)uint32_arrays_meta_data.array_sizes,
                                                               sizeof(uint32_t),
                                                               uint32_arrays_meta_data.num_arrays);

  uint32_arrays_meta_data.busy_flags = ResizeBusyMarkers(uint32_arrays_meta_data.busy_flags,
                                                         uint32_arrays_meta_data.num_arrays);
}

void AddFloat(uint32_t arrayId, uint32_t elementId, float element) {
  float_arrays[arrayId][elementId] = element;
}

void AddFloat3(uint32_t arrayId, uint32_t elementId, float element[3]) {
  float3_arrays[arrayId][elementId++] = element[0];
  float3_arrays[arrayId][elementId++] = element[1];
  float3_arrays[arrayId][elementId]   = element[2];
}

void AddUint32(uint32_t arrayId, uint32_t elementId, uint32_t element) {
  uint32_arrays[arrayId][elementId] = element;
}

void ResizeFloatArray(uint32_t arrayId, uint32_t elementsToAdd) {
  float_arrays_meta_data.array_sizes[arrayId] += elementsToAdd;
  ResizeArray((uint8_t*)float_arrays[arrayId], sizeof(float), float_arrays_meta_data.array_sizes[arrayId]);
}

void ResizeFloat3Array(uint32_t arrayId, uint32_t elementsToAdd) {
  float3_arrays_meta_data.array_sizes[arrayId] += elementsToAdd;
  ResizeArray((uint8_t*)float3_arrays[arrayId], sizeof(float[3]), float3_arrays_meta_data.array_sizes[arrayId]);
}

void ResizeUint32Array(uint32_t arrayId, uint32_t elementsToAdd) {
  uint32_arrays_meta_data.array_sizes[arrayId] += elementsToAdd;
  ResizeArray((uint8_t*)uint32_arrays[arrayId], sizeof(uint32_t), uint32_arrays_meta_data.array_sizes[arrayId]);
}
