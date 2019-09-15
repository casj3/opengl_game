
#include "Allocator.h"

#include "Array.h"

// Different columns which will only be known to the allocator.
// It is the owner and sole distributor of allocated memory.
// It keeps pointers to arrays of pointers to other arrays.

#define INIT_NUM_ARRAYS 5

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

void InitAllocator() {
  float_arrays = (float**)NewArray(sizeof(float**), INIT_NUM_ARRAYS);
  float_arrays_meta_data.busy_flags = NewBusyMarkers(INIT_NUM_ARRAYS);
  float_arrays_meta_data.array_sizes = (uint32_t*)NewArray(sizeof(float), INIT_NUM_ARRAYS);
  float_arrays_meta_data.num_arrays = INIT_NUM_ARRAYS;

  float3_arrays = (float**)NewArray(sizeof(float**), INIT_NUM_ARRAYS);
  float3_arrays_meta_data.busy_flags = NewBusyMarkers(INIT_NUM_ARRAYS);
  float3_arrays_meta_data.array_sizes = (uint32_t*)NewArray(sizeof(float), INIT_NUM_ARRAYS);
  float3_arrays_meta_data.num_arrays = INIT_NUM_ARRAYS;

  uint32_arrays = (uint32_t**)NewArray(sizeof(uint32_t**), INIT_NUM_ARRAYS);
  uint32_arrays_meta_data.busy_flags = NewBusyMarkers(INIT_NUM_ARRAYS);
  uint32_arrays_meta_data.array_sizes = (uint32_t*)NewArray(sizeof(float), INIT_NUM_ARRAYS);
  uint32_arrays_meta_data.num_arrays = INIT_NUM_ARRAYS;
}

// TODO: Vi måste ha ett sätt att garantera att det alltyid finns en plats att ta i busy_flags så
// vi vet när vi ska ändra storleken på arrayen för arrayer.
uint32_t AddFloatArray(uint32_t size) {
  uint32_t freeIndex = GetFreeIndex(float_arrays_meta_data.busy_flags);
  float* float_array = (float*)NewArray(sizeof(float), size);
  AddElement((uint8_t*)float_arrays, (uint8_t*)float_array, sizeof(float*), freeIndex);

  return freeIndex;
}
//uint32_t AddFloat3Array(uint32_t size);
//uint32_t AddUint32Array(uint32_t size);

// void AddFloat(uint32_t arrayId, float element);
// void AddFloat3(uint32_t arrayId, float element[3]);
