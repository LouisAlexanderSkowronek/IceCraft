#include "IceCraft/ice_craft.h"

int main(int argc, char **argv)
{
    struct IceCraft ice_craft;
    init_ice_craft(&ice_craft);

    if (argc > 1 && argv[1][0] == 'd' && argv[1][1] == 'e' && argv[1][2] == 'v')
    {
        ice_craft.player_can_go_to_ice_world = 1;
    }

    run_ice_craft_main_loop(&ice_craft);

    terminate_ice_craft(&ice_craft);

    return 0;
}
