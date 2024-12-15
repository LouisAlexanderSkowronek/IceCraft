#pragma once

#include "block_vertex.h"

#include <stdlib.h>
#include <stdio.h>


#define BLOCK_N_VERTICES 36
#define BLOCK_VERTICES_SIZE (BLOCK_N_VERTICES * sizeof(struct BlockVertex))

struct Block
{
    unsigned texture_id;
    struct BlockVertex *vertices;
};


struct Block generate_block(float x, float y, float z, unsigned material_id, struct BlockVertex *vertices);