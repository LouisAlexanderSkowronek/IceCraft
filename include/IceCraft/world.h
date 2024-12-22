#pragma once

#include "chunk.h"

struct World
{
    struct Chunk *chunk;
};

void generate_flat_world(struct World *world);

void world_free_chunk(struct World *world);