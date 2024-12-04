#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "chunk.h"

#include <stdio.h>

#define WORLD_N_CHUNKS 5

struct World
{
    struct Chunk *chunk_ptrs[WORLD_N_CHUNKS];
};

void generate_flat_world(struct World *world)
{
    for (unsigned i = 0; i < WORLD_N_CHUNKS; i++)
    {
        world->chunk_ptrs[i] = (struct Chunk*) malloc(sizeof(struct Chunk));
        if (!world->chunk_ptrs[i])
        {
            fprintf(stderr, "Failed to allocate %lu bytes for a chunk at index %d!\n", sizeof(struct Chunk), i);
            exit(1);
        }
    }

    float xs[5] = { 0.0f, 0.0f, 16.0f, 0.0f, -16.0f };
    float zs[5] = { 0.0f, -16.0f, 0.0f, 16.0f, 0.0f };

    for (unsigned i = 0; i < WORLD_N_CHUNKS; i++) {
        init_chunk(xs[i], zs[i], world->chunk_ptrs[i]);

        for (int z = 0; z < 16; z++)
        {
            for (int y = 0; y < 3; y++)
            {
                for (int x = 0; x < 16; x++)
                {
                    add_block_to_chunk(x + xs[i], y, -z - zs[i], 0, world->chunk_ptrs[i]);
                } 
            }
        }

        for (int z = 0; z < 16; z++)
        {
            for (int x = 0; x < 16; x++)
            {
                add_block_to_chunk(x + xs[i], 3, -z - zs[i], 4, world->chunk_ptrs[i]);
            }
        }
    }
}


void update_loaded_chunks(float player_x, float player_z, struct World *world)
{
    for (unsigned i = 0; i < WORLD_N_CHUNKS; i++)
    {
        const float dx = abs(world->chunk_ptrs[i]->x - player_x);
        const float dz = abs(world->chunk_ptrs[i]->z - player_z);

        if (dx > 16 || dz > 16)
        {
            free(world->chunk_ptrs[i]->blocks);
            free(world->chunk_ptrs[i]->vertices);

            glDeleteVertexArrays(1, &world->chunk_ptrs[i]->VAO);
            glDeleteBuffers(1, &world->chunk_ptrs[i]->VBO);
            
            free(world->chunk_ptrs[i]);
            world->chunk_ptrs[i] = NULL;
        }
    }
}

void world_free_chunks(struct World *world)
{
    for (unsigned i = 0; i < WORLD_N_CHUNKS; i++)
    {
        free(world->chunk_ptrs[i]->blocks);
        free(world->chunk_ptrs[i]->vertices);
        free(world->chunk_ptrs[i]);
    }
}