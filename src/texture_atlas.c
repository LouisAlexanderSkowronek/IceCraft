#include "IceCraft/texture_atlas.h"

#include "IceCraft/helper_funcs.h"

#include <stdlib.h>
#include <stdio.h>

void init_empty_texture_atlas(struct TextureAtlas *texture_atlas, const char *filepath)
{
    texture_atlas->capacity = 8;
    texture_atlas->n_textures = 0;
    texture_atlas->texture_id = load_jpg_texture(filepath);
    texture_atlas->texture_bounds = malloc(texture_atlas->capacity * sizeof(*texture_atlas->texture_bounds));
    texture_atlas->indices = malloc(texture_atlas->capacity * sizeof(*texture_atlas->indices));

    if (!texture_atlas->texture_bounds)
    {
        fprintf(stderr, "Failed to init empty texture atlas (malloc for texture_bounds returned NULL)!\n");
        exit(1);
    }
    
    if (!texture_atlas->indices)
    {
        fprintf(stderr, "Failed to init empty texture atlas (malloc for indices returned NULL)!\n");
        exit(1);
    }
}

void texture_atlas_add_texture(struct TextureAtlas *texture_atlas, float u_min, float v_min, float u_max, float v_max)
{
    if (texture_atlas->n_textures == texture_atlas->capacity)
    {
        printf("Doubling capacity ...\n");
        texture_atlas->capacity *= 2;
        struct TextureBounds *tmp = realloc(texture_atlas->texture_bounds, texture_atlas->capacity);
        if (!tmp)
        {
            fprintf(
                stderr,
                "Failed to add texture to texture atlas with area between uv-coordinates (%f | %f) and (%f | %f): "
                "Malloc for doubling capacity of texture_bounds to %u returned NULL!\n",
                u_min,
                v_min,
                u_max,
                v_max,
                texture_atlas->capacity
            );
            free(texture_atlas->texture_bounds);
            exit(1);
        }

        texture_atlas->texture_bounds = tmp;

        unsigned *tmp_indices = realloc(texture_atlas->indices, texture_atlas->capacity);
        if (!tmp)
        {
            fprintf(
                stderr,
                "Failed to add texture to texture atlas with area between uv-coordinates (%f | %f) and (%f | %f): "
                "Malloc for doubling capacity of indices to %u returned NULL!\n",
                u_min,
                v_min,
                u_max,
                v_max,
                texture_atlas->capacity
            );
            free(texture_atlas->indices);
            exit(1);
        }

        texture_atlas->indices = tmp_indices;
    }

    texture_atlas->texture_bounds[texture_atlas->n_textures].u_min = u_min;
    texture_atlas->texture_bounds[texture_atlas->n_textures].u_max = u_max;
    texture_atlas->texture_bounds[texture_atlas->n_textures].v_min = v_min;
    texture_atlas->texture_bounds[texture_atlas->n_textures].v_max = v_max;
    texture_atlas->indices[texture_atlas->n_textures] = texture_atlas->n_textures;

    texture_atlas->n_textures++;
}

void texture_atlas_get_texture_uv(struct TextureAtlas *texture_atlas, unsigned index, struct TextureBounds *texture_bounds_out)
{
    if (index >= texture_atlas->n_textures)
    {
        fprintf(stderr, "Texture Atlas has only %u textures but get_texture was called with index %u\n", texture_atlas->n_textures, index);
    }
    texture_bounds_out->u_min = texture_atlas->texture_bounds[index].u_min;
    texture_bounds_out->v_min = texture_atlas->texture_bounds[index].v_min;
    texture_bounds_out->u_max = texture_atlas->texture_bounds[index].u_max;
    texture_bounds_out->v_max = texture_atlas->texture_bounds[index].v_max;
}

void free_texture_atlas(struct TextureAtlas *texture_atlas)
{
    free(texture_atlas->texture_bounds);
    free(texture_atlas->indices);
}