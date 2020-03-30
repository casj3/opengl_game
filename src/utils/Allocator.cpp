
#include "Allocator.h"

#include "Animation.h"

namespace Allocator {
void InitAllocator(uint32_t capacity) {
    InitArrays<float>(capacity);
    InitArrays<glm::vec3>(capacity);
    InitArrays<uint32_t>(capacity);
    InitArrays<SkeletonPositions>(capacity);
    InitArrays<SkeletonRotations>(capacity);
    InitArrays<SkeletonScales>(capacity);
    InitArrays<BonePositionKeys>(capacity);
    InitArrays<BoneRotationKeys>(capacity);
    InitArrays<BoneScaleKeys>(capacity);
}
}
