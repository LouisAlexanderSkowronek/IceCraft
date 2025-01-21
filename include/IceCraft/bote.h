#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cglm/cglm.h"

#include "block_vertex.h"

struct Bote
{
    vec3 position;
    struct BlockVertex *vertices;
    unsigned n_vertices;
    unsigned VAO, VBO;
};


void init_bote(struct Bote *bote);

void bote_generate_vao_vbo(struct Bote *bote);