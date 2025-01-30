#include "IceCraft/input_handler.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "IceCraft/player.h"
#include "IceCraft/gui_block_selector.h"
#include "IceCraft/ice_craft.h"

#define BUFFER_SIZE 1024

static double remaining_time_block_placement_blocked = 0.0;
static double remaining_time_block_breaking_blocked = 0.0;
static double remaining_time_jumping_blocked = 0.0;

extern int gravity_enabled;

void process_input(struct IceCraft *ice_craft, const float delta)
{
    remaining_time_block_breaking_blocked -= delta;
    remaining_time_block_placement_blocked -= delta;
    remaining_time_jumping_blocked -= delta;

    if (gravity_enabled && remaining_time_jumping_blocked < 0.0)
    {
        ice_craft->space_key_is_blocked = 0;
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(ice_craft->window, GL_TRUE);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_C) == GLFW_PRESS && !(ice_craft->c_key_is_blocked))
    {
        ice_craft->show_coordinate_axes = !(ice_craft->show_coordinate_axes);
        ice_craft->c_key_is_blocked = 1;
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_C) == GLFW_RELEASE)
    {
        ice_craft->c_key_is_blocked = 0;
    }

    const unsigned n_items_in_hud = 6;
    for (int i = 0; i < n_items_in_hud; i++)
    {
        if (glfwGetKey(ice_craft->window, GLFW_KEY_1 + i) == GLFW_PRESS)
        {
            hud_select_item(i, &ice_craft->block_selector);
        }
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_P) == GLFW_PRESS && remaining_time_block_placement_blocked <= 0.0)
    {
        unsigned material_id;
        switch (ice_craft->block_selector.selected_block_index)
        {
            case 0: material_id = 0; break;
            case 1: material_id = 4; break;
            case 2: material_id = 1; break;
            case 3: material_id = 2; break;
            case 4: material_id = 3; break;
            case 5: material_id = 5; break;
        }

        unsigned count;
        unsigned *selected_blocks = blocks_player_looks_at(ice_craft->louis.camera.position, ice_craft->louis.camera.front, ice_craft->world.chunk, &count);

        if (count)
        {
            unsigned selected_block_idx = closest_block_to_player(selected_blocks, count, ice_craft->world.chunk, ice_craft->louis.camera.position);
            struct Block *selected_block = ice_craft->world.chunk->blocks + selected_block_idx;
            const enum BlockFace face = player_looks_at_face(&ice_craft->louis, selected_block);

            float new_x = selected_block->x, new_y = selected_block->y, new_z = selected_block->z;
            switch (face)
            {
                case LEFT:
                    new_x += -1.0f;
                    break;

                case RIGHT:
                    new_x += +1.0f;
                    break;
                               
                case TOP:
                    new_y += +1.0f;
                    break;

                case BOTTOM:
                    new_y += -1.0f;
                    break;

                case FRONT:
                    new_z += +1.0f;
                    break;

                case BACK:
                    new_z += -1.0f;
                    break;
                
                default: break;
            }
                
            remaining_time_block_placement_blocked = 0.5;

            if (new_x >= 0.0f && new_x <= 15.0f && new_z <= 0.0f && new_z >= -15.0f)
            {
                add_block_to_chunk(new_x, new_y, new_z, material_id, ice_craft->world.chunk);
            }
        }
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_Y) == GLFW_PRESS && remaining_time_block_breaking_blocked <= 0.0)
    {
        unsigned count;
        unsigned *selected_blocks = blocks_player_looks_at(ice_craft->louis.camera.position, ice_craft->louis.camera.front, ice_craft->world.chunk, &count);

        if (count)
        {
            unsigned selected_block = closest_block_to_player(selected_blocks, count, ice_craft->world.chunk, ice_craft->louis.camera.position);
            remove_block_from_chunk(selected_block, ice_craft->world.chunk);
            remaining_time_block_breaking_blocked = 0.5;
        }

        free(selected_blocks);
    }


    if (glfwGetKey(ice_craft->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        move_camera(
            &ice_craft->louis.camera,
            (vec3){ ice_craft->louis.camera.front[0] * CAMERA_SPEED * delta, 0.0f, ice_craft->louis.camera.front[2] * CAMERA_SPEED * delta }
        );
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        move_camera(
            &ice_craft->louis.camera,
            (vec3){ -ice_craft->louis.camera.front[0] * CAMERA_SPEED * delta, 0.0f, -ice_craft->louis.camera.front[2] * CAMERA_SPEED * delta }
        );
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        vec3 right;
        glm_cross(ice_craft->louis.camera.front, ice_craft->louis.camera.up, right);
        glm_normalize(right);
        move_camera(&ice_craft->louis.camera, (vec3){ -right[0] * CAMERA_SPEED * delta, 0.0f, -right[2] * CAMERA_SPEED * delta });
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        vec3 right;
        glm_cross(ice_craft->louis.camera.front, ice_craft->louis.camera.up, right);
        glm_normalize(right);
        move_camera(&ice_craft->louis.camera, (vec3){ right[0] * CAMERA_SPEED * delta, 0.0f, right[2] * CAMERA_SPEED * delta });
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_SPACE) == GLFW_PRESS && (!gravity_enabled || (!ice_craft->space_key_is_blocked && ice_craft->louis.can_jump)))
    {
        move_camera(&ice_craft->louis.camera, (vec3){ 0.0f, CAMERA_SPEED * delta, 0.0f });
        ice_craft->louis.velocity_y += 5.0f;
        ice_craft->louis.camera.position[1] += 0.001f;
        ice_craft->louis.can_jump = 0;
        ice_craft->space_key_is_blocked = 1;
        remaining_time_jumping_blocked = 0.5;
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        move_camera(&ice_craft->louis.camera, (vec3){ 0.0f, -CAMERA_SPEED * delta, 0.0f });
    }

    vec2 camera_rotation_offset = { 0.0f, 0.0f };

    if (glfwGetKey(ice_craft->window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera_rotation_offset[0] += CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camera_rotation_offset[0] -= CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera_rotation_offset[1] -= CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camera_rotation_offset[1] += CAMERA_SENSITIVITY * CAMERA_SPEED * delta;
    }

    rotate_camera(&ice_craft->louis.camera, camera_rotation_offset);
}
