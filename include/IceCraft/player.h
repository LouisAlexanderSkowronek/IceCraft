#pragma once

#include "camera.h"
#include "chunk.h"
#include "world.h"

struct Player
{
    struct Camera camera;
    double velocity_y;
    int can_jump;
};

void init_player(struct Player *player);

void update_player(struct Player *player, struct World *world, double delta);