#pragma once

#include "block_vertex.h"

#include <stdlib.h>
#include <stdio.h>

#include "cglm/cglm.h"

struct Chunk;
struct Player;


#define BLOCK_N_VERTICES 36
#define BLOCK_VERTICES_SIZE (BLOCK_N_VERTICES * sizeof(struct BlockVertex))

struct Block
{
    float x, y, z;
    unsigned texture_id;
    struct BlockVertex *vertices;
};

enum BlockFace
{
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    FRONT,
    BACK,
    OTHER_OR_NONE
};

struct Block generate_block(float x, float y, float z, unsigned material_id, struct BlockVertex *vertices);


// Note: Block edge size is assumed to be 1.0f (0.5f in all directions from center).
int point_is_in_block(float px, float py, float pz, struct Block *block);

int player_looks_at_block(vec3 player_pos, vec3 player_looking_direction, struct Block *block);

unsigned *blocks_player_looks_at(vec3 player_pos, vec3 player_looking_direction, struct Chunk *chunk, unsigned *count);

unsigned closest_block_to_player(unsigned *selected_blocks, unsigned n_selected_blocks, struct Chunk *chunk, vec3 player_position);

enum BlockFace player_looks_at_face(struct Player *player, struct Block *block);