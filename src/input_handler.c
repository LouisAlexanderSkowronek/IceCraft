#include "IceCraft/input_handler.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "IceCraft/camera.h"

void processInput(GLFWwindow *window, struct Camera *camera, struct World *world, int *show_coordinate_axes, int *c_key_is_blocked, const float delta)
{
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

    int should_place_block = 0;
    unsigned material_id;

    if (should_place_block)
    {
        for (unsigned i = 0; i < world->chunk_ptrs[0]->placed_blocks; i++)
        {
            if (player_looks_at_block(camera->position, camera->front, world->chunk_ptrs[0]->blocks + i))
            {
                struct Block *tmp_block = world->chunk_ptrs[0]->blocks + i;
                add_block_to_chunk(tmp_block->x, tmp_block->y + 1, tmp_block->z, material_id, world->chunk_ptrs[0]);
            }
        }
    }
    

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        should_place_block = 1;
        material_id = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        should_place_block = 1;
        material_id = 1;
    }

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        should_place_block = 1;
        material_id = 2;
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        should_place_block = 1;
        material_id = 3;
    }

        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        should_place_block = 1;
        material_id = 4;
    }

        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        should_place_block = 1;
        material_id = 5;
    }

    if (should_place_block)
    {
        for (unsigned i = 0; i < world->chunk_ptrs[0]->placed_blocks; i++)
        {
            if (player_looks_at_block(camera->position, camera->front, world->chunk_ptrs[0]->blocks + i))
            {
                struct Block *tmp_block = world->chunk_ptrs[0]->blocks + i;
                add_block_to_chunk(tmp_block->x, tmp_block->y + 1, tmp_block->z, material_id, world->chunk_ptrs[0]);
            }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        for (unsigned i = 0; i < world->chunk_ptrs[0]->placed_blocks; i++)
        {
            if (player_looks_at_block(camera->position, camera->front, world->chunk_ptrs[0]->blocks + i))
            {
                remove_block_from_chunk(i, world->chunk_ptrs[0]);
            }
        }
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
