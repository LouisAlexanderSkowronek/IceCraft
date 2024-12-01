#pragma once

#include "block_vertex.h"

#include <stdlib.h>
#include <stdio.h>


#define BLOCK_N_VERTICES 36
#define BLOCK_VERTICES_SIZE (BLOCK_N_VERTICES * sizeof(struct BlockVertex))

struct Block
{
    struct BlockVertex *vertices;
};


struct Block generate_block();