#pragma once

#include "chunk.h"

#define WORLD_N_CHUNKS 1

struct World
{
    struct Chunk *chunk_ptrs[WORLD_N_CHUNKS];
};

void generate_flat_world(struct World *world);

void update_loaded_chunks(float player_x, float player_z, struct World *world);

void world_free_chunks(struct World *world);