#include "IceCraft/camera.h"


void init_camera(struct Camera *camera)
{
    camera->pitch = 0.0f;
    camera->yaw = -90.0f;

    camera->position[0] = 0.0f;
    camera->position[1] = 4.8f;
    camera->position[2] = 0.0f;

    camera->front[0] = camera->front[1] = 0.0f;
    camera->front[2] = -1.0f;

    camera->up[0] = camera->up[2] = 0.0f;
    camera->up[1] = 1.0f;
}

void move_camera(struct Camera *camera, vec3 offset)
{
    glm_vec3_add(
        camera->position,
        offset,
        camera->position
    );
}

void rotate_camera(struct Camera *camera, float pitch_offset, float yaw_offset)
{
    camera->pitch += pitch_offset;
    camera->yaw += yaw_offset;

    if (camera->pitch > 89.0f)
    {
        camera->pitch = 89.0f;
    }

    if (camera->pitch < -89.0f)
    {
        camera->pitch = -89.0f;
    }

    vec3 tmp_front;
    tmp_front[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
    tmp_front[1] = sinf(glm_rad(camera->pitch));
    tmp_front[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
    glm_normalize(tmp_front);
    glm_vec3_copy(tmp_front, camera->front);
}