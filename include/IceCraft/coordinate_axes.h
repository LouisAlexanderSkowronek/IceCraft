#pragma once

#include "line_vertex.h"


#define COORDINATE_AXES_N_VERTICES 6
#define COORDINATE_AXES_NVERTICES_SIZE (COORDINATE_AXES_N_VERTICES * sizeof(struct LineVertex))


struct CoordinateAxes
{
    struct LineVertex vertices[COORDINATE_AXES_N_VERTICES];
};


void generateCoordinateAxes(struct CoordinateAxes *coord_axes)
{
    for (unsigned i = 0; i < COORDINATE_AXES_N_VERTICES; i+=2)
    {
        coord_axes->vertices[i].x = coord_axes->vertices[i].y = coord_axes->vertices[i].z = 0.0f;
        coord_axes->vertices[i].r = (i == 0) ? 1.0f : 0.0f;
        coord_axes->vertices[i].g = (i == 2) ? 1.0f : 0.0f;
        coord_axes->vertices[i].b = (i == 4) ? 1.0f : 0.0f;
    }

    for (unsigned i = 1; i < COORDINATE_AXES_N_VERTICES; i+=2)
    {
        coord_axes->vertices[i].x = (i == 1) ? 1024.0f : 0.0f;
        coord_axes->vertices[i].y = (i == 3) ? 1024.0f : 0.0f;
        coord_axes->vertices[i].z = (i == 5) ? 1024.0f : 0.0f;
        coord_axes->vertices[i].r = (i == 1) ? 1.0f : 0.0f;
        coord_axes->vertices[i].g = (i == 3) ? 1.0f : 0.0f;
        coord_axes->vertices[i].b = (i == 5) ? 1.0f : 0.0f;
    }
}