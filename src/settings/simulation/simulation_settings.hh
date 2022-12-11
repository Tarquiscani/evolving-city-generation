#ifndef GM_SIMULATION_SETTINGS_HH
#define GM_SIMULATION_SETTINGS_HH


#include "settings/simulation/map_settings.hh"


namespace tgm
{



//TODO: 15: It's needed to gather here EVERY POSSIBLE option that affects the simulation, in order to be able to compare these options and avoinding that they're a source of desync.
struct SimSettings
{
    unsigned const test_seed = 1528u;

    float step_length = 1.f / 120.f; // Time span between each simulation update (in seconds)

    MapSettings map{};
};


inline auto sim_settings = SimSettings{};



} //namespace tgm


#endif //GM_SIMULATION_SETTINGS_HH