#pragma once

#include "block_vertex.h"
#include "point.h"

char *read_file(const char *filename);

char **split_string(const char *string, char delimiter, int *count);

void free_tokens(char **tokens, int count);

unsigned load_jpg_texture(const char *filename);

// Note: vertices is expected to be an array of 36 elements, points to be an array of 8 elements.
void generate_cube_vertices_from_points(struct BlockVertex *vertices, const struct Point *points, float u_min, float u_max, float v_min, float v_max);