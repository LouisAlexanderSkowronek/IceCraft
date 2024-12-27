#include "IceCraft/gui_crossbar.h"

void generate_crossbar(struct GUICrossbar *crossbar)
{
    crossbar->vertices[0].x = -0.01f;
    crossbar->vertices[0].y = +0.05f;
    crossbar->vertices[0].r = 1.0f;
    crossbar->vertices[0].g = 1.0f;
    crossbar->vertices[0].b = 1.0f;

    crossbar->vertices[1].x = -0.01f;
    crossbar->vertices[1].y = -0.05f;
    crossbar->vertices[1].r = 1.0f;
    crossbar->vertices[1].g = 1.0f;
    crossbar->vertices[1].b = 1.0f;

    crossbar->vertices[2].x = +0.01f;
    crossbar->vertices[2].y = -0.05f;
    crossbar->vertices[2].r = 1.0f;
    crossbar->vertices[2].g = 1.0f;
    crossbar->vertices[2].b = 1.0f;

    crossbar->vertices[3].x = -0.01f;
    crossbar->vertices[3].y = +0.05f;
    crossbar->vertices[3].r = 1.0f;
    crossbar->vertices[3].g = 1.0f;
    crossbar->vertices[3].b = 1.0f;

    crossbar->vertices[4].x = +0.01f;
    crossbar->vertices[4].y = +0.05f;
    crossbar->vertices[4].r = 1.0f;
    crossbar->vertices[4].g = 1.0f;
    crossbar->vertices[4].b = 1.0f;

    crossbar->vertices[5].x = +0.01f;
    crossbar->vertices[5].y = -0.05f;
    crossbar->vertices[5].r = 1.0f;
    crossbar->vertices[5].g = 1.0f;
    crossbar->vertices[5].b = 1.0f;

    //

    crossbar->vertices[6].x = -0.05f;
    crossbar->vertices[6].y = +0.01f;
    crossbar->vertices[6].r = 1.0f;
    crossbar->vertices[6].g = 1.0f;
    crossbar->vertices[6].b = 1.0f;

    crossbar->vertices[7].x = -0.05f;
    crossbar->vertices[7].y = -0.01f;
    crossbar->vertices[7].r = 1.0f;
    crossbar->vertices[7].g = 1.0f;
    crossbar->vertices[7].b = 1.0f;

    crossbar->vertices[8].x = +0.05f;
    crossbar->vertices[8].y = -0.01f;
    crossbar->vertices[8].r = 1.0f;
    crossbar->vertices[8].g = 1.0f;
    crossbar->vertices[8].b = 1.0f;

    crossbar->vertices[9].x = -0.05f;
    crossbar->vertices[9].y = +0.01f;
    crossbar->vertices[9].r = 1.0f;
    crossbar->vertices[9].g = 1.0f;
    crossbar->vertices[9].b = 1.0f;

    crossbar->vertices[10].x = +0.05f;
    crossbar->vertices[10].y = +0.01f;
    crossbar->vertices[10].r = 1.0f;
    crossbar->vertices[10].g = 1.0f;
    crossbar->vertices[10].b = 1.0f;

    crossbar->vertices[11].x = +0.05f;
    crossbar->vertices[11].y = -0.01f;
    crossbar->vertices[11].r = 1.0f;
    crossbar->vertices[11].g = 1.0f;
    crossbar->vertices[11].b = 1.0f;

    crossbar_update_vao_and_vbo(crossbar);
}

void crossbar_update_vao_and_vbo(struct GUICrossbar *crossbar)
{
    // Generate and bind the VAO
    glGenVertexArrays(1, &crossbar->VAO);
    glBindVertexArray(crossbar->VAO);

    // Generate and bind VBO
    glGenBuffers(1, &crossbar->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, crossbar->VBO);
    glBufferData(GL_ARRAY_BUFFER, GUI_CROSSBAR_VERTICES_SIZE, crossbar->vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct GUICrossbarVertex), (GLvoid*)0);  // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct GUICrossbarVertex), (GLvoid*)(2 * sizeof(GLfloat)));  // Color attributes
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}