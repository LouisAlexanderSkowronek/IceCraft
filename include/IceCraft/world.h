#pragma once

#include "block.h"

struct World
{
    unsigned n_blocks, placed_blocks;
    struct BlockVertex *vertices;
};

void init_world(struct World *world, unsigned n_blocks)
{
    world->n_blocks = n_blocks;
    world->placed_blocks = 0;
    world->vertices = (struct BlockVertex*) malloc(BLOCK_VERTICES_SIZE * n_blocks);
}

void add_block(float x, float y, float z, struct World *world)
{
    generate_block(x, y, z, world->vertices + world->placed_blocks*BLOCK_N_VERTICES);
    world->placed_blocks++;
}