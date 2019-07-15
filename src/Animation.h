#pragma once

#include <glm/glm.hpp>
#include <vector>

using namespace glm;
using namespace std;

struct SkeletonSuper
{
	vector<vector<mat4>> key_bone_frames;
	mat4* animation_bone_transforms = nullptr;
	int bone_amount;
  // Not sure addition
  int entity_id;
};

///struct SkeletonTable {
//  Array<int> entity;
//  Array<mat4> bones;
//};

// This method presupposes an exact correlation between animation keys and the indicies of the matrix vector within the vector
void SetAnimationBones(float animationTime, SkeletonSuper* animSuper);

// The key_bone_frames mustn't be deallocated because it is a vector
void DestroySkeleton(mat4* animaitonBoneTransforms);
