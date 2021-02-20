#include "player.h"

void IncrementRotation(glm::vec3* direction, glm::vec3* rotation, RotationIncrementers rot, float deltaTime)
{
    rotation->z += (rot.increment_left + rot.increment_right) * deltaTime;
    direction->x = cosf(rotation->z);
    direction->y = sinf(rotation->z);
}

void SetCameraPos(glm::vec3* cameraPos, glm::vec3 otherPos)
{
    cameraPos->x = otherPos.x;
    cameraPos->y = otherPos.y;
    // Not the z-position since we don't want the camera to depend on the player along that axis
}

void Animate(float deltaTime, float* animTimer, AnimationStateKeyFrames animStates, StretchBooleans* stretch, GlideVariables* glide)
{
    *animTimer += deltaTime;

    // State 2 is when the avatar is stretched out
    if (*animTimer > animStates.state_2&& stretch->stretch_out && !glide->glide_ahead)
    {
        stretch->stretch_out = false;
    }
    else if (*animTimer > animStates.state_2 && stretch->stretch_out && glide->glide_ahead)
    {
        glide->glide_again = true;
        stretch->stretch_out = false;
    }
    else if (*animTimer > animStates.state_1)
    {
        stretch->pull_in = false;
        animTimer = 0;
    }
    else if (!stretch->stretch_out && stretch->pull_in && !glide->glide_ahead)
    {
        glide->glide_ahead = true;
    }
    else if (glide->glide_again && stretch->pull_in)
    {
        glide->glide_again = false;
        glide->glide_timer = 0;
    }
}

void Glide(float deltaTime, GlideVariables* glide, glm::vec3 direction, glm::vec3* pos)
{
    glide->glide_timer += deltaTime;

    float speed = 25;
    float deacceleration = 25;

    glm::vec3 velocity = glm::vec3(direction.x * speed, direction.y * speed, 0);
    glm::vec3 deaccelerator = glm::vec3(deacceleration * -direction.x * pow(glide->glide_timer, 2),
                                        deacceleration * -direction.y * pow(glide->glide_timer, 2), 0);

    if (length(velocity) < length(deaccelerator))
    {
        glide->glide_ahead = false;
        glide->glide_timer = 0;
    }
    else
    {
        pos->x += (velocity.x + deaccelerator.x) * deltaTime;
        pos->y += (velocity.y + deaccelerator.y) * deltaTime;
    }
}

// The calling layer above this function has to check whether mouse wheel input has been received or not to determine if this method should be called or not
void CameraSlide(MouseWheel* wheel, View* view, SlideProperties* slideProperties, float deltaTime)
{
    if (!slideProperties->slide)
    {
        if (wheel->down && view->pos.z > -35)
        {
            slideProperties->slide = true;
            slideProperties->track_value = -9;
            slideProperties->deacceleration = -3.3f;

            view->pos += Track(slideProperties->track_value, &slideProperties->slide_timer, deltaTime);
        }
        else if (wheel->up && view->pos.z < -8)
        {
            slideProperties->slide = true;
            slideProperties->track_value = 9;
            slideProperties->deacceleration = -3.3f;

            view->pos += Track(slideProperties->track_value, &slideProperties->slide_timer, deltaTime);
        }
    }
    else if (slideProperties->slide)
    {
        if (view->pos.z > -35 && view->pos.z < -8)
        {
            view->pos += Slide(slideProperties, deltaTime);
        }
        else
        {
            slideProperties->slide = false;
            slideProperties->slide_timer = 0;
        }
    }
}
