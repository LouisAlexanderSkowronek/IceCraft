#include "IceCraft/input_handler.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "IceCraft/camera.h"
#include "IceCraft/gui_block_selector.h"

static double remaining_time_block_placement_blocked = 0.0;
static double remaining_time_block_breaking_blocked = 0.0;

void processInput(GLFWwindow *window, struct GUIBlockSelector *hud, struct Camera *camera, struct World *world, int *show_coordinate_axes, int *c_key_is_blocked, const float delta)
{
    remaining_time_block_breaking_blocked -= delta;
    remaining_time_block_placement_blocked -= delta;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !(*c_key_is_blocked))
    {
        *show_coordinate_axes = !(*show_coordinate_axes);
        *c_key_is_blocked = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
    {
        *c_key_is_blocked = 0;
    }

    const unsigned n_items_in_hud = 6;
    for (int i = 0; i < n_items_in_hud; i++)
    {
        if (glfwGetKey(window, GLFW_KEY_1 + i) == GLFW_PRESS)
        {
            hud_select_item(i, hud);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && remaining_time_block_placement_blocked <= 0.0)
    {
        unsigned material_id;
        switch (hud->selected_block_index)
        {
            case 0: material_id = 0; break;
            case 1: material_id = 4; break;
            case 2: material_id = 1; break;
            case 3: material_id = 2; break;
            case 4: material_id = 3; break;
            case 5: material_id = 5; break;
        }

        unsigned count;
        unsigned *selected_blocks = blocks_player_looks_at(camera->position, camera->front, world->chunk, &count);

        if (count)
        {
            unsigned selected_block_idx = closest_block_to_player(selected_blocks, count, world->chunk, camera->position);
            struct Block *selected_block = world->chunk->blocks + selected_block_idx;
            add_block_to_chunk(selected_block->x, selected_block->y + 1, selected_block->z, material_id, world->chunk);
            remaining_time_block_placement_blocked = 0.5;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && remaining_time_block_breaking_blocked <= 0.0)
    {
        unsigned count;
        unsigned *selected_blocks = blocks_player_looks_at(camera->position, camera->front, world->chunk, &count);

        if (count)
        {
            unsigned selected_block = closest_block_to_player(selected_blocks, count, world->chunk, camera->position);
            remove_block_from_chunk(selected_block, world->chunk);
            remaining_time_block_breaking_blocked = 0.5;
        }

        free(selected_blocks);
    }


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        move_camera(camera, (vec3){ camera->front[0] * CAMERA_SPEED * delta, 0.0f, camera->front[2] * CAMERA_SPEED * delta });
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        move_camera(camera, (vec3){ -camera->front[0] * CAMERA_SPEED * delta, 0.0f, -camera->front[2] * CAMERA_SPEED * delta });
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        vec3 right;
        glm_cross(camera->front, camera->up, right);
        glm_normalize(right);
        move_camera(camera, (vec3){ -right[0] * CAMERA_SPEED * delta, 0.0f, -right[2] * CAMERA_SPEED * delta });
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        vec3 right;
        glm_cross(camera->front, camera->up, right);
        glm_normalize(right);
        move_camera(camera, (vec3){ right[0] * CAMERA_SPEED * delta, 0.0f, right[2] * CAMERA_SPEED * delta });
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        move_camera(camera, (vec3){ 0.0f, CAMERA_SPEED * delta, 0.0f });
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        move_camera(camera, (vec3){ 0.0f, -CAMERA_SPEED * delta, 0.0f });
    }

    vec2 camera_rotation_offset = { 0.0f, 0.0f };

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera_rotation_offset[0] += CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camera_rotation_offset[0] -= CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera_rotation_offset[1] -= CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camera_rotation_offset[1] += CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    rotate_camera(camera, camera_rotation_offset);
}
