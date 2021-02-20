
#include "animate.h"

#include <math.h>

#include "utils/print.h"

glm::vec3 GetAnimatedValue(float animationTime, BoneAnimKeys boneAnimKeys) {
    size_t numKeyFrames = GET_ARRAY_SIZE(boneAnimKeys.key_frames);
    if ((*boneAnimKeys.key_frames)[numKeyFrames - 1] < animationTime) {
        return (*boneAnimKeys.values)[numKeyFrames - 1];
    }
    size_t i = 0;
    while(i < numKeyFrames - 1 && (*boneAnimKeys.key_frames)[i + 1] < animationTime) {
        i++;
    }
    float deltaTime = (*boneAnimKeys.key_frames)[i + 1] - (*boneAnimKeys.key_frames)[i];
    float factor = (animationTime - (*boneAnimKeys.key_frames)[i]) / deltaTime;

    glm::vec3 start = (*boneAnimKeys.values)[i];
    glm::vec3 end = (*boneAnimKeys.values)[i + 1];

    return start + factor * (end - start);
}

void AnimateBones(float animationTime, Skeleton skeleton) {
    animationTime = fmod(animationTime, skeleton.animation_duration);
    
    size_t numBones = GET_ARRAY_SIZE(skeleton.bones);
    for(size_t i = 0; i < numBones; i++) {
        if ((*skeleton.bone_anims)[i].animation_flags == AnimationFlags::NONE) {
            continue;
        }
        
        glm::vec3 pos;
        bool animateTranslation = ((*skeleton.bone_anims)[i].animation_flags & AnimationFlags::POSITION) != 0;
        if (animateTranslation) {
            pos = GetAnimatedValue(animationTime, (*skeleton.bone_anims)[i].bone_position_keys);
        } else {
            pos = (*skeleton.bone_transforms)[i].pos;
        }
        
        glm::vec3 rot;
        bool animateRotation = ((*skeleton.bone_anims)[i].animation_flags & AnimationFlags::ROTATION) != 0;
        if (animateRotation) {
            rot = GetAnimatedValue(animationTime, (*skeleton.bone_anims)[i].bone_rotation_keys);
        } else {
            rot = (*skeleton.bone_transforms)[i].rot;
        }
        
        glm::vec3 scale;
        bool animateScale = ((*skeleton.bone_anims)[i].animation_flags & AnimationFlags::SCALE) != 0;
        if (animateScale) {
            scale = GetAnimatedValue(animationTime, (*skeleton.bone_anims)[i].bone_scale_keys);
        } else {
            scale = (*skeleton.bone_transforms)[i].scale;
        }

        (*skeleton.bones)[i] = GetTransform(pos, rot, scale);
    }
}

int32_t GetAnimationFlags(bool position, bool rotation, bool scale) {
    int32_t animationFlags = 0;
    animationFlags |= (position & 1);
    animationFlags |= (rotation & 1) << 1;
    animationFlags |= (scale & 1) << 2;
    return animationFlags;
}
