#pragma once

#include "chunk.h"
#include "texture_atlas.h"

struct World
{
    struct Chunk *chunk;
};

void generate_flat_world(struct World *world, struct TextureAtlas *texture_atlas);

void world_free_chunk(struct World *world);