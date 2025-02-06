#pragma once

#include "cglm/cglm.h"

#define CAMERA_SPEED 2.5f
#define CAMERA_SENSITIVITY 25.0f

struct Camera
{
    float pitch, yaw;
    vec3 position, front, up;
};


void init_camera(struct Camera *camera);

void move_camera(struct Camera *camera, vec3 offset);

void rotate_camera(struct Camera *camera, float pitch_offset, float yaw_offset);