#pragma once

char *read_file(const char *filename);

char **split_string(const char *string, char delimiter, int *count);

void free_tokens(char **tokens, int count);

unsigned load_jpg_texture(const char *filename);

unsigned *load_textures(const char *filepath, unsigned *count);

unsigned load_texture_atlas(const char *filepath);