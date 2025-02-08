#pragma once

#include "chunk.h"
#include "texture_atlas.h"

enum WorldType
{
    LOBBY,
    ICE_WORLD
};

struct World
{
    struct Chunk *chunk;
};

void init_world(struct World *world);

void generate_lobby_world(struct World *world, struct TextureAtlas *texture_atlas);

void generate_ice_world(struct World *world, struct TextureAtlas *texture_atlas);

void world_free_chunk(struct World *world);