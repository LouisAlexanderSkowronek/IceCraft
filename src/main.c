#include "IceCraft/ice_craft.h"

int main()
{
    struct IceCraft ice_craft;
    init_ice_craft(&ice_craft);

    run_ice_craft_main_loop(&ice_craft);

    terminate_ice_craft(&ice_craft);

    return 0;
}
