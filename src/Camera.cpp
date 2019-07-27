#include "Camera.h"

//#include <glm\gtx\transform.hpp>

glm::mat4 ViewMatrix(View view)
{
	return glm::lookAt(view.pos, view.pos + view.forward, view.up);
}

glm::mat4 ProjectionMatrix(Projection projection)
{
	return glm::perspective(glm::radians(projection.fov), projection.aspect, projection.z_near, projection.z_far);
}

glm::mat4 OrthoProjectionMatrix(OrthoProjection projection)
{
	return glm::ortho(0.0f, (float)WIDTH, 0.0f, float(HEIGHT), projection.z_near, projection.z_far);
}

// The resulting float is meant to be added to the z component of the view position instanced at a higher level
float Track(float trackValue, float* slideTimer, float deltaTime)
{
	slideTimer = 0;
	return trackValue * deltaTime;
}

// This function only works in co-operation with Track - both use the same slideTimer instance
float Slide(SlideProperties* slideProperties, float deltaTime)
{
	slideProperties->slide_timer += deltaTime;

	slideProperties->acceleration = slideProperties->deacceleration * pow(slideProperties->slide_timer, 2);

	if (slideProperties->track_value < -slideProperties->acceleration && slideProperties->track_value > 0 ||
		slideProperties->track_value > -slideProperties->acceleration && slideProperties->track_value < 0)
	{
		slideProperties->slide = false;
		slideProperties->slide_timer = 0;
		return 0;
	}
	else
	{
		return (slideProperties->track_value + slideProperties->acceleration) * deltaTime;
	}
}
