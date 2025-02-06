#include "IceCraft/block.h"

#include <math.h>

#include "IceCraft/point.h"
#include "IceCraft/chunk.h"
#include "IceCraft/player.h"
#include "IceCraft/helper_funcs.h"
#include "IceCraft/texture_atlas.h"

inline static void assign_point_to_vertex(const struct Point *p, struct BlockVertex *v)
{
    *v = *((struct BlockVertex*)p);
}


struct Block generate_block(float x, float y, float z, unsigned material_id, struct BlockVertex *vertices, struct TextureBounds *texture_bounds)
{
    struct Block block;
    block.x = x;
    block.y = y;
    block.z = z;
    block.texture_id = material_id;
    block.vertices = vertices;

    /*
       C - - - - B
      /|        /|
     / |       / |
    D -|- - - A  |
    |  |      |  |
    |  G - - -|- F
    | /       | /
    |/        |/
    H - - - - E
    */

    const struct Point A = { +0.5f + x, +0.5f + y, +0.5f + z };
    const struct Point B = { +0.5f + x, +0.5f + y, -0.5f + z };
    const struct Point C = { -0.5f + x, +0.5f + y, -0.5f + z };
    const struct Point D = { -0.5f + x, +0.5f + y, +0.5f + z };
    const struct Point E = { +0.5f + x, -0.5f + y, +0.5f + z };
    const struct Point F = { +0.5f + x, -0.5f + y, -0.5f + z };
    const struct Point G = { -0.5f + x, -0.5f + y, -0.5f + z };
    const struct Point H = { -0.5f + x, -0.5f + y, +0.5f + z };

    struct Point points[8] = { A, B, C, D, E, F, G, H };

    generate_cube_vertices_from_points(vertices, points, texture_bounds->u_min, texture_bounds->u_max, texture_bounds->v_min, texture_bounds->v_max);

    return block;
}


// Note: Block edge size is assumed to be 1.0f (0.5f in all directions from center).
int point_is_in_block(float px, float py, float pz, struct Block *block)
{
    // Actually 1/2 a, but I'm too lazy to type a_half all the time.
    const float a = 0.5f;
    
    const float left = block->x - a;
    const float right = block->x + a;
    const float bottom = block->y - a;
    const float top = block->y + a;
    const float front = block->z + a;
    const float back = block->z - a;

    return px >= left && px <= right && py >= bottom && py <= top && pz >= back && pz <= front;
}

int player_looks_at_block(vec3 player_pos, vec3 player_looking_direction, struct Block *block)
{
    const unsigned n_steps = 24;
    const float step_size = 0.25f;

    float px, py, pz;
    px = player_pos[0];
    py = player_pos[1];
    pz = player_pos[2];

    if (point_is_in_block(px, py, pz, block))
    {
        return 1;
    }

    for (unsigned i = 0; i < n_steps; i++)
    {
        px += player_looking_direction[0] * step_size;
        py += player_looking_direction[1] * step_size;
        pz += player_looking_direction[2] * step_size;

        if (point_is_in_block(px, py, pz, block))
        {
            return 1;
        }
    }

    return 0;
}

unsigned *blocks_player_looks_at(vec3 player_pos, vec3 player_looking_direction, struct Chunk *chunk, unsigned *count)
{
    *count = 0;
    unsigned capacity = 6;
    unsigned *selected_blocks = malloc(capacity * sizeof(unsigned));
    if (!selected_blocks)
    {
        fprintf(stderr, "Memory Error: Couldn't allocate %lu bytes for blocks_player_looks_at function!", capacity * sizeof(unsigned));
        exit(1);
    }

    for (unsigned i = 0; i < chunk->placed_blocks; i++)
    {
        if (player_looks_at_block(player_pos, player_looking_direction, &chunk->blocks[i]))
        {
            if (*count == capacity)
            {
                capacity <<= 1;
                unsigned *tmp = realloc(selected_blocks, capacity * sizeof(unsigned));
                if (!tmp)
                {
                    fprintf(stderr, "Memory Error: Couldn't allocate %lu bytes for blocks_player_looks_at function!", capacity * sizeof(unsigned));
                    exit(1);
                }

                selected_blocks = tmp;
            }

            selected_blocks[(*count)++] = i;
        }
    }

    return selected_blocks;
}


