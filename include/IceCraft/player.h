#pragma once

#include "camera.h"
#include "chunk.h"

struct Player
{
    struct Camera camera;
    double velocity_y;
    int can_jump;
};

void init_player(struct Player *player);

void update_player(struct Player *player, struct Chunk *chunk, double delta);