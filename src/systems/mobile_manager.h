#ifndef GM_MOBILE_MANAGER_H
#define GM_MOBILE_MANAGER_H


#include "characters/mobile.h"
#include "data_strctures/data_array.hh"
#include "mediators/queues/mobile_ev.hh"
#include "graphics/camera.hh"
#include "graphics/dynamic_manager.hh"
#include "graphics/mobile_subimage_set.hh"
#include "map/buildings/building.hh"
#include "map/map_forward_decl.hh"
#include "map/tiles/tile_set.hh"
#include "settings/graphics_settings.hh"
#include "system/parallelepiped.hh"
#include "systems/trail_system.hh"
#include "utilities.hh"

#include "debug/test_logger/streams.h"
#include "debug/visual/player_movement_stream.hh"


namespace tgm
{



class MobileManager
{
    public:
        MobileManager(MobileEventQueues & mobile_events, MobileBody & player_body, DataArray<MobileBody> & npc_bodies, Camera & camera,
                      DynamicManager & dynamic_manager, TileSet & tiles, DataArray<Building> & buildings, DoorEventQueues & door_events);
        
        void add_playerBody_to_map();
        void update();
        void move();


    private:
        MobileEventQueues & m_mobile_events;
        MobileBody & m_player_body;
        DataArray<MobileBody> & m_npc_bodies;
        Camera & m_camera;
        DynamicManager & m_dynamic_manager;
        TileSet& m_tiles;
        DataArray<Building> & m_buildings;
        DoorEventQueues & m_door_events;

        
        #if GSET_ALTERNATIVE_ASSETS
            static inline MobileSubimageSet const test_character_subimage_set{ {0.f, 432.f, 0.f, 432.f}, default_texture_dynamics };
            static inline MobileSubimageSet const builder_subimageSet{ {0.f, 432.f, 0.f, 432.f}, default_texture_dynamics };
            static inline MobileSubimageSet const warrior_subimageSet{ {0.f, 432.f, 0.f, 432.f}, default_texture_dynamics };
        #else
            static inline MobileSubimageSet const test_character_subimage_set{ { 896.f, 640.f, 1792.f, 1280.f }, default_texture_dynamics };
            static inline MobileSubimageSet const builder_subimageSet{ { 896.f, 0.f, 1792.f, 0.f }, default_texture_dynamics };
            static inline MobileSubimageSet const warrior_subimageSet{ { 896.f, 673.f, 32.f, 41.f, 1792.f, 1346.f, 64.f, 82.f }, default_texture_dynamics };
        #endif


        ////
        //	@orig_zPos, @dest_zPos: tiles reference system.
        ////
        void move_player(FloatRect const orig_square, int const orig_zPos, FloatRect const dest_square, int const dest_zPos);


        static auto pick_subimageSet(MobileStyle const style) -> MobileSubimageSet const&;
};



} // namespace tgm


#endif //GM_MOBILE_MANAGER_H