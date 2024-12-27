#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gui_crossbar_vertex.h"

#define GUI_CROSSBAR_N_VERTICES 12
#define GUI_CROSSBAR_VERTICES_SIZE (GUI_CROSSBAR_N_VERTICES * sizeof(struct GUICrossbarVertex))

struct GUICrossbar
{
    struct GUICrossbarVertex vertices[GUI_CROSSBAR_N_VERTICES];
    unsigned VAO, VBO;
};

void generate_crossbar(struct GUICrossbar *crossbar);

void crossbar_update_vao_and_vbo(struct GUICrossbar *crossbar);
