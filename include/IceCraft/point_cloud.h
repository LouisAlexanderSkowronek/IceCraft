#pragma once

#include "point_vertex.h"

#define POINT_CLOUD_MAX_POINTS 6

struct PointCloud
{
    struct PointVertex points[POINT_CLOUD_MAX_POINTS];
};

void generate_point_cloud(struct PointCloud *point_cloud, float camera_x, float camera_y, float camera_z)
{
    
}