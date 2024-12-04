#pragma once

#include "line_vertex.h"

#define COORDINATE_AXES_N_VERTICES 6
#define COORDINATE_AXES_NVERTICES_SIZE (COORDINATE_AXES_N_VERTICES * sizeof(struct LineVertex))

struct CoordinateAxes
{
    struct LineVertex vertices[COORDINATE_AXES_N_VERTICES];
};

void generate_coordinate_axes(struct CoordinateAxes *coord_axes);

void generate_coord_axes_vao_and_vbo(unsigned *VAO_ptr, unsigned *VBO_ptr, struct CoordinateAxes *coord_axes);