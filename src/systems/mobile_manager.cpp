#include "mobile_manager.h"


#include "settings/gameplay_settings.hh"


namespace tgm
{



MobileManager::MobileManager(MobileEventQueues & mobile_events, MobileBody & player_body, DataArray<MobileBody> & npc_bodies, Camera & camera,
                             DynamicManager & dynamic_manager, TileSet & tiles, DataArray<Building> & buildings, DoorEventQueues & door_events) :
    m_mobile_events(mobile_events), m_player_body(player_body), m_npc_bodies(npc_bodies), m_camera(camera),
    m_dynamic_manager(dynamic_manager), m_tiles(tiles), m_buildings(buildings), m_door_events(door_events) { }


void MobileManager::add_playerBody_to_map()
{
    auto const occupied_tiles = TrailSystem::compute_tilesFromVolume(m_player_body.feet_square(), m_player_body.z_floor());
    for (auto const p : occupied_tiles)
    {
        m_tiles.add_mobile(p);
    }


    auto const& subimage = pick_subimageSet(m_player_body.style()).pick_subimage(m_player_body.get_moveDirection());

    auto const new_spriteId = m_dynamic_manager.create(m_player_body.volume(), subimage, true);

    m_player_body.set_spriteId(new_spriteId);
    
    // Set camera target
    m_camera.set_target(m_player_body.volume(), subimage, Direction::none, {});
}

void MobileManager::update()
{
    auto & pt_queue = m_mobile_events.get<DebugPlayerTeleportationEv>();
    while (!pt_queue.empty())
    {
        auto & e = pt_queue.front();
                

        auto orig_square = m_player_body.feet_square();

        FloatRect dest_square{ GSet::tiles_to_units(e.tile_pos.x), GSet::tiles_to_units(e.tile_pos.y), orig_square.length, orig_square.width };

        move_player(orig_square, m_player_body.z_floor(), dest_square, e.tile_pos.z);


        pt_queue.pop();
    }

    
    auto & dspfs_queue = m_mobile_events.get<DebugShrinkPlayerFeetSquareEv>();
    while (!dspfs_queue.empty())
    {
        auto new_dim = m_player_body.feet_square().length * 1.5f;
        
        auto orig_square = m_player_body.feet_square();
        auto z_floor = m_player_body.z_floor();

        if(new_dim > 0.f)
            m_player_body.debug_setFeetDim(new_dim); 
        
        move_player(orig_square, z_floor, m_player_body.feet_square(), z_floor);


        dspfs_queue.pop();
    }
            

    auto & depfs_queue = m_mobile_events.get<DebugEnlargePlayerFeetSquareEv>();
    while (!depfs_queue.empty())
    {
        auto orig_square = m_player_body.feet_square();
        auto z_floor = m_player_body.z_floor();

        auto new_dim = m_player_body.feet_square().length / 1.5f;
        if (new_dim < 1000.f)
            m_player_body.debug_setFeetDim(new_dim);
        
        move_player(orig_square, z_floor, m_player_body.feet_square(), z_floor);


        depfs_queue.pop();
    }
}

void MobileManager::move()
{
    // Trail system for the first mobile: i.e. the player
    // ---------------

    auto move_drc = m_player_body.get_moveDirection();

    switch (move_drc)
    {
        case Direction::U:
        case Direction::L:
        case Direction::none:
            //	#if PLAYERMOVEMENT_DEBUGLOG
            //		//PMlog << "Player movement direction = Direction::none or U or L" << player.get_feetSquare() << std::endl;
            //	#endif
            break;

        default:
        {
            auto const z_floor = m_player_body.z_floor();
            auto const orig_square = m_player_body.feet_square();
            auto const velocity = m_player_body.velocity();

            #if GMPLSET_DETECT_COLLISIONS
                auto const [adjusted_destSquare, doors_to_open] = TrailSystem::compute_nearestPosition(orig_square, z_floor, move_drc, velocity, m_tiles);
                //auto const adjusted_destSquare = TrailSystem::debug_brute_computeNearestPosition(orig_square, z_floor, move_drc, m_player_body.rounded_velocity(), m_tiles);
                //std::vector<CompleteId> const doors_to_open;


                #if TESTLOG_PLAYERMOVEMENT_TRAIL_SYSTEM_VS_BRUTE
                    
                    auto const brute_adjustedDestSquare = TrailSystem::debug_brute_computeNearestPosition(orig_square, z_floor,
                                                                                                          move_drc, velocity, m_tiles);
                    

                    //For the small numbers of the map an abs_tolerance is enough. The rel_tolerance is set at a value that prevents 
                    //it to kick-in for a map smaller than 1000x1000. I've set and abs_tolerance of GSet::mu because the two algorithms
                    //produce slightly different results, but they can't differ more than GSet::mu.
                    if (!are_equal(adjusted_destSquare, brute_adjustedDestSquare, GSet::mu, 0.001f * GSet::mu))
                    {
                        PMtest.begin_report();
                        PMtest << bug_bigNotification()
                               << Logger::nltb << "feet_square: " << orig_square
                               << Logger::nltb << "move_drc:    " << move_drc
                               << Logger::nltb << "velocity:    " << velocity
                               << Logger::nltb << "adjusted_destSquare:      " << adjusted_destSquare
                               << Logger::nltb << "brute_adjustedDestSquare: " << brute_adjustedDestSquare << "\n\n\n";
                        PMtest.end_report();
                    }
                #endif
            #else
                auto const adjusted_destSquare = DirectionUtil::compute_newRect(orig_square, move_drc, velocity);
                std::vector<CompleteId> const doors_to_open;
            #endif
                    
            move_player(orig_square, z_floor, adjusted_destSquare, z_floor);

            for(auto did : doors_to_open)
                m_door_events.push<TryOpenDoorEv>(did);

        }
        break;
    }

    
    // Move the other mobiles, i.e. the NPCs
    // ---------------
    //for (auto & npc_body : m_npc_bodies)
    //{
    //	//TODO: do stuffs and collisions...
    //}
}


void MobileManager::move_player(FloatRect const orig_square, int const orig_zFloor, FloatRect const dest_square, int const dest_zFloor)
{
    auto const dest_tiles = TrailSystem::compute_tilesFromVolume(dest_square, dest_zFloor);
    for (auto const p : dest_tiles)
    {
        if (!m_tiles.get(p))
        {
            g_log << "Cannot move player on unexistent tiles." << std::endl;
            return;
        }
    }

    auto const previouslyOccupied_tiles = TrailSystem::compute_tilesFromVolume(orig_square, orig_zFloor);
    for (auto const p : previouslyOccupied_tiles)
    {
        m_tiles.remove_mobile(p);
    }

    m_player_body.set_feetPosition(dest_square.top, dest_square.left);
    m_player_body.set_zFloor(dest_zFloor);

    for (auto const p : dest_tiles)
    {
        m_tiles.add_mobile(p);
    }


    //TODO: Refactor this chunk of code.
    std::vector<BuildingAreaId> TEMP_added_areas;
    std::vector<IntParallelepiped> TEMP_noRoofAreas_volumes;
    for(auto const occ_pos : dest_tiles)
    {
        auto t = m_tiles.get(occ_pos);

        if (t && t->is_innerArea())
        {
            auto const& b = m_buildings.get_or_throw(t->get_innerAreaInfo().bid());
            auto const aid = t->get_innerAreaInfo().aid();
            auto const& a = b.getOrThrow_area(aid);
            if (std::find(TEMP_added_areas.begin(), TEMP_added_areas.end(), aid) == TEMP_added_areas.end())
            {
                TEMP_added_areas.push_back(aid);
                TEMP_noRoofAreas_volumes.push_back(a.volume());
            }
        }
        else if (t && t->is_border())
        {
            for (auto const info : t->get_borderInfos())
            {
                if (!info.is_empty())
                {
                    auto const& b = m_buildings.get_or_throw(info.bid());
                    auto const aid = info.aid();
                    auto const& a = b.getOrThrow_area(aid);
                    if (std::find(TEMP_added_areas.begin(), TEMP_added_areas.end(), aid) == TEMP_added_areas.end())
                    {
                        TEMP_added_areas.push_back(aid);
                        TEMP_noRoofAreas_volumes.push_back(a.volume());
                    }
                }
            }
        }
    }


    auto drc = m_player_body.get_moveDirection();
    auto const& subimage = pick_subimageSet(m_player_body.style()).pick_subimage(drc);

    m_dynamic_manager.modify( m_player_body.sprite_id(), m_player_body.volume(), subimage, drc, true);
    

    // Update camera target
    m_camera.set_target(m_player_body.volume(), subimage, drc, TEMP_noRoofAreas_volumes);
}
        
auto MobileManager::pick_subimageSet(MobileStyle const style) -> MobileSubimageSet const&
{
    switch (style)
    {
        case MobileStyle::None:
            throw std::runtime_error("Trying to pick the texture subimage of a mobile that has no style.");
            break;
        case MobileStyle::Builder:
            return builder_subimageSet;
            break;
        case MobileStyle::TestCharacter:
            return test_character_subimage_set;
            break;
        case MobileStyle::Warrior:
            return warrior_subimageSet;
            break;
        default:
            throw std::runtime_error("Trying to pick the texture of a mobile that has an unexpected style.");
            break;
    }
}



} // namespace tgm