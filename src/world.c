#include "IceCraft/world.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "IceCraft/chunk.h"
#include "IceCraft/texture_atlas.h"

#include <stdio.h>


void init_world(struct World *world)
{
    world->chunk = NULL;
}

void generate_lobby_world(struct World *world, struct TextureAtlas *texture_atlas)
{
    if (world->chunk)
    {
        world_free_chunk(world);
    }

    world->chunk = (struct Chunk*) malloc(sizeof(struct Chunk));
    if (!world->chunk)
    {
        fprintf(stderr, "Failed to allocate %lu bytes for the only chunk!\n", sizeof(struct Chunk));
        exit(1);
    }

    init_chunk(0.0f, 0.0f, world->chunk, CHUNK_BASE_AREA * 5);

    for (int z = 0; z < 16; z++)
    {
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 16; x++)
            {
                add_block_to_chunk(x, y, -z, 0, world->chunk, texture_atlas);
            }
        }
    }

    for (int z = 0; z < 16; z++)
    {
        for (int x = 0; x < 16; x++)
        {
            add_block_to_chunk(x, 3, -z, 4, world->chunk, texture_atlas);
        }
    }
}


void generate_ice_world(struct World *world, struct TextureAtlas *texture_atlas)
{
    if (world->chunk)
    {
        world_free_chunk(world);
    }

    world->chunk = malloc(sizeof(struct Chunk));
    if (!world->chunk)
    {
        fprintf(stderr, "Couldn't allocate chunk for Ice World!\n");
        exit(1);
    }

    init_chunk(0.0f, 0.0f, world->chunk, CHUNK_BASE_AREA * 5);

    for (int z = 0; z < 16; z++)
    {
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 16; x++)
            {
                add_block_to_chunk(x, y, -z, 6, world->chunk, texture_atlas);
            }
        }
    }
}

void world_free_chunk(struct World *world)
{
    free(world->chunk->blocks);
    free(world->chunk->vertices);
    free(world->chunk);
    world->chunk = NULL;
}