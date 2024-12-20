#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "IceCraft/camera.h"
#include "IceCraft/world.h"
#include "IceCraft/hud.h"

void processInput(GLFWwindow *window, struct HUD *hud, struct Camera *camera, struct World *world, int *show_coordinate_axes, int *c_key_is_blocked, const float delta);