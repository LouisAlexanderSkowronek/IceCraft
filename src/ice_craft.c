#include "IceCraft/ice_craft.h"

#include "IceCraft/opengl_utils.h"
#include "IceCraft/input_handler.h"
#include "IceCraft/block_vertex.h"
#include "IceCraft/block.h"
#include "IceCraft/helper_funcs.h"
#include "IceCraft/camera.h"
#include "IceCraft/chunk.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>


void init_ice_craft(struct IceCraft *ice_craft)
{
    ice_craft->gravity_enabled = 1;
    ice_craft->player_is_in_world = ICE_WORLD;
    ice_craft->player_can_go_to_ice_world = 1;
    ice_craft->render_distance = 1;

    init_glfw();

    ice_craft->window = create_window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    init_glad();

    glEnable(GL_DEPTH_TEST);

    ice_craft->world_shader_program = build_shader_program("../shaders/world/vertex_shader.glsl", "../shaders/world/fragment_shader.glsl");
    ice_craft->coord_axes_shader_program = build_shader_program("../shaders/coord_axes/vertex_shader.glsl", "../shaders/coord_axes/fragment_shader.glsl");
    ice_craft->hud_shader_program = build_shader_program("../shaders/hud/vertex_shader.glsl", "../shaders/hud/fragment_shader.glsl");

    init_empty_texture_atlas(&ice_craft->texture_atlas, "../assets/textures/textures.jpg");

    texture_atlas_add_texture(&ice_craft->texture_atlas, 1.0f/4.0f, 0.0f/1.0f, 2.0f/4.0f, 1.0f/2.0f);
    texture_atlas_add_texture(&ice_craft->texture_atlas, 0.0f/1.0f, 1.0f/2.0f, 1.0f/4.0f, 1.0f/1.0f);
    texture_atlas_add_texture(&ice_craft->texture_atlas, 1.0f/4.0f, 1.0f/2.0f, 2.0f/4.0f, 1.0f/1.0f);
    texture_atlas_add_texture(&ice_craft->texture_atlas, 0.0f/1.0f, 0.0f/1.0f, 1.0f/4.0f, 1.0f/2.0f);
    texture_atlas_add_texture(&ice_craft->texture_atlas, 2.0f/4.0f, 0.0f/1.0f, 3.0f/4.0f, 1.0f/2.0f);
    texture_atlas_add_texture(&ice_craft->texture_atlas, 2.0f/4.0f, 1.0f/2.0f, 3.0f/4.0f, 1.0f/1.0f);
    texture_atlas_add_texture(&ice_craft->texture_atlas, 3.0f/4.0f, 0.0f/1.0f, 1.0f/1.0f, 1.0f/2.0f);

    init_player(&ice_craft->louis);

    ice_craft->space_key_is_blocked = 0;
    ice_craft->c_key_is_blocked = 0;

    ice_craft->show_coordinate_axes = 0;

    init_world(&ice_craft->world);

    if (ice_craft->player_is_in_world == LOBBY)
    {
        generate_lobby_world(&ice_craft->world , &ice_craft->texture_atlas);
        load_changes_onto_world(&ice_craft->world, "../assets/worlds/lobby.s", &ice_craft->texture_atlas);
    } else if (ice_craft->player_is_in_world == ICE_WORLD)
    {
        generate_ice_world(&ice_craft->world, &ice_craft->texture_atlas, ice_craft->louis.camera.position);
        load_changes_onto_world(&ice_craft->world, "../assets/worlds/ice_world.s", &ice_craft->texture_atlas);
    } else
    {
        fprintf(stderr, "You should never see this message! ice_craft->player_is_in_world has unexpected value: %u\n", ice_craft->player_is_in_world);
        exit(1);
    }

    generate_coordinate_axes(&ice_craft->coordinate_axes);
    generate_hud(&ice_craft->block_selector);
    generate_crossbar(&ice_craft->crossbar);

    ice_craft->world_model_location = glGetUniformLocation(ice_craft->world_shader_program, "model");
    ice_craft->world_view_location  = glGetUniformLocation(ice_craft->world_shader_program, "view");
    ice_craft->world_projection_location = glGetUniformLocation(ice_craft->world_shader_program, "projection");

    ice_craft->coord_axes_model_location = glGetUniformLocation(ice_craft->coord_axes_shader_program, "model");
    ice_craft->coord_axes_view_location  = glGetUniformLocation(ice_craft->coord_axes_shader_program, "view");
    ice_craft->coord_axes_projection_location = glGetUniformLocation(ice_craft->coord_axes_shader_program, "projection");

    glm_perspective(glm_rad(45.0f), ((float) WINDOW_WIDTH) / ((float) WINDOW_HEIGHT), 0.1f, 2*ice_craft->render_distance*16.0f + 16.0f, ice_craft->projection);

    glBindTexture(GL_TEXTURE_2D, ice_craft->texture_atlas.texture_id);

    ice_craft->frames_since_last_update = 0;
    ice_craft->last_time = glfwGetTime();
    ice_craft->time_of_last_update = ice_craft->last_time;
    ice_craft->remaining_time_block_placement_blocked = 0.0;
    ice_craft->remaining_time_block_breaking_blocked = 0.0;
    ice_craft->remaining_time_jumping_blocked = 0.0;
}


