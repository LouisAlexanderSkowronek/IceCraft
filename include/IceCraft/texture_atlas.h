#pragma once

struct TextureBounds
{
    float u_min, u_max;
    float v_min, v_max;
};

struct TextureAtlas
{
    struct TextureBounds *texture_bounds;
    unsigned *indices;
    unsigned n_textures;
    unsigned capacity;
    unsigned texture_id;
};

void init_empty_texture_atlas(struct TextureAtlas *texture_atlas, const char *filepath);

void texture_atlas_add_texture(struct TextureAtlas *texture_atlas, float u_min, float v_min, float u_max, float v_max);

void texture_atlas_get_texture_uv(struct TextureAtlas *texture_atlas, unsigned index, struct TextureBounds *texture_bounds_out);

void free_texture_atlas(struct TextureAtlas *texture_atlas);