#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void init_glfw();

void init_glad();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLFWwindow* create_window(unsigned width, unsigned height, const char *title);

unsigned build_shader_program(const char *vertex_shader_filepath, const char *fragment_shader_filepath);