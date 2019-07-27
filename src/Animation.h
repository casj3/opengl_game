#pragma once

#include <glm/glm.hpp>
#include <vector>

struct SkeletonSuper
{
	std::vector<std::vector<glm::mat4>> key_bone_frames;
	glm::mat4* animation_bone_transforms = nullptr;
	int bone_amount;
  // Not sure addition
  int entity_id;
};

///struct SkeletonTable {
//  Array<int> entity;
//  Array<glm::mat4> bones;
//};

// This method presupposes an exact correlation between animation keys and the indicies of the matrix std::vector within the vector
void SetAnimationBones(float animationTime, SkeletonSuper* animSuper);

// The key_bone_frames mustn't be deallocated because it is a std::vector
void DestroySkeleton(glm::mat4* animaitonBoneTransforms);
