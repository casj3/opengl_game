
#include "ArrayManager.h"

#include "Animation.h"
#include "LoadVAO.h"
#include "Transform.h"
#include "VAO_Data.h"

void InitArrayManager(uint32_t capacity) {
    InitArrays<float>(capacity);
    InitArrays<glm::vec3>(capacity);
    InitArrays<glm::mat4>(capacity); 
    InitArrays<Transform>(capacity);
    InitArrays<uint32_t>(capacity);
    InitArrays<Skeleton>(capacity);
    InitArrays<BoneAnimation>(capacity);
    InitArrays<SkeletalVertex>(capacity);
    InitArrays<NodeKeyFrameValues>(capacity);
    InitArrays<Pair<uint32_t, uint32_t>>(capacity);
    InitArrays<Pair<uint32_t, glm::vec3>>(capacity);
}
