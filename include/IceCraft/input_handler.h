#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "IceCraft/camera.h"

void processInput(GLFWwindow *window, struct Camera *camera, int *show_coordinate_axes, int *c_key_is_blocked, const float delta);