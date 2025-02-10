#include "IceCraft/world.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "IceCraft/chunk.h"
#include "IceCraft/texture_atlas.h"
#include "IceCraft/helper_funcs.h"

#include <stdio.h>
#include <string.h>


void init_world(struct World *world)
{
    world->cached_chunks = NULL;
    world->n_cached_chunks = 0;
    world->modification_capacity = 64;
    world->n_modifications = 0;
    world->modifications = malloc(world->modification_capacity * sizeof(struct WorldModification));
    if (!world->modifications)
    {
        fprintf(stderr, "Couldn't allocate capacity for world modifications\n");
        exit(1);
    }
}


void save_world(struct World *world, const char *filepath)
{
    unsigned n_characters_written = 0;
    unsigned capacity = 1024;
    char *saved_string = malloc(capacity);
    if (!saved_string)
    {
        fprintf(stderr, "Couldn't allocate initial buffer of capacity %u characters/bytes for saving world\n", capacity);
        exit(1);
    }

    saved_string[0] = '\0';


    for (unsigned i = 0; i < world->n_modifications; i++)
    {
        char modification_buffer[1024];
        struct WorldModification *modification = world->modifications + i;
        if (modification->type == PLACED)
        {
            sprintf(
                modification_buffer,
                "place %d, %d, %d, %u\n",
                (int) modification->x,
                (int) modification->y,
                (int) modification->z,
                modification->material_id
            );
        } else if (modification->type == DESTROYED)
        {
            sprintf(modification_buffer, "destroy %d, %d, %d\n", (int) modification->x, (int) modification->y, (int) modification->z);
        }

        const unsigned length = strlen(modification_buffer);
        if (n_characters_written + length + 1 > capacity)  // Include null-terminator
        {
            capacity *= 2;
            char *tmp = realloc(saved_string, capacity);
            if (!tmp)
            {
                fprintf(stderr, "Couldn't double capacity to %u bytes/characters when saving world\n", capacity);
                exit(1);
            }

            saved_string = tmp;
        }

        strcpy(saved_string + n_characters_written, modification_buffer);

        n_characters_written += length;
    }

    FILE *file = fopen(filepath, "w");
    if (!file)
    {
        fprintf(stderr, "Couldn't open file %s\n", filepath);
        exit(1);
    }

    if (fwrite(saved_string, 1, n_characters_written, file) != n_characters_written)
    {
        fprintf(stderr, "Failed to save world in a file, so I print it here:\n%s\n", saved_string);
        fclose(file);
        exit(1);
    }

    fclose(file);
}


void load_changes_onto_world(struct World *world, const char *filepath, struct TextureAtlas *texture_atlas)
{
    char *file_content = read_file(filepath);

    if (strlen(file_content) == 0)
    {
        return;
    }

    unsigned count;
    char **commands_to_reconstruct = split_string(file_content, '\n', &count);

    for (unsigned i = 0; i < count; i++)
    {
        if (commands_to_reconstruct[i][0] == 'p')  // Assume command to be 'place'
        {
            int x, y, z;
            unsigned material_id;
            commands_to_reconstruct[i] += strlen("place ");
            unsigned inner_count;
            char **parameters = split_string(commands_to_reconstruct[i], ',', &inner_count);
            x = atoi(parameters[0]);
            y = atoi(parameters[1]);
            z = atoi(parameters[2]);
            material_id = strtoul(parameters[3], NULL, 10);
            free_tokens(parameters, inner_count);
            world_place_block(world, x, y, z, material_id, texture_atlas);
            commands_to_reconstruct[i] -= strlen("place ");
        } else if (commands_to_reconstruct[i][0] == 'd')  // Assume command to be 'destroy'
        {
            int x, y, z;
            commands_to_reconstruct[i] += strlen("destroy ");
            unsigned inner_count;
            char **parameters = split_string(commands_to_reconstruct[i], ',', &inner_count);
            x = atoi(parameters[0]);
            y = atoi(parameters[1]);
            z = atoi(parameters[2]);
            free_tokens(parameters, inner_count);
            world_destroy_block_at_position(world, x, y, z);
            commands_to_reconstruct[i] -= strlen("destroy ");
        }
    }

    free_tokens(commands_to_reconstruct, count);
    free(file_content);
}


