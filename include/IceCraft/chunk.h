#pragma once

#include "block.h"
#include "texture_atlas.h"

#define CHUNK_WIDTH 16
#define CHUNK_DEPTH 16
#define CHUNK_BASE_AREA (CHUNK_WIDTH * CHUNK_DEPTH)

struct Chunk
{
    unsigned VBO, VAO;
    unsigned placed_blocks;
    unsigned block_capacity;
    float x, z;
    struct Block *blocks;
    struct BlockVertex *vertices;
};

void init_chunk(float x, float z, struct Chunk *chunk, unsigned block_capacity);

void add_block_to_chunk(float x, float y, float z, unsigned texture_id, struct Chunk *chunk, struct TextureAtlas *texture_atlas);

void remove_block_from_chunk(unsigned index, struct Chunk *chunk);

unsigned get_block_located_at(float x, float y, float z, struct Chunk *chunk);

int block_does_exist(float x, float y, float z, struct Chunk *chunk);

void generate_chunk_vao_and_vbo(unsigned *VAO_ptr, unsigned *VBO_ptr, struct Chunk *chunk);