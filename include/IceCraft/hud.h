#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "hud_vertex.h"

#define HUD_N_VERTICES 39
#define HUD_VERTICES_SIZE (HUD_N_VERTICES * sizeof(struct HUDVertex))

struct HUD
{
    struct HUDVertex vertices[HUD_N_VERTICES];
    unsigned VAO, VBO;
    unsigned selected_block_index;
};


void generate_hud(struct HUD *hud);

void generate_hud_vao_and_vbo(unsigned *VAO_ptr, unsigned *VBO_ptr, struct HUD *hud);

void hud_select_item(unsigned index, struct HUD *hud);