void generate_lobby_world(struct World *world, struct TextureAtlas *texture_atlas)
{
    if (world->cached_chunks)
    {
        world_free(world);
    }

    world->n_cached_chunks = 1;
    world->cached_chunks = malloc(world->n_cached_chunks * sizeof(struct Chunk));
    if (!world->cached_chunks)
    {
        fprintf(stderr, "Failed to allocate %lu bytes for the only chunk!\n", sizeof(struct Chunk));
        exit(1);
    }

    init_chunk(0, 0, world->cached_chunks, CHUNK_BASE_AREA * 5);

    for (int z = 0; z < 16; z++)
    {
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 16; x++)
            {
                add_block_to_chunk(x, y, -z, 0, world->cached_chunks, texture_atlas);
            }
        }
    }

    for (int z = 0; z < 16; z++)
    {
        for (int x = 0; x < 16; x++)
        {
            add_block_to_chunk(x, 3, -z, 4, world->cached_chunks, texture_atlas);
        }
    }
}


void generate_ice_world(struct World *world, struct TextureAtlas *texture_atlas)
{
    if (world->cached_chunks)
    {
        world_free(world);
    }

    world->n_cached_chunks = 2;
    world->cached_chunks = malloc(world->n_cached_chunks * sizeof(struct Chunk));
    if (!world->cached_chunks)
    {
        fprintf(stderr, "Couldn't allocate %u chunks for Ice World!\n", world->n_cached_chunks);
        exit(1);
    }

    for (unsigned i = 0; i < world->n_cached_chunks; i++)
    {
        init_chunk(16*i, 0, world->cached_chunks+i, CHUNK_BASE_AREA * 5);
    }

    for (int z = 0; z < 16; z++)
    {
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 16 * world->n_cached_chunks; x++)
            {
                add_block_to_chunk(x, y, -z, 6, world->cached_chunks + x/16, texture_atlas);
            }
        }
    }
}


void world_place_block(struct World *world, int x, int y, int z, unsigned texture_id, struct TextureAtlas *texture_atlas)
{
    if (world->n_modifications == world->modification_capacity)
    {
        world->modification_capacity *= 2;
        struct WorldModification *tmp = realloc(world->modifications, world->modification_capacity * sizeof(struct WorldModification));
        if (!tmp)
        {
            fprintf(stderr, "Couldn't double world modification capacity to %u\n", world->modification_capacity);
            free(world->modifications);
            exit(1);
        }

        world->modifications = tmp;
    }

    world->modifications[world->n_modifications].type = PLACED;
    world->modifications[world->n_modifications].material_id = texture_id;
    world->modifications[world->n_modifications].x = x;
    world->modifications[world->n_modifications].y = y;
    world->modifications[world->n_modifications].z = z;

    world->n_modifications++;

    add_block_to_chunk(x, y, z, texture_id, world->cached_chunks + x/16, texture_atlas);
}


void world_destroy_block_at_position(struct World *world, int x, int y, int z)
{
    if (world->n_modifications == world->modification_capacity)
    {
        world->modification_capacity *= 2;
        struct WorldModification *tmp = realloc(world->modifications, world->modification_capacity * sizeof(struct WorldModification));
        if (!tmp)
        {
            fprintf(stderr, "Couldn't double world modification capacity to %u\n", world->modification_capacity);
            free(world->modifications);
            exit(1);
        }

        world->modifications = tmp;
    }

    world->modifications[world->n_modifications].type = DESTROYED;
    world->modifications[world->n_modifications].x = x;
    world->modifications[world->n_modifications].y = y;
    world->modifications[world->n_modifications].z = z;

    world->n_modifications++;

    for (unsigned i = 0; i < (world->cached_chunks + x/16)->placed_blocks; i++)
    {
        struct Block *block = (world->cached_chunks + x/16)->blocks + i;
        if (block->x == x && block->y == y && block->z == z)
        {
            remove_block_from_chunk(i, world->cached_chunks + x/16);
        }
    }
}


void world_free(struct World *world)
{
    for (unsigned i = 0; i < world->n_cached_chunks; i++)
    {
        free((world->cached_chunks+i)->blocks);
        free((world->cached_chunks+i)->vertices);
    }

    free(world->cached_chunks);
    free(world->modifications);

    world->cached_chunks = NULL;
    world->modifications = NULL;
    world->n_cached_chunks = 0;
    world->n_modifications = 0;
    world->modification_capacity = 0;
}