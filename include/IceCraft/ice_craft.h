#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cglm/cglm.h"

#include "coordinate_axes.h"
#include "world.h"
#include "gui_block_selector.h"
#include "gui_crossbar.h"
#include "player.h"
#include "texture_atlas.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define WINDOW_TITLE "IceCraft"

struct IceCraft
{
    int gravity_enabled;
    
    GLFWwindow *window;

    unsigned world_shader_program;
    unsigned coord_axes_shader_program;
    unsigned hud_shader_program;

    struct TextureAtlas texture_atlas;

    struct Player louis;

    int space_key_is_blocked;
    int c_key_is_blocked;

    int show_coordinate_axes;

    struct World world;

    struct CoordinateAxes coordinate_axes;
    struct GUIBlockSelector block_selector;
    struct GUICrossbar crossbar;

    GLuint world_model_location;
    GLuint world_view_location;
    GLuint world_projection_location;
    GLuint coord_axes_model_location;
    GLuint coord_axes_view_location;
    GLuint coord_axes_projection_location;

    mat4 view, projection;

    unsigned frames_since_last_update;
    double last_time;
    double time_of_last_update;
    double remaining_time_block_placement_blocked;
    double remaining_time_block_breaking_blocked;
    double remaining_time_jumping_blocked;
};

void init_ice_craft(struct IceCraft *ice_craft);

void run_ice_craft_main_loop(struct IceCraft *ice_craft);

void ice_craft_update_window_caption(struct IceCraft *ice_craft, double average_fps);

void ice_craft_draw_world_with_camera(struct IceCraft *ice_craft, struct Camera *camera);

void ice_craft_draw_coordinate_axes(struct IceCraft *ice_craft);

void ice_craft_draw_hud(struct IceCraft *ice_craft);

void terminate_ice_craft(struct IceCraft *ice_craft);