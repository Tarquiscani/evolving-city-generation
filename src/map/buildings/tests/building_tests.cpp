#include "building_tests.hh"

#include "settings/simulation/simulation_settings.hh"

#include "settings/debug/visual_debug_player_movement_settings.hh"
#include "settings/debug/visual_debug_hip_roof_matrix_settings.hh"

#include "debug/visual/building_expansion_stream.hh"

namespace tgm
{



namespace BuildingSpecialCases
{
    auto tris_building(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "always_replace" };
        prefab.append_area(AreaType::field,   pos + Vector3i{  2,  2, 0 }, { 10, 10 });
        prefab.append_area(AreaType::cowshed, pos + Vector3i{ 11,  2, 0 }, { 10, 10 });
        prefab.append_area(AreaType::cowshed, pos + Vector3i{ 20,  2, 0 }, { 10, 10 });
        prefab.append_area(AreaType::cowshed, pos + Vector3i{  2, 11, 0 }, { 10, 10 });
        prefab.append_area(AreaType::field,	  pos + Vector3i{ 11, 11, 0 }, { 10, 10 });
        prefab.append_area(AreaType::cowshed, pos + Vector3i{ 20, 11, 0 }, { 10, 10 });
        prefab.append_area(AreaType::cowshed, pos + Vector3i{  2, 20, 0 }, { 10, 10 });
        prefab.append_area(AreaType::cowshed, pos + Vector3i{ 11, 20, 0 }, { 10, 10 });
        prefab.append_area(AreaType::field,   pos + Vector3i{ 20, 20, 0 }, { 10, 10 });

        return prefab;
    }

    auto inner_yard(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "farm" };
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 0,   0, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 10,  0, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 20,  0, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  0, 10, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 16, 10, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  0, 20, 0 }, { 11, 11 });

        prefab.add_door(pos + Vector3i{ 12, 10, 0 });
        prefab.add_door(pos + Vector3i{ 10, 14, 0 });
        prefab.add_door(pos + Vector3i{ 10, 15, 0 });
        prefab.add_door(pos + Vector3i{ 10, 16, 0 });
        prefab.add_door(pos + Vector3i{ 16, 14, 0 });
        prefab.add_door(pos + Vector3i{ 16, 15, 0 });
        prefab.add_door(pos + Vector3i{ 16, 16, 0 });

        return prefab;
    }
    
    auto thin_innerYard(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "farm" };
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  0,  0, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 10,  0, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 20,  0, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  0, 10, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 12, 10, 0 }, { 11, 11 });

        return prefab;
    }
    
    auto singleTile_innerYard(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "farm" };
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  0,  0, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ -2, 10, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 10,  2, 0 }, { 11, 11 });
        
        prefab.add_door(pos + Vector3i{  9, 10, 0 });
        prefab.add_door(pos + Vector3i{  8, 11, 0 });
        prefab.add_door(pos + Vector3i{ 10, 11, 0 });

        return prefab;
    }

    auto twoTile_innerYard(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "farm" };
        prefab.append_area(AreaType::farmyard, pos + Vector3i( 0,  0, 0), { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i(-2, 10, 0), { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i(10,  3, 0), { 11, 11 });

        return prefab;
    }
    
    auto double_innerYard(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "farm" };
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  0,  0, 0 }, { 11, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 10,  0, 0 }, { 11,  6 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 15,  5, 0 }, {  6,  6 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  0, 10, 0 }, {  6, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  5, 15, 0 }, {  6,  6 });

        return prefab;
    }

    auto replaceable_in_innerYard(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "farm" };
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  0,  0, 0 }, {  6,  6 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  5,  0, 0 }, { 11,  6 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 15,  0, 0 }, { 11,  6 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 20,  5, 0 }, {  6, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 20, 15, 0 }, {  6, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  0,  5, 0 }, {  6, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  0, 15, 0 }, {  6, 11 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{  5, 20, 0 }, { 11,  6 });
        prefab.append_area(AreaType::farmyard, pos + Vector3i{ 15, 20, 0 }, {  6,  6 });
        
        prefab.append_area(AreaType::field, pos + Vector3i{  5,  5, 0 }, { 11, 11 });	//replaceable area

        return prefab;
    }
        
    auto full_replacement(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "always_replace" };

        prefab.append_area(AreaType::field, pos + Vector3i{  0,  0, 0 }, { 6, 6 });	//replaceable area
        prefab.append_area(AreaType::field, pos + Vector3i{  5,  0, 0 }, { 6, 6 });	//replaceable area

        return prefab;
    }

    auto disconnecting_replacement(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "always_replace" };

        prefab.append_area(AreaType::cowshed, pos + Vector3i{  0,  0, 0 }, { 11, 11 });
        prefab.append_area(AreaType::field,   pos + Vector3i{  0, 10, 0 }, { 21, 21 });	//replaceable area
        prefab.append_area(AreaType::cowshed, pos + Vector3i{  0, 30, 0 }, { 11, 11 });

        return prefab;
    }

    auto disconnecting_replacement2(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "always_replace" };

        prefab.append_area(AreaType::cowshed, pos + Vector3i{  0,  0, 0 }, { 11, 11 });
        prefab.append_area(AreaType::field,   pos + Vector3i{  0, 10, 0 }, { 11,  8 });	//replaceable area
        prefab.append_area(AreaType::field,   pos + Vector3i{  0, 17, 0 }, { 11,  9 });	//replaceable area
        prefab.append_area(AreaType::cowshed, pos + Vector3i{  0, 25, 0 }, { 11, 11 });

        return prefab;
    }

    auto nondisconnecting_replacement(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "always_replace" };

        prefab.append_area(AreaType::cowshed, pos + Vector3i{  0,  0, 0 }, { 11, 11 });
        prefab.append_area(AreaType::field,   pos + Vector3i{  0, 10, 0 }, { 11, 10 });	//replaceable area
        prefab.append_area(AreaType::cowshed, pos + Vector3i{  0, 19, 0 }, { 11, 11 });

        return prefab;
    }

    auto firstArea_replacement(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "always_replace" };
        
        prefab.append_area(AreaType::field,   pos + Vector3i{  0,  0, 0 }, { 10, 10 });	//replaceable area
        prefab.append_area(AreaType::cowshed, pos + Vector3i{  0,  9, 0 }, { 11, 11 });
        prefab.append_area(AreaType::cowshed, pos + Vector3i{  0, 19, 0 }, { 11, 11 });

        return prefab;
    }

    
    auto thinInnerYard_replacement(Vector3i const pos) -> PrefabBuilding
    {
        auto prefab = PrefabBuilding{ "always_replace" };

        prefab.append_area(AreaType::field,	  pos + Vector3i{  0,  2, 0 }, { 10, 10 });	//replaceable area
        prefab.append_area(AreaType::cowshed, pos + Vector3i{  9,  0, 0 }, { 10, 10 });
        prefab.append_area(AreaType::cowshed, pos + Vector3i{  9, 11, 0 }, { 10, 10 });
        prefab.append_area(AreaType::cowshed, pos + Vector3i{ 18,  6, 0 }, { 10, 10 });

        return prefab;
    }
};


