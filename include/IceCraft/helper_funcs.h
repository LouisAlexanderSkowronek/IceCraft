#pragma once

char *read_file(const char *filename);

char **split_string(const char *string, char delimiter, int *count);

void free_tokens(char **tokens, int count);

unsigned load_jpg_texture(const char *filename);