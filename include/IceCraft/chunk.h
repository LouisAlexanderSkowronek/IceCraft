#pragma once

#include "block.h"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_DEPTH 16
#define N_BLOCKS_PER_CHUNK (CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH)

struct Chunk
{
    unsigned placed_blocks;
    struct Block *blocks;
    struct BlockVertex *vertices;
};

void init_chunk(struct Chunk *chunk)
{
    chunk->placed_blocks = 0;
    chunk->blocks = (struct Block*) malloc(sizeof(struct Block) * N_BLOCKS_PER_CHUNK);
    chunk->vertices = (struct BlockVertex*) malloc(BLOCK_VERTICES_SIZE * N_BLOCKS_PER_CHUNK);
}

void add_block_to_chunk(float x, float y, float z, unsigned texture_id, struct Chunk *chunk)
{
    chunk->blocks[chunk->placed_blocks] = generate_block(x, y, z, texture_id, chunk->vertices + chunk->placed_blocks*BLOCK_N_VERTICES);
    chunk->placed_blocks++;
}