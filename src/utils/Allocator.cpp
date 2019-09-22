
#include "Allocator.h"

#include "Array.h"

// Different columns which will only be known to the allocator.
// It is the owner and sole distributor of allocated memory.
// It keeps pointers to arrays of pointers to other arrays.

#define INIT_NUM_ARRAYS 5
#define NUM_ARRAYS_RESIZE_ADDITION 10

// TODO: This needs padding
struct ArraysMetaData {
  int32_t* busy_flags;
  uint32_t* array_sizes;
  uint32_t num_arrays;
  uint32_t added_arrays;
};

float** float_arrays;
ArraysMetaData float_arrays_meta_data;

float** float3_arrays;
ArraysMetaData float3_arrays_meta_data;

uint32_t** uint32_arrays;
ArraysMetaData uint32_arrays_meta_data;

void InitAllocator() {
  float_arrays = (float**)NewArray(sizeof(float**), INIT_NUM_ARRAYS);
  float_arrays_meta_data.busy_flags = NewBusyMarkers(INIT_NUM_ARRAYS);
  float_arrays_meta_data.array_sizes = (uint32_t*)NewArray(sizeof(uint32_t), INIT_NUM_ARRAYS);
  float_arrays_meta_data.num_arrays = INIT_NUM_ARRAYS;
  float_arrays_meta_data.added_arrays = 0;

  float3_arrays = (float**)NewArray(sizeof(float**), INIT_NUM_ARRAYS);
  float3_arrays_meta_data.busy_flags = NewBusyMarkers(INIT_NUM_ARRAYS);
  float3_arrays_meta_data.array_sizes = (uint32_t*)NewArray(sizeof(uint32_t), INIT_NUM_ARRAYS);
  float3_arrays_meta_data.num_arrays = INIT_NUM_ARRAYS;
  float3_arrays_meta_data.added_arrays = 0;

  uint32_arrays = (uint32_t**)NewArray(sizeof(uint32_t**), INIT_NUM_ARRAYS);
  uint32_arrays_meta_data.busy_flags = NewBusyMarkers(INIT_NUM_ARRAYS);
  uint32_arrays_meta_data.array_sizes = (uint32_t*)NewArray(sizeof(uint32_t), INIT_NUM_ARRAYS);
  uint32_arrays_meta_data.num_arrays = INIT_NUM_ARRAYS;
  uint32_arrays_meta_data.added_arrays = 0;
}

uint32_t AddFloatArray(uint32_t size) {
  if (float_arrays_meta_data.added_arrays == float_arrays_meta_data.num_arrays) {
    float_arrays_meta_data.num_arrays += NUM_ARRAYS_RESIZE_ADDITION;

    float_arrays = (float**)ResizeArray((uint8_t*)float_arrays, sizeof(float**),
                    float_arrays_meta_data.num_arrays);

    float_arrays_meta_data.array_sizes = (uint32_t*)ResizeArray((uint8_t*)float_arrays_meta_data.array_sizes,
                                                                sizeof(uint32_t),
                                                                float_arrays_meta_data.num_arrays);

    float_arrays_meta_data.busy_flags = ResizeBusyMarkers(float_arrays_meta_data.busy_flags,
                                                          float_arrays_meta_data.num_arrays);
  }

  uint32_t freeIndex = GetFreeIndex(float_arrays_meta_data.busy_flags);
  float* float_array = (float*)NewArray(sizeof(float), size);
  float_arrays[freeIndex] = float_array;
  float_arrays_meta_data.array_sizes[freeIndex] = size;
  float_arrays_meta_data.added_arrays++;

  return freeIndex;
}

uint32_t AddFloat3Array(uint32_t size){
  if (float3_arrays_meta_data.added_arrays == float3_arrays_meta_data.num_arrays) {
    float3_arrays_meta_data.num_arrays += NUM_ARRAYS_RESIZE_ADDITION;

    float3_arrays = (float**)ResizeArray((uint8_t*)float3_arrays, sizeof(float**),
                     float3_arrays_meta_data.num_arrays);

    float3_arrays_meta_data.array_sizes = (uint32_t*)ResizeArray((uint8_t*)float3_arrays_meta_data.array_sizes,
                                                                 sizeof(uint32_t),
                                                                 float3_arrays_meta_data.num_arrays);

    float3_arrays_meta_data.busy_flags = ResizeBusyMarkers(float3_arrays_meta_data.busy_flags,
                                                           float3_arrays_meta_data.num_arrays);
  }

  uint32_t freeIndex = GetFreeIndex(float3_arrays_meta_data.busy_flags);
  float* float3_array = (float*)NewArray(sizeof(float[3]), size);
  float3_arrays[freeIndex] = float3_array;
  float3_arrays_meta_data.array_sizes[freeIndex] = size;
  float3_arrays_meta_data.added_arrays++;

  return freeIndex;
}

uint32_t AddUint32Array(uint32_t size) {
  if (uint32_arrays_meta_data.added_arrays == uint32_arrays_meta_data.num_arrays) {
    uint32_arrays_meta_data.num_arrays += NUM_ARRAYS_RESIZE_ADDITION;

    uint32_arrays = (uint32_t**)ResizeArray((uint8_t*)uint32_arrays, sizeof(uint32_t**),
                                            uint32_arrays_meta_data.num_arrays);

    uint32_arrays_meta_data.array_sizes = (uint32_t*)ResizeArray((uint8_t*)uint32_arrays_meta_data.array_sizes,
                                                                 sizeof(uint32_t),
                                                                 uint32_arrays_meta_data.num_arrays);

    uint32_arrays_meta_data.busy_flags = ResizeBusyMarkers(uint32_arrays_meta_data.busy_flags,
                                                           uint32_arrays_meta_data.num_arrays);
  }

  uint32_t freeIndex = GetFreeIndex(uint32_arrays_meta_data.busy_flags);
  uint32_t* uint32_array = (uint32_t*)NewArray(sizeof(uint32_t), size);
  uint32_arrays[freeIndex] = uint32_array;
  uint32_arrays_meta_data.array_sizes[freeIndex] = size;
  uint32_arrays_meta_data.added_arrays++;

  return freeIndex;
}

// void AddFloat(uint32_t arrayId, float element);
// void AddFloat3(uint32_t arrayId, float element[3]);