unsigned closest_block_to_player(unsigned *selected_blocks, unsigned n_selected_blocks, struct Chunk *chunk, vec3 player_position)
{
    if (n_selected_blocks == 0)
    {
        fprintf(stderr, "Invalid argument in closest_block_to_player function: n_selected_blocks is zero!");
        exit(1);
    }

    float *distances = malloc(n_selected_blocks * sizeof(float));
    if (!distances)
    {
        fprintf(stderr, "Memory error in closest_block_to_player function!");
        exit(1);
    }

    for (unsigned i = 0; i < n_selected_blocks; i++)
    {
        unsigned block_idx = selected_blocks[i];
        struct Block *block = chunk->blocks + block_idx;
        vec3 block_position = { block->x, block->y, block->z };
        const float distance = glm_vec3_distance(player_position, block_position);
        distances[i] = distance;
    }

    unsigned idx_min_distance = selected_blocks[0];
    float min_distance = distances[0];

    for (unsigned i = 1; i < n_selected_blocks; i++)
    {
        if (distances[i] < min_distance)
        {
            idx_min_distance = selected_blocks[i];
            min_distance = distances[i];
        }
    }

    free(distances);

    return idx_min_distance;
}


enum BlockFace player_looks_at_face(struct Player *player, struct Block *block)
{
    const float ELx = block->x - 0.5f;
    const float ELy_min = block->y - 0.5f;
    const float ELy_max = block->y + 0.5f;
    const float ELz_min = block->z - 0.5f;
    const float ELz_max = block->z + 0.5f;

    const float ERx = block->x + 0.5f;
    const float ERy_min = block->y - 0.5f;
    const float ERy_max = block->y + 0.5f;
    const float ERz_min = block->z - 0.5f;
    const float ERz_max = block->z + 0.5f;

    const float ETx_min = block->x - 0.5f;
    const float ETx_max = block->x + 0.5f;
    const float ETy = block->y + 0.5f;
    const float ETz_min = block->z - 0.5f;
    const float ETz_max = block->z + 0.5f;

    const float EBx_min = block->x - 0.5f;
    const float EBx_max = block->x + 0.5f;
    const float EBy = block->y - 0.5f;
    const float EBz_min = block->z - 0.5f;
    const float EBz_max = block->z + 0.5f;

    const float EFx_min = block->x - 0.5f;
    const float EFx_max = block->x + 0.5f;
    const float EFy_min = block->y - 0.5f;
    const float EFy_max = block->y + 0.5f;
    const float EFz = block->z + 0.5f;

    const float EBCKx_min = block->x - 0.5f;
    const float EBCKx_max = block->x + 0.5f;
    const float EBCKy_min = block->y - 0.5f;
    const float EBCKy_max = block->y + 0.5f;
    const float EBCKz = block->z - 0.5f;

    float ts[6];  // Left, Right, Top, Bottom, Front, Back
    ts[0] = (ELx - player->camera.position[0]) / player->camera.front[0];
    ts[1] = (ERx - player->camera.position[0]) / player->camera.front[0];
    ts[2] = (ETy - player->camera.position[1]) / player->camera.front[1];
    ts[3] = (EBy - player->camera.position[1]) / player->camera.front[1];
    ts[4] = (EFz - player->camera.position[2]) / player->camera.front[2];
    ts[5] = (EBCKz - player->camera.position[2]) / player->camera.front[2];

    const float potential_hit_Lx = player->camera.position[0] + ts[0] * player->camera.front[0];
    const float potential_hit_Ly = player->camera.position[1] + ts[0] * player->camera.front[1];
    const float potential_hit_Lz = player->camera.position[2] + ts[0] * player->camera.front[2];

    const float potential_hit_Rx = player->camera.position[0] + ts[1] * player->camera.front[0];
    const float potential_hit_Ry = player->camera.position[1] + ts[1] * player->camera.front[1];
    const float potential_hit_Rz = player->camera.position[2] + ts[1] * player->camera.front[2];

    const float potential_hit_Tx = player->camera.position[0] + ts[2] * player->camera.front[0];
    const float potential_hit_Ty = player->camera.position[1] + ts[2] * player->camera.front[1];
    const float potential_hit_Tz = player->camera.position[2] + ts[2] * player->camera.front[2];

    const float potential_hit_Bx = player->camera.position[0] + ts[3] * player->camera.front[0];
    const float potential_hit_By = player->camera.position[1] + ts[3] * player->camera.front[1];
    const float potential_hit_Bz = player->camera.position[2] + ts[3] * player->camera.front[2];

