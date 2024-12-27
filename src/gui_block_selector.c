#include "IceCraft/gui_block_selector.h"

static void draw_rect(float x, float y, float r, float g, float b, struct GUIBlockSelectorVertex *vertices)
{
    const float half_edge_size = 0.1f;
    vertices[0].x = x - half_edge_size;
    vertices[0].y = y + half_edge_size;
    vertices[0].r = r;
    vertices[0].g = g;
    vertices[0].b = b;

    vertices[1].x = x - half_edge_size;
    vertices[1].y = y - half_edge_size;
    vertices[1].r = r;
    vertices[1].g = g;
    vertices[1].b = b;

    vertices[2].x = x + half_edge_size;
    vertices[2].y = y - half_edge_size;
    vertices[2].r = r;
    vertices[2].g = g;
    vertices[2].b = b;

    vertices[3].x = x - half_edge_size;
    vertices[3].y = y + half_edge_size;
    vertices[3].r = r;
    vertices[3].g = g;
    vertices[3].b = b;

    vertices[4].x = x + half_edge_size;
    vertices[4].y = y + half_edge_size;
    vertices[4].r = r;
    vertices[4].g = g;
    vertices[4].b = b;

    vertices[5].x = x + half_edge_size;
    vertices[5].y = y - half_edge_size;
    vertices[5].r = r;
    vertices[5].g = g;
    vertices[5].b = b;
}

void generate_hud(struct GUIBlockSelector *hud)
{
    const float dirt_r = 0.5882352941176471f;
    const float dirt_g = 0.29411764;

    const float x = -0.8f;
    const float y = -0.8f;

    draw_rect(-0.8f, -0.8f, dirt_r, dirt_g, 0.0f, hud->vertices);
    draw_rect(-0.6f, -0.8f, 0.0f, 1.0f, 0.0f, hud->vertices + 6);
    draw_rect(-0.4f, -0.8f, 0.0f, 0.0f, 1.0f, hud->vertices + 12);
    draw_rect(-0.2f, -0.8f, 0.6f, 0.6f, 0.9f, hud->vertices + 18);
    draw_rect(+0.0f, -0.8f, dirt_r - 0.3f, dirt_g - 0.1f, 0.0f, hud->vertices + 24);
    draw_rect(+0.2f, -0.8f, 0.3f, 0.2f, 0.3f, hud->vertices + 30);

    hud_select_item(0, hud);

    hud_update_vao_and_vbo(hud);

}

void hud_update_vao_and_vbo(struct GUIBlockSelector *hud)
{
    // Generate and bind the VAO
    glGenVertexArrays(1, &hud->VAO);
    glBindVertexArray(hud->VAO);

    // Generate and bind VBO
    glGenBuffers(1, &hud->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, hud->VBO);
    glBufferData(GL_ARRAY_BUFFER, HUD_VERTICES_SIZE, hud->vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct GUIBlockSelectorVertex), (GLvoid*)0);  // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct GUIBlockSelectorVertex), (GLvoid*)(2 * sizeof(GLfloat)));  // Color attributes
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void hud_select_item(unsigned index, struct GUIBlockSelector *hud)
{
    const unsigned max_index = 5;

    if (index > max_index)
    {
        return;
    }

    hud->selected_block_index = index;

    float x = -0.8f;
    const float step_size = 0.2f;
    for (unsigned i = 0; i < index; i++)
    {
        x += step_size;
    }

    hud->vertices[HUD_N_VERTICES-3].x = x;
    hud->vertices[HUD_N_VERTICES-3].y = -0.9f;
    hud->vertices[HUD_N_VERTICES-3].r = 1.0f;
    hud->vertices[HUD_N_VERTICES-3].g = 0.0f;
    hud->vertices[HUD_N_VERTICES-3].b = 0.0f;

    hud->vertices[HUD_N_VERTICES-2].x = x - 0.05f;
    hud->vertices[HUD_N_VERTICES-2].y = -0.95f;
    hud->vertices[HUD_N_VERTICES-2].r = 1.0f;
    hud->vertices[HUD_N_VERTICES-2].g = 0.0f;
    hud->vertices[HUD_N_VERTICES-2].b = 0.0f;

    hud->vertices[HUD_N_VERTICES-1].x = x + 0.05f;
    hud->vertices[HUD_N_VERTICES-1].y = -0.95f;
    hud->vertices[HUD_N_VERTICES-1].r = 1.0f;
    hud->vertices[HUD_N_VERTICES-1].g = 0.0f;
    hud->vertices[HUD_N_VERTICES-1].b = 0.0f;

    hud_update_vao_and_vbo(hud);
}