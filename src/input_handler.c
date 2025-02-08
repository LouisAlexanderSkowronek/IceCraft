#include "IceCraft/input_handler.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "IceCraft/player.h"
#include "IceCraft/gui_block_selector.h"
#include "IceCraft/ice_craft.h"

void process_input(struct IceCraft *ice_craft, float delta)
{
    if (glfwGetKey(ice_craft->window, GLFW_KEY_I) == GLFW_PRESS)
    {
        ice_craft->player_is_in_world = ICE_WORLD;
        save_world(&ice_craft->world, "../assets/worlds/lobby.s");
        world_free_chunk(&ice_craft->world);
        free(ice_craft->world.modifications);
        init_world(&ice_craft->world);
        generate_ice_world(&ice_craft->world, &ice_craft->texture_atlas);
        load_changes_onto_world(&ice_craft->world, "../assets/worlds/ice_world.s", &ice_craft->texture_atlas);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_L) == GLFW_PRESS)
    {
        ice_craft->player_is_in_world = LOBBY;
        save_world(&ice_craft->world, "../assets/worlds/ice_world.s");
        world_free_chunk(&ice_craft->world);
        free(ice_craft->world.modifications);
        init_world(&ice_craft->world);
        generate_lobby_world(&ice_craft->world, &ice_craft->texture_atlas);
        load_changes_onto_world(&ice_craft->world, "../assets/worlds/lobby.s", &ice_craft->texture_atlas);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(ice_craft->window, GL_TRUE);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_C) == GLFW_PRESS)
    {
        handle_key_press_toggle_show_coordinate_system(ice_craft);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_C) == GLFW_RELEASE)
    {
        handle_key_release_toggle_show_coordinate_system(ice_craft);
    }

    check_for_and_handle_selecting_item(ice_craft);

    if (glfwGetKey(ice_craft->window, GLFW_KEY_P) == GLFW_PRESS)
    {
        handle_key_place(ice_craft);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        handle_key_destroy(ice_craft);
    }


    if (glfwGetKey(ice_craft->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        handle_key_move_forward(ice_craft, delta);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        handle_key_move_backward(ice_craft, delta);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        handle_key_move_left(ice_craft, delta);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        handle_key_move_right(ice_craft, delta);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        handle_key_jump(ice_craft, delta);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        handle_key_float_downward(ice_craft, delta);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        handle_key_look_up(ice_craft, delta);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        handle_key_look_down(ice_craft, delta);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        handle_key_look_left(ice_craft, delta);
    }

    if (glfwGetKey(ice_craft->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        handle_key_look_right(ice_craft, delta);
    }

}


void handle_key_press_toggle_show_coordinate_system(struct IceCraft *ice_craft)
{
    if (ice_craft->c_key_is_blocked)
    {
        return;
    }

    ice_craft->show_coordinate_axes = !(ice_craft->show_coordinate_axes);
    ice_craft->c_key_is_blocked = 1;
}


void handle_key_release_toggle_show_coordinate_system(struct IceCraft *ice_craft)
{
    ice_craft->c_key_is_blocked = 0;
}


void check_for_and_handle_selecting_item(struct IceCraft *ice_craft)
{
    for (int i = 0; i < HUD_N_ITEM_SLOTS; i++)
    {
        if (glfwGetKey(ice_craft->window, GLFW_KEY_1 + i) == GLFW_PRESS)
        {
            hud_select_item(i, &ice_craft->block_selector);
        }
    }
}


void handle_key_place(struct IceCraft *ice_craft)
{
    if (ice_craft->remaining_time_block_placement_blocked > 0.0)
    {
        return;
    }

    unsigned material_id;
    
    switch (ice_craft->block_selector.selected_block_index)
    {
        case 0: material_id = 0; break;
        case 1: material_id = 4; break;
        case 2: material_id = 1; break;
        case 3: material_id = 2; break;
        case 4: material_id = 3; break;
        case 5: material_id = 5; break;
        case 6: material_id = 6; break;
    }

    unsigned count;
    unsigned *selected_blocks = blocks_player_looks_at(ice_craft->louis.camera.position, ice_craft->louis.camera.front, ice_craft->world.chunk, &count);

    if (!count)
    {
        return;
    }

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
                
    ice_craft->remaining_time_block_placement_blocked = 0.5;

    if (new_x >= 0.0f && new_x <= 15.0f && new_z <= 0.0f && new_z >= -15.0f)
    {
        world_place_block(&ice_craft->world, new_x, new_y, new_z, material_id, &ice_craft->texture_atlas);
    }
}


void handle_key_destroy(struct IceCraft *ice_craft)
{
    if (ice_craft->remaining_time_block_breaking_blocked > 0.0)
    {
        return;
    }

    unsigned count;
    unsigned *selected_blocks = blocks_player_looks_at(ice_craft->louis.camera.position, ice_craft->louis.camera.front, ice_craft->world.chunk, &count);

    if (count)
    {
        unsigned selected_block = closest_block_to_player(selected_blocks, count, ice_craft->world.chunk, ice_craft->louis.camera.position);
        struct Block *block = ice_craft->world.chunk->blocks + selected_block;
        world_destroy_block_at_position(&ice_craft->world, block->x, block->y, block->z);
        ice_craft->remaining_time_block_breaking_blocked = 0.5;
    }

    free(selected_blocks);
}


void handle_key_move_forward(struct IceCraft *ice_craft, float delta)
{
    vec3 moving_direction;
    glm_vec3_copy(ice_craft->louis.camera.front, moving_direction);
    moving_direction[1] = 0.0f;
    glm_vec3_normalize(moving_direction);
    glm_vec3_scale(moving_direction, CAMERA_SPEED * delta, moving_direction);

    move_camera(&ice_craft->louis.camera, moving_direction);
}


void handle_key_move_backward(struct IceCraft *ice_craft, float delta)
{
    vec3 moving_direction;
    glm_vec3_copy(ice_craft->louis.camera.front, moving_direction);
    moving_direction[1] = 0.0f;
    glm_vec3_negate(moving_direction);
    glm_vec3_normalize(moving_direction);
    glm_vec3_scale(moving_direction, CAMERA_SPEED * delta, moving_direction);
    move_camera(&ice_craft->louis.camera, moving_direction);
}


void handle_key_move_left(struct IceCraft *ice_craft, float delta)
{
    vec3 right;
    glm_cross(ice_craft->louis.camera.front, ice_craft->louis.camera.up, right);
    glm_normalize(right);
    move_camera(&ice_craft->louis.camera, (vec3){ -right[0] * CAMERA_SPEED * delta, 0.0f, -right[2] * CAMERA_SPEED * delta });
}


void handle_key_move_right(struct IceCraft *ice_craft, float delta)
{
    vec3 right;
    glm_cross(ice_craft->louis.camera.front, ice_craft->louis.camera.up, right);
    glm_normalize(right);
    move_camera(&ice_craft->louis.camera, (vec3){ right[0] * CAMERA_SPEED * delta, 0.0f, right[2] * CAMERA_SPEED * delta });
}


void handle_key_jump(struct IceCraft *ice_craft, float delta)
{
    if (ice_craft->gravity_enabled && (ice_craft->space_key_is_blocked || !ice_craft->louis.can_jump))
    {
        return;
    }

    move_camera(&ice_craft->louis.camera, (vec3){ 0.0f, CAMERA_SPEED * delta, 0.0f });
    ice_craft->louis.velocity_y += 5.0f;
    ice_craft->louis.camera.position[1] += 0.001f;
    ice_craft->louis.can_jump = 0;
    ice_craft->space_key_is_blocked = 1;
    ice_craft->remaining_time_jumping_blocked = 0.5;
}


void handle_key_float_downward(struct IceCraft *ice_craft, float delta)
{
    if (ice_craft->gravity_enabled)
    {
        return;
    }

    move_camera(&ice_craft->louis.camera, (vec3){ 0.0f, -CAMERA_SPEED * delta, 0.0f });
}


void handle_key_look_up(struct IceCraft *ice_craft, float delta)
{
    rotate_camera(&ice_craft->louis.camera, CAMERA_SENSITIVITY * CAMERA_SPEED * delta, 0.0f);
}


void handle_key_look_down(struct IceCraft *ice_craft, float delta)
{
    rotate_camera(&ice_craft->louis.camera, -CAMERA_SENSITIVITY * CAMERA_SPEED * delta, 0.0f);
}


void handle_key_look_left(struct IceCraft *ice_craft, float delta)
{
    rotate_camera(&ice_craft->louis.camera, 0.0f, -CAMERA_SENSITIVITY * CAMERA_SPEED * delta);
}


void handle_key_look_right(struct IceCraft *ice_craft, float delta)
{
    rotate_camera(&ice_craft->louis.camera, 0.0f, CAMERA_SENSITIVITY * CAMERA_SPEED * delta);
}