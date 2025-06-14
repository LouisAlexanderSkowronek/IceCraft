#pragma once

#include "chunk.h"
#include "texture_atlas.h"

enum WorldType
{
    LOBBY,
    ICE_WORLD
};

enum WorldModificationType
{
    PLACED,
    DESTROYED
};

struct WorldModification
{
    enum WorldModificationType type;
    int x, y, z;
    unsigned material_id;  // Doesn't have to be initialized when type == DESTROYED
};

struct World
{
    struct Chunk *cached_chunks;
    unsigned n_cached_chunks;
    struct WorldModification *modifications;
    unsigned n_modifications;
    unsigned modification_capacity;
};

void init_world(struct World *world);

void save_world(struct World *world, const char *filepath);
void load_changes_onto_world(struct World *world, const char *filepath, struct TextureAtlas *texture_atlas);

void generate_lobby_world(struct World *world, struct TextureAtlas *texture_atlas);

void generate_ice_world(struct World *world, struct TextureAtlas *texture_atlas, vec3 player_position);

void world_place_block(struct World *world, int x, int y, int z, unsigned texture_id, struct TextureAtlas *texture_atlas);
void world_destroy_block_at_position(struct World *world, int x, int y, int z);

void world_update_cached_chunks(struct World *world, vec3 player_position, unsigned render_distance, struct TextureAtlas *texture_atlas);

void world_free(struct World *world);