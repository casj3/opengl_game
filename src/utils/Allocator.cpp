
#include "Allocator.h"

#include "Array.h"

// Different columns which will only be known to the allocator.
// It is the owner and sole distributor of allocated memory.
// It keeps pointers to arrays of pointers to other arrays.

#define INIT_NUM_ARRAYS 5

float** float_arrays;
uint32_t* float_arrays_num_elements;
uint32_t num_float_arrays;

float** float3_arrays;
uint32_t* float3_arrays_num_elements;
uint32_t num_float3_arrays;

uint32_t** uint32_arrays;
uint32_t* uint32_arrays_num_elements;
uint32_t num_uint32_arrays;

void InitAllocator() {
  float_arrays = (float**)NewArray(sizeof(float**), INIT_NUM_ARRAYS);
  float_arrays_num_elements = (uint32_t*)NewArray(sizeof(uint32_t*), INIT_NUM_ARRAYS);
  num_float_arrays = INIT_NUM_ARRAYS;

  float3_arrays = (float**)NewArray(sizeof(float**), INIT_NUM_ARRAYS);
  float3_arrays_num_elements = (uint32_t*)NewArray(sizeof(uint32_t*), INIT_NUM_ARRAYS);
  num_float3_arrays = INIT_NUM_ARRAYS;

  uint32_arrays = (uint32_t**)NewArray(sizeof(uint32_t**), INIT_NUM_ARRAYS);
  uint32_arrays_num_elements = (uint32_t*)NewArray(sizeof(uint32_t*), INIT_NUM_ARRAYS);
  num_uint32_arrays = INIT_NUM_ARRAYS;

  for(int i = 0; i < INIT_NUM_ARRAYS; i++) {
    float_arrays[i] = (float*)NewArray(sizeof(float), INIT_NUM_ARRAYS);
    float_arrays_num_elements[i] = INIT_NUM_ARRAYS;

    float3_arrays[i] = (float*)NewArray(sizeof(float[3]), INIT_NUM_ARRAYS);
    float3_arrays_num_elements[i] = INIT_NUM_ARRAYS;

    uint32_arrays[i] = (uint32_t*)NewArray(sizeof(uint32_t), INIT_NUM_ARRAYS);
    uint32_arrays_num_elements[i] = INIT_NUM_ARRAYS;
  }
}

// void AddFloat3Array(uint32_t size);
// void AddFloatArray(uint32_t size);
// void AddUint32Array(uint32_t size);
// 
// void AddFloat(uint32_t arrayId, float element);
// void AddFloat3(uint32_t arrayId, float element[3]);