void run_ice_craft_main_loop(struct IceCraft *ice_craft)
{
    const double update_timeout = 0.5;

    while (!glfwWindowShouldClose(ice_craft->window))
    {
        const double current_time = glfwGetTime();
        const double delta = current_time - ice_craft->last_time;
        ice_craft->last_time = current_time;
        ice_craft->remaining_time_block_breaking_blocked -= delta;
        ice_craft->remaining_time_block_placement_blocked -= delta;
        ice_craft->remaining_time_jumping_blocked -= delta;

        if (current_time >= ice_craft->time_of_last_update + update_timeout)
        {
            double average_fps = ice_craft->frames_since_last_update / update_timeout;
            ice_craft_update_window_caption(ice_craft, average_fps);
            ice_craft->time_of_last_update = current_time;
        }

        if (ice_craft->gravity_enabled && ice_craft->remaining_time_jumping_blocked < 0.0)
        {
            ice_craft->space_key_is_blocked = 0;
        }

        process_input(ice_craft, delta);

        if (ice_craft->gravity_enabled)
        {
            update_player(&ice_craft->louis, &ice_craft->world, delta);
        }

        world_update_cached_chunks(&ice_craft->world, ice_craft->louis.camera.position, ice_craft->render_distance, &ice_craft->texture_atlas);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ice_craft_draw_world_with_camera(ice_craft, &ice_craft->louis.camera);

        if (ice_craft->show_coordinate_axes)
        {
            ice_craft_draw_coordinate_axes(ice_craft);
        }

        ice_craft_draw_hud(ice_craft);
 
        glfwSwapBuffers(ice_craft->window);
        glfwPollEvents();

        ice_craft->frames_since_last_update++;
    }
}


void ice_craft_update_window_caption(struct IceCraft *ice_craft, double average_fps)
{
    const size_t title_size = 64;
    char title[title_size];
    snprintf(title, title_size, WINDOW_TITLE " | %.2f fps", average_fps);

    glfwSetWindowTitle(ice_craft->window, title);

    ice_craft->frames_since_last_update = 0;
}


void ice_craft_draw_world_with_camera(struct IceCraft *ice_craft, struct Camera *camera)
{
    glUseProgram(ice_craft->world_shader_program);

    vec3 camera_target;
    glm_vec3_add(camera->position, camera->front, camera_target);
    glm_lookat(camera->position, camera_target, camera->up, ice_craft->view);

    mat4 model = GLM_MAT4_IDENTITY_INIT;

    glUniformMatrix4fv(ice_craft->world_model_location, 1, GL_FALSE, (float*)model);
    glUniformMatrix4fv(ice_craft->world_view_location, 1, GL_FALSE, (float*)ice_craft->view);
    glUniformMatrix4fv(ice_craft->world_projection_location, 1, GL_FALSE, (float*)ice_craft->projection);

    for (unsigned i = 0; i < ice_craft->world.n_cached_chunks; i++)
    {
        glBindVertexArray((ice_craft->world.cached_chunks+i)->VAO);
        glDrawArrays(GL_TRIANGLES, 0, (ice_craft->world.cached_chunks+i)->placed_blocks*BLOCK_N_VERTICES);
    }
}


void ice_craft_draw_coordinate_axes(struct IceCraft *ice_craft)
{
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glUseProgram(ice_craft->coord_axes_shader_program);

    glUniformMatrix4fv(ice_craft->coord_axes_model_location, 1, GL_FALSE, (float*)model);
    glUniformMatrix4fv(ice_craft->coord_axes_view_location, 1, GL_FALSE, (float*)ice_craft->view);
    glUniformMatrix4fv(ice_craft->coord_axes_projection_location, 1, GL_FALSE, (float*)ice_craft->projection);

    glBindVertexArray(ice_craft->coordinate_axes.VAO);
    glDrawArrays(GL_LINES, 0, COORDINATE_AXES_N_VERTICES);
}


void ice_craft_draw_hud(struct IceCraft *ice_craft)
{
    glUseProgram(ice_craft->hud_shader_program);
    glBindVertexArray(ice_craft->block_selector.VAO);
    glDrawArrays(GL_TRIANGLES, 0, HUD_N_VERTICES);
    glBindVertexArray(ice_craft->crossbar.VAO);
    glDrawArrays(GL_TRIANGLES, 0, GUI_CROSSBAR_N_VERTICES);
}


void terminate_ice_craft(struct IceCraft *ice_craft)
{
    glDeleteVertexArrays(1, &ice_craft->coordinate_axes.VAO);
    glDeleteBuffers(1, &ice_craft->coordinate_axes.VBO);

    glDeleteProgram(ice_craft->world_shader_program);
    glDeleteProgram(ice_craft->coord_axes_shader_program);

    if (ice_craft->player_is_in_world == LOBBY)
    {
        save_world(&ice_craft->world, "../assets/worlds/lobby.s");
    } else if (ice_craft->player_is_in_world == ICE_WORLD)
    {
        save_world(&ice_craft->world, "../assets/worlds/ice_world.s");
    } else
    {
        fprintf(stderr, "You should never see this message here from terminate_ice_craft! Player is in unexpected world.\n");
        exit(1);
    }

    world_free(&ice_craft->world);

    free_texture_atlas(&ice_craft->texture_atlas);

    glfwTerminate();
}