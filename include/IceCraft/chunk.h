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

void init_chunk(struct Chunk *chunk);

void add_block_to_chunk(float x, float y, float z, unsigned texture_id, struct Chunk *chunk);

void generate_chunk_vao_and_vbo(unsigned *VAO_ptr, unsigned *VBO_ptr, struct Chunk *chunk);