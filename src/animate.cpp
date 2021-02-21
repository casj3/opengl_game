
#include "animate.h"

#include <math.h>

#include "utils/print.h"

glm::vec3 GetAnimatedValue(float animationTime, Bone_Anim_Keys boneAnimKeys) {
    size_t num_key_frames = GET_ARRAY_SIZE(boneAnimKeys.key_frames);
    if ((*boneAnimKeys.key_frames)[num_key_frames - 1] < animationTime) {
        return (*boneAnimKeys.values)[num_key_frames - 1];
    }
    size_t i = 0;
    while(i < num_key_frames - 1 && (*boneAnimKeys.key_frames)[i + 1] < animationTime) {
        i++;
    }
    float delta_time = (*boneAnimKeys.key_frames)[i + 1] - (*boneAnimKeys.key_frames)[i];
    float factor = (animationTime - (*boneAnimKeys.key_frames)[i]) / delta_time;

    glm::vec3 start = (*boneAnimKeys.values)[i];
    glm::vec3 end = (*boneAnimKeys.values)[i + 1];

    return start + factor * (end - start);
}

void AnimateBones(float animationTime, Skeleton skeleton) {
    animationTime = fmod(animationTime, skeleton.animation_duration);
    
    size_t num_bones = GET_ARRAY_SIZE(skeleton.bones);
    for(size_t i = 0; i < num_bones; i++) {
        if ((*skeleton.bone_anims)[i].animation_flags == Anim_Flags::NONE) {
            continue;
        }
        
        glm::vec3 pos;
        bool anim_transl = ((*skeleton.bone_anims)[i].animation_flags & Anim_Flags::POSITION) != 0;
        if (anim_transl) {
            pos = GetAnimatedValue(animationTime, (*skeleton.bone_anims)[i].bone_position_keys);
        } else {
            pos = (*skeleton.bone_transforms)[i].pos;
        }
        
        glm::vec3 rot;
        bool anim_rot = ((*skeleton.bone_anims)[i].animation_flags & Anim_Flags::ROTATION) != 0;
        if (anim_rot) {
            rot = GetAnimatedValue(animationTime, (*skeleton.bone_anims)[i].bone_rotation_keys);
        } else {
            rot = (*skeleton.bone_transforms)[i].rot;
        }
        
        glm::vec3 scale;
        bool anim_scale = ((*skeleton.bone_anims)[i].animation_flags & Anim_Flags::SCALE) != 0;
        if (anim_scale) {
            scale = GetAnimatedValue(animationTime, (*skeleton.bone_anims)[i].bone_scale_keys);
        } else {
            scale = (*skeleton.bone_transforms)[i].scale;
        }

        (*skeleton.bones)[i] = GetTransform(pos, rot, scale);
    }
}

int32_t GetAnimationFlags(bool pos, bool rot, bool scale) {
    int32_t anim_flags = 0;
    anim_flags |= (pos & 1);
    anim_flags |= (rot & 1) << 1;
    anim_flags |= (scale & 1) << 2;
    return anim_flags;
}
