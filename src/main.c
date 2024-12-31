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
#include "IceCraft/gui_block_selector.h"
#include "IceCraft/gui_crossbar.h"
#include "IceCraft/player.h"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define WINDOW_TITLE "IceCraft"

int gravity_enabled = 1;


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

    struct Player louis;
    init_player(&louis);
    int space_key_is_blocked = 0;

    int show_coordinate_axes = 0;
    int c_key_is_blocked = 0;

    struct World world;
    generate_flat_world(&world);

    add_block_to_chunk(5, 4, -5, 1, world.chunk);
    add_block_to_chunk(6, 4, -5, 2, world.chunk);
    add_block_to_chunk(7, 4, -5, 3, world.chunk);
    add_block_to_chunk(8, 4, -5, 5, world.chunk);
    add_block_to_chunk(6, 10, -5, 0, world.chunk);

    struct CoordinateAxes coordinate_axes;
    generate_coordinate_axes(&coordinate_axes);

    struct GUIBlockSelector block_selector;
    generate_hud(&block_selector);

    struct GUICrossbar crossbar;
    generate_crossbar(&crossbar);

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

    double remaining_time_next_print = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        const double current_time = glfwGetTime();
        const double delta = current_time - last_time;
        last_time = current_time;
        remaining_time_next_print -= delta;

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

        update_player(&louis, world.chunk, delta);

        processInput(window, &block_selector, &louis, &world, &show_coordinate_axes, &c_key_is_blocked, &space_key_is_blocked, delta);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(world_shader_program);

        vec3 camera_target;
        glm_vec3_add(louis.camera.position, louis.camera.front, camera_target);
        glm_lookat(louis.camera.position, camera_target, louis.camera.up, view);

        mat4 model = GLM_MAT4_IDENTITY_INIT;

        glUniformMatrix4fv(world_model_location, 1, GL_FALSE, (float*)model);
        glUniformMatrix4fv(world_view_location, 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(world_projection_location, 1, GL_FALSE, (float*)projection);

        glBindVertexArray(world.chunk->VAO);
        glDrawArrays(GL_TRIANGLES, 0, world.chunk->placed_blocks*BLOCK_N_VERTICES);

        if (show_coordinate_axes)
        {
            glUseProgram(coord_axes_shader_program);

            glUniformMatrix4fv(coord_axes_model_location, 1, GL_FALSE, (float*)model);
            glUniformMatrix4fv(coord_axes_view_location, 1, GL_FALSE, (float*)view);
            glUniformMatrix4fv(coord_axes_projection_location, 1, GL_FALSE, (float*)projection);

            glBindVertexArray(coordinate_axes.VAO);
            glDrawArrays(GL_LINES, 0, COORDINATE_AXES_N_VERTICES);
        }

        glUseProgram(hud_shader_program);
        glBindVertexArray(block_selector.VAO);
        glDrawArrays(GL_TRIANGLES, 0, HUD_N_VERTICES);
        glBindVertexArray(crossbar.VAO);
        glDrawArrays(GL_TRIANGLES, 0, GUI_CROSSBAR_N_VERTICES);
 
        glfwSwapBuffers(window);
        glfwPollEvents();

        frames_since_last_update++;
    }

    glDeleteVertexArrays(1, &world.chunk->VAO);
    glDeleteBuffers(1, &world.chunk->VBO);

    glDeleteVertexArrays(1, &coordinate_axes.VAO);
    glDeleteBuffers(1, &coordinate_axes.VBO);

    glDeleteProgram(world_shader_program);
    glDeleteProgram(coord_axes_shader_program);

    world_free_chunk(&world);

    glfwTerminate();
    return 0;
}
