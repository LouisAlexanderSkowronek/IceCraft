#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gui_block_selector_vertex.h"

#define HUD_N_VERTICES 39
#define HUD_VERTICES_SIZE (HUD_N_VERTICES * sizeof(struct GUIBlockSelectorVertex))
#define HUD_N_ITEM_SLOTS 6

struct GUIBlockSelector
{
    struct GUIBlockSelectorVertex vertices[HUD_N_VERTICES];
    unsigned VAO, VBO;
    unsigned selected_block_index;
};


void generate_hud(struct GUIBlockSelector *hud);

void hud_update_vao_and_vbo(struct GUIBlockSelector *hud);

void hud_select_item(unsigned index, struct GUIBlockSelector *hud);