#pragma once

#include <GLFW/glfw3.h>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define WINDOW_TITLE "IceCraft"


void init_glfw();

void init_glad();

GLFWwindow* create_window(unsigned width, unsigned height, const char *title);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