namespace BuildingAlgorithmTests
{
    static void inner_yard(PrefabBuilding & prefab, Vector3i & newArea_pos, Vector2i & newArea_dims, 
                           std::vector<BuildingAreaCompleteId> & replaceable_areas)
    {
        auto const pos = Vector3i{ 30, 30, 0 };

        prefab = BuildingSpecialCases::inner_yard(pos);

        newArea_pos = pos + Vector3i{ 10, 20, 0 };
        newArea_dims = Vector2i{ 11, 11 };
    }

    static void thin_innerYard(PrefabBuilding & prefab, Vector3i & newArea_pos, Vector2i & newArea_dims, 
                               std::vector<BuildingAreaCompleteId> & replaceable_areas)
    {
        auto const pos = Vector3i{ 30, 30, 0 };

        prefab = BuildingSpecialCases::thin_innerYard(pos);

        newArea_pos = pos + Vector3i{ 10, 20, 0 };
        newArea_dims = Vector2i{ 11, 11 };
    }

    static void singleTile_innerYard(PrefabBuilding & prefab, Vector3i & newArea_pos, Vector2i & newArea_dims, 
                                     std::vector<BuildingAreaCompleteId> & replaceable_areas)
    {
        auto const pos = Vector3i{ 30, 30, 0 };
        
        prefab = BuildingSpecialCases::singleTile_innerYard(pos);

        newArea_pos = pos + Vector3i{ 8, 12, 0 };
        newArea_dims = Vector2i{ 11, 11 };
    }

    static void twoTile_innerYard(PrefabBuilding & prefab, Vector3i & newArea_pos, Vector2i & newArea_dims, 
                                  std::vector<BuildingAreaCompleteId> & replaceable_areas)
    {
        auto const pos = Vector3i{ 30, 30, 0 };

        prefab = BuildingSpecialCases::twoTile_innerYard(pos);

        newArea_pos = pos + Vector3i{ 8, 13, 0 };
        newArea_dims = Vector2i{ 11, 11 };
    }

