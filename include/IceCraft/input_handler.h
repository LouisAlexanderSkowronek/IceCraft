#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "IceCraft/player.h"
#include "IceCraft/world.h"
#include "IceCraft/gui_block_selector.h"

void processInput(GLFWwindow *window, struct GUIBlockSelector *hud, struct Player *player, struct World *world, int *show_coordinate_axes, int *c_key_is_blocked, int *space_key_is_blocked, const float delta);