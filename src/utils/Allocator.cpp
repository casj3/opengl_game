
#include "Allocator.h"

#include "Animation.h"
#include "VAO_Data.h"
#include "LoadVAO.h"

namespace Allocator {
void InitAllocator(uint32_t capacity) {
    InitArrays<float>(capacity);
    InitArrays<glm::vec3>(capacity);
    InitArrays<uint32_t>(capacity);
    InitArrays<SkeletonPositions>(capacity);
    InitArrays<SkeletonRotations>(capacity);
    InitArrays<SkeletonScalings>(capacity);
    InitArrays<BonePositionKeys>(capacity);
    InitArrays<BoneRotationKeys>(capacity);
    InitArrays<BoneScaleKeys>(capacity);
    InitArrays<SkeletalVertex>(capacity);
    InitArrays<NodeKeyFrameValues>(capacity);
    InitArrays<Pair<uint32_t, uint32_t>>(capacity);
    InitArrays<Pair<uint32_t, glm::vec3>>(capacity);
}
}