    const float potential_hit_Fx = player->camera.position[0] + ts[4] * player->camera.front[0];
    const float potential_hit_Fy = player->camera.position[1] + ts[4] * player->camera.front[1];
    const float potential_hit_Fz = player->camera.position[2] + ts[4] * player->camera.front[2];

    const float potential_hit_BCKx = player->camera.position[0] + ts[5] * player->camera.front[0];
    const float potential_hit_BCKy = player->camera.position[1] + ts[5] * player->camera.front[1];
    const float potential_hit_BCKz = player->camera.position[2] + ts[5] * player->camera.front[2];

    const int bl = potential_hit_Ly >= ELy_min && potential_hit_Ly <= ELy_max
        && potential_hit_Lz >= ELz_min && potential_hit_Lz <= ELz_max;
    
    const int br = potential_hit_Ry >= ERy_min && potential_hit_Ry <= ERy_max
        && potential_hit_Rz >= ERz_min && potential_hit_Rz <= ERz_max;

    const int bt = potential_hit_Tx >= ETx_min && potential_hit_Tx <= ETx_max
        && potential_hit_Tz >= ETz_min && potential_hit_Tz <= ETz_max;
    
    const int bb = potential_hit_Bx >= EBx_min && potential_hit_Bx <= EBx_max
        && potential_hit_Bz >= EBz_min && potential_hit_Bz <= EBz_max;

    const int bf = potential_hit_Fx >= EFx_min && potential_hit_Fx <= EFx_max
        && potential_hit_Fy >= EFy_min && potential_hit_Fy <= EFy_max;

    const int bbck = potential_hit_BCKx >= EBCKx_min && potential_hit_BCKx <= EBCKx_max
        && potential_hit_BCKy >= EBCKy_min && potential_hit_BCKy <= EBCKy_max;


    unsigned n_faces_at_once = 0;
    if (bl) { n_faces_at_once++; }
    if (br) { n_faces_at_once++; }
    if (bt) { n_faces_at_once++; }
    if (bb) { n_faces_at_once++; }
    if (bf) { n_faces_at_once++; }
    if (bbck) { n_faces_at_once++; }

    if (n_faces_at_once > 2)
    {
        fprintf(stderr, "%u faces at once!\n", n_faces_at_once);
        exit(1);
    }

    if (bl && br)
    {
        if (ts[0] < ts[1])
        {
            return LEFT;
        }

        return RIGHT;
    }

    if (bl && bt)
    {
        if (ts[0] < ts[2])
        {
            return LEFT;
        }

        return TOP;
    }

    if (bl && bb)
    {
        if (ts[0] < ts[3])
        {
            return LEFT;
        }

        return BOTTOM;
    }

    if (bl && bf)
    {
        if (ts[0] < ts[4])
        {
            return LEFT;
        }

        return FRONT;
    }

    if (bl && bbck)
    {
        if (ts[0] < ts[5])
        {
            return LEFT;
        }

        return BACK;
    }

    if (br && bt)
    {
        if (ts[1] < ts[2])
        {
            return RIGHT;
        }

        return TOP;
    }

    if (br && bb)
    {
        if (ts[1] < ts[3])
        {
            return RIGHT;
        }

        return BOTTOM;
    }

    if (br && bf)
    {
        if (ts[1] < ts[4])
        {
            return RIGHT;
        }

        return FRONT;
    }

    if (br && bbck)
    {
        if (ts[1] < ts[5])
        {
            return RIGHT;
        }

        return BACK;
    }

    if (bt && bb)
    {
        if (ts[2] < ts[3])
        {
            return TOP;
        }

        return BOTTOM;
    }

    if (bt && bf)
    {
        if (ts[2] < ts[4])
        {
            return TOP;
        }

        return FRONT;
    }

    if (bt && bbck)
    {
        if (ts[2] < ts[5])
        {
            return TOP;
        }

        return BACK;
    }

    if (bf && bbck)
    {
        if (ts[4] < ts[5])
        {
            return FRONT;
        }

        return BACK;
    }

    if (bt)
    {
        return TOP;
    }

    if (bl)
    {
        return LEFT;
    }

    if (br)
    {
        return RIGHT;
    }

    if (bb)
    {
        return BOTTOM;
    }

    if (bf)
    {
        return FRONT;
    }

    if (bbck)
    {
        return BACK;
    }
    
    return OTHER_OR_NONE;
}