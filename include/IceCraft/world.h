#pragma once

#include "block.h"

struct World
{
    unsigned n_blocks, placed_blocks;
    struct Block *blocks;
    struct BlockVertex *vertices;
};

void init_world(struct World *world, unsigned n_blocks)
{
    world->n_blocks = n_blocks;
    world->placed_blocks = 0;
    world->blocks = (struct Block*) malloc(sizeof(struct Block) * n_blocks);
    world->vertices = (struct BlockVertex*) malloc(BLOCK_VERTICES_SIZE * n_blocks);
}

void add_block(float x, float y, float z, unsigned texture_id, struct World *world)
{
    world->blocks[world->placed_blocks] = generate_block(x, y, z, texture_id, world->vertices + world->placed_blocks*BLOCK_N_VERTICES);
    world->placed_blocks++;
}