    #pragma warning(disable: 4702)
    void blockOutline_tests(GameMap & map)
    {		
        if (sim_settings.map.ground_floor != 0 || map.tiles().length() < 100 || map.tiles().width() < 150 || map.tiles().height() < 1)
        {
            throw std::runtime_error("Cannot perform the test if the map is not large enough.");
        }
        
        #if !BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            throw std::runtime_error("Cannot perform the test if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE is decativated.");
        #endif


        static auto test_id = 0;
        static auto test_count = 4;
        static auto last_bid = BuildingId{ 0 };

        ++test_id;
        if (test_id > test_count) { test_id = 1; }

        if (last_bid != 0)
        {
            #if VISUALDEBUG
                visualDebug_runtime_openWindow = false;
            #endif

            map.debug_remove_building(last_bid);
            last_bid = 0;

            #if VISUALDEBUG
                visualDebug_runtime_openWindow = true;
            #endif
        }


        g_log << "Added a building to test the BlockOutline algorithm";

        auto prefab = PrefabBuilding{ "farm" };

        auto newArea_pos = Vector3i{};
        auto newArea_dims = Vector2i{};

        auto replaceable_areas = std::vector<BuildingAreaCompleteId>{};
        
        switch (test_id)
        {
            case 1:
                inner_yard(prefab, newArea_pos, newArea_dims, replaceable_areas);
                break;
            case 2:
                thin_innerYard(prefab, newArea_pos, newArea_dims, replaceable_areas);
                break;
            case 3:
                singleTile_innerYard(prefab, newArea_pos, newArea_dims, replaceable_areas);
                break;
            case 4:
                twoTile_innerYard(prefab, newArea_pos, newArea_dims, replaceable_areas);
                break;
            default:
                throw std::runtime_error("Unexpected test_id");
        }
        
        #if VISUALDEBUG
            int old_maxRecordableDepth = visualDebug_runtime_maxRecordableDepth;
            visualDebug_runtime_maxRecordableDepth = 0;
        #endif

        auto const building_handler = map.debug_build_prefabBuilding(prefab);
        last_bid = building_handler.first;
        if (!last_bid) { throw std::runtime_error("Impossible to build the building."); }


        #if BUILDEXP_VISUALDEBUG
            std::ostringstream oss; oss << "BlockOutline algorithm Test - Case " << test_id;
            BEdeb.begin_chapter(oss.str());
        #endif
        #if VISUALDEBUG
            visualDebug_runtime_openWindowForBuildingExpansion = true;
        #endif

        map.debug_is_area_buildable(building_handler.second->cbid(), last_bid, *building_handler.second, newArea_pos, newArea_dims, replaceable_areas);

        #if BUILDEXP_VISUALDEBUG
            BEdeb.end_chapter();
        #endif
        #if VISUALDEBUG
            visualDebug_runtime_openWindowForBuildingExpansion = false;
            visualDebug_runtime_maxRecordableDepth = old_maxRecordableDepth;
        #endif
            
        #pragma warning(default: 4702)
    }

    
    void automatic_cityDevelopment(GameMap & map, std::vector<BuildingId> & created_buildings)
    {
        if (sim_settings.map.ground_floor != 0 || map.tiles().length() < 100 || map.tiles().width() < 150 || map.tiles().height() < 1)
        {
            throw std::runtime_error("Cannot perform the test if the map is not large enough.");
        }
        
        #if BUILDEXP_VISUALDEBUG || HIPROOFMATRIX_VISUALDEBUG
            throw std::runtime_error("Cannot perform the test if BUILDEXP_VISUALDEBUG or HIPROOFMATRIX_VISUALDEBUG are activated.");
        #endif

        Logger lgr{ g_log };
        
        auto const building_recipe = BuildingRecipe{ { 200.f, 200.f }, AreaType::cowshed, { 10, 10 }, "farm" };


        for (auto i = 0; i < 300; ++i)
        {
            lgr << Logger::nltb << "New building #" << i;

            auto const building_id = map.debug_buildBuilding_inNearestCity(building_recipe);
            if (building_id) { created_buildings.push_back(building_id.value()); }

            lgr << Logger::addt;

            for (auto j = 0; j < 4; ++j)
            {
                lgr << Logger::nltb << "Expansion #" << j;
                
                //Expand only the last 100 created buildings.
                auto it = created_buildings.crbegin();
                auto count = 0;
                while (it != created_buildings.crend() && count != 100)
                {
                    map.debug_request_buildingExpansion(*it);

                    ++it;
                    ++count;
                }

                map.debug_expand_buildings();
            }

            lgr << Logger::remt;
        }
    }
}



} //namespace tgm