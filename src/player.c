#include "IceCraft/player.h"

#include "IceCraft/world.h"

void init_player(struct Player *player)
{
    init_camera(&player->camera);
    player->velocity_y = 0.0;
    player->can_jump = 0;
}

void update_player(struct Player *player, struct World *world, double delta)
{    
    player->camera.position[1] += player->velocity_y * delta;
    player->velocity_y -= 9.81 * delta;

    
    if (player->velocity_y > 30.0)
    {
        player->velocity_y = 30.0f;
    }

    if (player->velocity_y < -30.0)
    {
        player->velocity_y = -30.0;
    }

    for (unsigned chunk_idx = 0; chunk_idx < world->n_cached_chunks; chunk_idx++)
    {
        struct Chunk *current_chunk = world->cached_chunks + chunk_idx;
        for (unsigned i = 0; i < current_chunk->placed_blocks; i++)
        {
            if (player->camera.position[0] > current_chunk->blocks[i].x - 0.5f && player->camera.position[0] < current_chunk->blocks[i].x + 0.5f
                && player->camera.position[1] - 1.8f > current_chunk->blocks[i].y - 0.5f && player->camera.position[1] - 1.8f < current_chunk->blocks[i].y + 0.5f
                && player->camera.position[2] > current_chunk->blocks[i].z - 0.5f && player->camera.position[2] < current_chunk->blocks[i].z + 0.5f)
            {
                player->can_jump = 1;
                player->velocity_y = 0.0;
                player->camera.position[1] = current_chunk->blocks[i].y + 2.3f;
            }
        }
    }
}