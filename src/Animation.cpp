
#include "Animation.h"

// This method presupposes an exact correlation between animation keys and
// the indicies of the matrix vector within the vector
void SetAnimationBones(float animationTime, SkeletonSuper* animSuper)
{
	// The - 1 addition may not be necessary - unsure at the moment. The indices for
  // the matrices specify the keyframes for each bone and the corresponding transforms.
	animationTime = fmod(animationTime, animSuper->key_bone_frames[0].size() - 1);

  // Casting the animation time to an int to get rid of the decimals
  // and thereby making it stand side by side with an animation key.
	float keyFrameStart = (int)animationTime;
	float keyFrameEnd = keyFrameStart + 1;

	float deltaTime = keyFrameEnd - keyFrameStart;
	float factor = (animationTime - keyFrameStart) / deltaTime;
	assert(factor >= 0.0f && factor <= 1.0f);

	for (int i = 0; i < animSuper->key_bone_frames.size(); i++)
	{
		glm::mat4 start = animSuper->key_bone_frames[i][keyFrameStart];
		glm::mat4 end = animSuper->key_bone_frames[i][keyFrameEnd];
		glm::mat4 current;

		// Interpolation for each element in the matrix
		current[0][0] = start[0][0] + factor * (end[0][0] - start[0][0]);	current[0][1] = start[0][1] + factor * (end[0][1] - start[0][1]);
		current[0][2] = start[0][2] + factor * (end[0][2] - start[0][2]);	current[0][3] = start[0][3] + factor * (end[0][3] - start[0][3]);
		current[1][0] = start[1][0] + factor * (end[1][0] - start[1][0]);	current[1][1] = start[1][1] + factor * (end[1][1] - start[1][1]);
		current[1][2] = start[1][2] + factor * (end[1][2] - start[1][2]);	current[1][3] = start[1][3] + factor * (end[1][3] - start[1][3]);
		current[2][0] = start[2][0] + factor * (end[2][0] - start[2][0]);	current[2][1] = start[2][1] + factor * (end[2][1] - start[2][1]);
		current[2][2] = start[2][2] + factor * (end[2][2] - start[2][2]);	current[2][3] = start[2][3] + factor * (end[2][3] - start[2][3]);
		current[3][0] = start[3][0] + factor * (end[3][0] - start[3][0]);	current[3][1] = start[3][1] + factor * (end[3][1] - start[3][1]);
		current[3][2] = start[3][2] + factor * (end[3][2] - start[3][2]);	current[3][3] = start[3][3] + factor * (end[3][3] - start[3][3]);

		animSuper->animation_bone_transforms[i] = current;
	}
}

void DestroySkeleton(glm::mat4* animaitonBoneTransforms)
{
	free(animaitonBoneTransforms);
}
