
#include "Animation.h"

#include <math.h>

#include "utils/Print.h"

glm::vec3 GetAnimatedValue(float animationTime, BoneAnimKeys boneAnimKeys) {
    uint32_t numKeyFrames = GetArraySize<>(boneAnimKeys.key_frames);
    uint32_t i = 0;
    // TODO: Revisit these conditions.
    while(i < numKeyFrames - 1 && boneAnimKeys.key_frames[i + 1] < animationTime) {
        i++;
    }
    Print("Animation time between IDs %u and %u\n", i, i + 1);
    float deltaTime = boneAnimKeys.key_frames[i + 1] - boneAnimKeys.key_frames[i];
    float factor = (animationTime - boneAnimKeys.key_frames[i]) / deltaTime;

    glm::vec3 start = boneAnimKeys.values[i];
    glm::vec3 end = boneAnimKeys.values[i + 1];

    return start + factor * (end - start);
}

void AnimateBones(float animationTime, Skeleton skeleton) {
    animationTime = fmod(animationTime, skeleton.animation_duration);

    Print("animationTime=%.6f\n", animationTime);
    
    uint32_t numBones = GetArraySize<>(skeleton.bones);
    for(uint32_t i = 0; i < numBones; i++) {
        if (skeleton.bone_anims[i].animation_flags == AnimationFlags::NONE) {
            continue;
        }
        
        glm::vec3 pos;
        if (skeleton.bone_anims[i].animation_flags & AnimationFlags::POSITION != 0) {
            pos = GetAnimatedValue(animationTime, skeleton.bone_anims[i].bone_position_keys);
        } else {
            pos = skeleton.bone_transforms[i].pos;
        }
        
        glm::vec3 rot;
        if (skeleton.bone_anims[i].animation_flags & AnimationFlags::ROTATION != 0) {
            rot = GetAnimatedValue(animationTime, skeleton.bone_anims[i].bone_rotation_keys);
        } else {
            rot = skeleton.bone_transforms[i].rot;
        }
        
        glm::vec3 scale;
        if (skeleton.bone_anims[i].animation_flags & AnimationFlags::SCALE != 0) {
            scale = GetAnimatedValue(animationTime, skeleton.bone_anims[i].bone_scale_keys);
        } else {
            scale = skeleton.bone_transforms[i].scale;
        }

        skeleton.bones[i] = GetTransform(pos, rot, scale);
    }
}

int32_t GetAnimationFlags(bool position, bool rotation, bool scale) {
    int32_t animationFlags = 0;
    animationFlags |= (position & 1);
    animationFlags |= (rotation & 1) << 1;
    animationFlags |= (scale & 1) << 2;
    return animationFlags;
}
