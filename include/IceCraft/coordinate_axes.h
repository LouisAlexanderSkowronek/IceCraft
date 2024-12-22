#pragma once

#include "line_vertex.h"

#define COORDINATE_AXES_N_VERTICES 6
#define COORDINATE_AXES_NVERTICES_SIZE (COORDINATE_AXES_N_VERTICES * sizeof(struct LineVertex))

struct CoordinateAxes
{
    struct LineVertex vertices[COORDINATE_AXES_N_VERTICES];
    unsigned VAO, VBO;
};

void generate_coordinate_axes(struct CoordinateAxes *coord_axes);

void coord_axes_update_vao_and_vbo(struct CoordinateAxes *coord_axes);