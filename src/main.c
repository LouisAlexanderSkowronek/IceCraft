#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cglm/cglm.h"

#include "IceCraft/camera.h"
#include "IceCraft/block_vertex.h"
#include "IceCraft/block.h"
#include "IceCraft/coordinate_axes.h"
#include "IceCraft/chunk.h"
#include "IceCraft/helper_funcs.h"
#include "IceCraft/opengl_utils.h"
#include "IceCraft/input_handler.h"
#include "IceCraft/world.h"
#include "IceCraft/hud.h"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define WINDOW_TITLE "IceCraft"


int main()
{
    init_glfw();

    GLFWwindow *window = create_window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    init_glad();

    glEnable(GL_DEPTH_TEST);

    unsigned world_shader_program = build_shader_program("../shaders/world/vertex_shader.glsl", "../shaders/world/fragment_shader.glsl");

    unsigned coord_axes_shader_program = build_shader_program("../shaders/coord_axes/vertex_shader.glsl", "../shaders/coord_axes/fragment_shader.glsl");

    unsigned hud_shader_program = build_shader_program("../shaders/hud/vertex_shader.glsl", "../shaders/hud/fragment_shader.glsl");

    unsigned texture_atlas = load_jpg_texture("../assets/textures/textures.jpg");

    struct Camera camera;
    init_camera(&camera);

    int show_coordinate_axes = 0;
    int c_key_is_blocked = 0;

    struct World world;
    generate_flat_world(&world);

    struct CoordinateAxes coordinate_axes;
    generate_coordinate_axes(&coordinate_axes);

    struct HUD hud;
    generate_hud(&hud);

    for (unsigned i = 0; i < WORLD_N_CHUNKS; i++)
    {
        generate_chunk_vao_and_vbo(&world.chunk_ptrs[i]->VAO, &world.chunk_ptrs[i]->VBO, world.chunk_ptrs[i]);
    }

    GLuint coord_axes_VBO, coord_axes_VAO;
    generate_coord_axes_vao_and_vbo(&coord_axes_VAO, &coord_axes_VBO, &coordinate_axes);

    const GLuint world_model_location = glGetUniformLocation(world_shader_program, "model");
    const GLuint world_view_location  = glGetUniformLocation(world_shader_program, "view");
    const GLuint world_projection_location = glGetUniformLocation(world_shader_program, "projection");

    const GLuint coord_axes_model_location = glGetUniformLocation(coord_axes_shader_program, "model");
    const GLuint coord_axes_view_location  = glGetUniformLocation(coord_axes_shader_program, "view");
    const GLuint coord_axes_projection_location = glGetUniformLocation(coord_axes_shader_program, "projection");


    mat4 view, projection;
    glm_perspective(glm_rad(45.0f), ((float) WINDOW_WIDTH) / ((float) WINDOW_HEIGHT), 0.1f, 100.0f, projection);

    unsigned frames_since_last_update = 0;
    double last_time = glfwGetTime();
    double time_of_last_update = last_time;
    const double update_timeout = 0.5;

    glBindTexture(GL_TEXTURE_2D, texture_atlas);

    while (!glfwWindowShouldClose(window))
    {
        double current_time = glfwGetTime();
        const double delta = current_time - last_time;
        last_time = current_time;

        if (current_time >= time_of_last_update + update_timeout)
        {
            double average_fps = frames_since_last_update / update_timeout;
            const size_t title_size = 256;
            char title[title_size];
            snprintf(title, title_size, WINDOW_TITLE " | %.2f fps", average_fps);

            glfwSetWindowTitle(window, title);

            frames_since_last_update = 0;
            time_of_last_update = current_time;
        }

        processInput(window, &hud, &camera, &world, &show_coordinate_axes, &c_key_is_blocked, delta);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(world_shader_program);

        vec3 camera_target;
        glm_vec3_add(camera.position, camera.front, camera_target);
        glm_lookat(camera.position, camera_target, camera.up, view);

        mat4 model = GLM_MAT4_IDENTITY_INIT;

        glUniformMatrix4fv(world_model_location, 1, GL_FALSE, (float*)model);
        glUniformMatrix4fv(world_view_location, 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(world_projection_location, 1, GL_FALSE, (float*)projection);

        for (unsigned c = 0; c < WORLD_N_CHUNKS; c++)
        {
            glBindVertexArray(world.chunk_ptrs[c]->VAO);
            glDrawArrays(GL_TRIANGLES, 0, world.chunk_ptrs[c]->placed_blocks*BLOCK_N_VERTICES);
        }

        if (show_coordinate_axes)
        {
            glUseProgram(coord_axes_shader_program);

            glUniformMatrix4fv(coord_axes_model_location, 1, GL_FALSE, (float*)model);
            glUniformMatrix4fv(coord_axes_view_location, 1, GL_FALSE, (float*)view);
            glUniformMatrix4fv(coord_axes_projection_location, 1, GL_FALSE, (float*)projection);

            glBindVertexArray(coord_axes_VAO);
            glDrawArrays(GL_LINES, 0, COORDINATE_AXES_N_VERTICES);
        }

        glUseProgram(hud_shader_program);
        glBindVertexArray(hud.VAO);
        glDrawArrays(GL_TRIANGLES, 0, HUD_N_VERTICES);
 
        glfwSwapBuffers(window);
        glfwPollEvents();

        frames_since_last_update++;
    }

    for (unsigned i = 0; i < WORLD_N_CHUNKS; i++)
    {
        glDeleteVertexArrays(1, &world.chunk_ptrs[i]->VAO);
        glDeleteBuffers(1, &world.chunk_ptrs[i]->VBO);
    }

    glDeleteVertexArrays(1, &coord_axes_VAO);
    glDeleteBuffers(1, &coord_axes_VBO);

    glDeleteProgram(world_shader_program);
    glDeleteProgram(coord_axes_shader_program);

    world_free_chunks(&world);

    glfwTerminate();
    return 0;
}
