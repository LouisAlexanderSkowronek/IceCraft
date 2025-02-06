#pragma once

#include "ice_craft.h"

void process_input(struct IceCraft *ice_craft, float delta);

void handle_key_press_toggle_show_coordinate_system(struct IceCraft *ice_craft);
void handle_key_release_toggle_show_coordinate_system(struct IceCraft *ice_craft);

void check_for_and_handle_selecting_item(struct IceCraft *ice_craft);

void handle_key_place(struct IceCraft *ice_craft);
void handle_key_destroy(struct IceCraft *ice_craft);

void handle_key_move_forward(struct IceCraft *ice_craft, float delta);
void handle_key_move_backward(struct IceCraft *ice_craft, float delta);
void handle_key_move_left(struct IceCraft *ice_craft, float delta);
void handle_key_move_right(struct IceCraft *ice_craft, float delta);

void handle_key_jump(struct IceCraft *ice_craft, float delta);
void handle_key_float_downward(struct IceCraft *ice_craft, float delta);

void handle_key_look_up(struct IceCraft *ice_craft, float delta);
void handle_key_look_down(struct IceCraft *ice_craft, float delta);
void handle_key_look_left(struct IceCraft *ice_craft, float delta);
void handle_key_look_right(struct IceCraft *ice_craft, float delta);