#include "IceCraft/input_handler.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "IceCraft/camera.h"

void processInput(GLFWwindow *window, struct Camera *camera, int *show_coordinate_axes, int *c_key_is_blocked, const float delta)
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
