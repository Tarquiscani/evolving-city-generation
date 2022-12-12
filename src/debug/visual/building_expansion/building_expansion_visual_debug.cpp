#include "building_expansion_visual_debug.hh"

#if BUILDEXP_VISUALDEBUG

#include <iomanip>
#include <sstream>
#include <algorithm>
#include <utility>
#include <filesystem>

#include "map/direction.h"
#include "map/tiles/tile.hh"
#include "map/gamemap.h"


namespace tgm
{



auto operator<<(Logger & lgr, BuildingExpansionVisualDebugChange const& bec) -> Logger &
{
    lgr << "BuildingExpansionVisualDebugChange:"
        << Logger::addt
            << Logger::nltb << "state_id" << bec.m_change_id
            << Logger::nltb << "removed_buildings: " << bec.m_removed_buildings
            << Logger::nltb << "added_buildings: " << bec.m_added_buildings
        << Logger::remt;

    return lgr;
}


void BuildingExpansionVisualDebug::start(int const map_length, int const map_width, int const map_height, DataArray<Building> const& map_buildings)
{
    copy_mapBuildings(map_buildings);
    init_originalState();

    base_start(map_length, map_width, map_height);
}

void BuildingExpansionVisualDebug::copy_mapBuildings(DataArray<Building> const& map_buildings)
{
    for (auto const& [bid, b] : map_buildings)
    {
        m_buildings.insert({ bid, b });
    }
}

void BuildingExpansionVisualDebug::init_originalState()
{
    m_custom_originalState = BuildingExpansionVisualDebugState(m_buildings);

    //-------------------------------------------------//
    //    BuildingExpansionVisualDebugChange TESTS:    //
    //-------------------------------------------------//


    //BuildingExpansionVisualDebugChange test_change(999, "TEST");


    ////ADDITION TEST
    ////must fail
    //test_change.record_buildingAddition(36, Building({ 0,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //test_change.record_buildingAddition(36, Building({ 1,0,0 }, MapSettings::test_farm_expId, AreaType::field));


    ////REMOVAL TESTS
    ////must let the removed building and delete the newly added one
    //test_change.record_buildingRemoval(36, Building({ 0,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //test_change.record_buildingAddition(36, Building({ 1,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //test_change.record_buildingRemoval(36, Building({ 1,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //g_log << test_change << std::endl;

    ////must fail
    //test_change.record_buildingRemoval(36, Building({ 0,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //test_change.record_buildingRemoval(36, Building({ 0,0,0 }, MapSettings::test_farm_expId, AreaType::field));

    ////must remove the newly added building
    //test_change.record_buildingAddition(54, Building({ 0,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //test_change.record_buildingRemoval(54, Building({ 0,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //g_log << test_change << std::endl;

    ////must remove the building
    //test_change.record_buildingRemoval(36, Building({ 0,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //g_log << test_change << std::endl;


    ////UPDATE TESTS
    ////must replace the added building (1,0,0) with the new one (2,0,0)
    //test_change.record_buildingRemoval(36, Building({ 0,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //test_change.record_buildingAddition(36, Building({ 1,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //test_change.record_buildingUpdate(36, Building({ 1,0,0 }, MapSettings::test_farm_expId, AreaType::field), Building({ 2,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //g_log << test_change << std::endl;

    ////must fail
    //test_change.record_buildingRemoval(36, Building({ 0,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //test_change.record_buildingUpdate(36, Building({ 0,0,0 }, MapSettings::test_farm_expId, AreaType::field), Building({ 1,0,0 }, MapSettings::test_farm_expId, AreaType::field));

    ////must replace the newly added building (1,0,0) with the new one (2,0,0)
    //test_change.record_buildingAddition(54, Building({ 1,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //test_change.record_buildingUpdate(54, Building({ 1,0,0 }, MapSettings::test_farm_expId, AreaType::field), Building({ 2,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //g_log << test_change << std::endl;

    ////must remove the old building (0,0,0) and add the new one (1,0,0)
    //test_change.record_buildingUpdate(36, Building({ 0,0,0 }, MapSettings::test_farm_expId, AreaType::field), Building({ 1,0,0 }, MapSettings::test_farm_expId, AreaType::field));
    //g_log << test_change << std::endl;
}

void BuildingExpansionVisualDebug::custom_stop()
{ 
    m_custom_changes.clear();
    m_buildings.clear(); 
}

void BuildingExpansionVisualDebug::init_newChange(int const current_st)
{
    m_custom_changes.emplace_back(current_st);
}

void BuildingExpansionVisualDebug::focus_onVolume(IntParallelepiped vol)
{
    vol.behind -= 20;
    vol.left -= 20;
    vol.length += 40;
    vol.width += 40;

    VisualDebug::change_frame(vol);
}

void BuildingExpansionVisualDebug::focus_onPosition(Vector2f const pos)
{
    auto const vol = IntParallelepiped(pos.x - 100, pos.y - 100, 0, 200, 200, 1);

    VisualDebug::change_frame(vol);
}

void BuildingExpansionVisualDebug::add_building(BuildingId const bid, Building const& new_building)
{
    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::nltb << "add building";
    #endif

    validate_inChapterAction();

    auto const& [it, success] = m_buildings.insert({ bid, new_building });

    if (!success) { throw std::runtime_error("BuildingExpansionVisualDebug: Added an already added building."); }

    m_custom_changes.back().record_buildingAddition(bid, new_building);
}

void BuildingExpansionVisualDebug::update_building(BuildingId const bid, Building const& updated_building)
{
    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::nltb << "update building";
    #endif

    validate_inChapterAction();

    auto it = m_buildings.find(bid);
    if (it == m_buildings.cend()) { throw std::runtime_error("BuildingExpansionVisualDebug: Updated an unexistent building."); }

    Building const old_building = it->second;

    m_buildings.erase(it);

    m_buildings.insert({ bid, updated_building });

    m_custom_changes.back().record_buildingUpdate(bid, old_building, updated_building);
}

void BuildingExpansionVisualDebug::remove_building(BuildingId const bid)
{
    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::nltb << "remove building";
    #endif

    validate_inChapterAction();

    auto it = m_buildings.find(bid);
    if (it == m_buildings.cend()) { throw std::runtime_error("BuildingExpansionVisualDebug: Removed an unexistent building."); }

    Building const removed_building = it->second;

    m_buildings.erase(it);

    m_custom_changes.back().record_buildingRemoval(bid, removed_building);
}

void BuildingExpansionVisualDebug::custom_goToPreviousStep(int const change_id)
{
    auto const ch = m_custom_changes[change_id];

    for (auto const& [bid, building] : ch.added_buildings())
    {
        if (m_buildings.erase(bid) < 1) { throw std::runtime_error("BuildingExpansionVisualDebug: Cannot remove an added building when going to the previous step."); }
    }

    for (auto const& [bid, building] : ch.removed_buildings())
    {
        auto const [it, success] = m_buildings.insert({ bid, building });
        if (!success) { throw std::runtime_error("BuildingExpansionVisualDebug: Cannot add a removed building when going to the previous step."); }
    }
}

void BuildingExpansionVisualDebug::custom_goToNextStep(int const change_id)
{
    auto const ch = m_custom_changes[change_id];

    for (auto const& [bid, building] : ch.removed_buildings())
    {
        if (m_buildings.erase(bid) < 1) { throw std::runtime_error("BuildingExpansionVisualDebug: Cannot remove a removed building when going to the next step."); }
    }

    for (auto const& [bid, building] : ch.added_buildings())
    {
        auto const [it, success] = m_buildings.insert({ bid, building });
        if (!success) { throw std::runtime_error("BuildingExpansionVisualDebug: Cannot add an added building when going to the next step."); }
    }
}

void BuildingExpansionVisualDebug::custom_pushVertices(DebugVertices & vertices) const
{
    //drawing building areas
    for (auto const& [bid, building] : m_buildings)
    {
        for (auto const& [aid, area] : building.areas_by_ref())
        {
            auto const vol = area.volume();
            if (vol.down == cursor_pos().z)
            {
                // Draw borders
                auto const borders_color = Color{ 102, 153, 255 };
                for (int y = vol.left; y <= vol.right(); ++y)
                {
                    auto const x_top = vol.behind;
                    vertices.push_tile(x_top, y, 1.f, borders_color);

                    auto const x_bottom = vol.front();
                    vertices.push_tile(x_bottom, y, 1.f, borders_color);
                }
                for (int x = vol.behind + 1; x <= vol.front() - 1; ++x) // excluding the already colored corners tile
                {
                    auto const y_left = vol.left;
                    vertices.push_tile(x, y_left, 1.f, borders_color);

                    auto const y_right = vol.right();
                    vertices.push_tile(x, y_right, 1.f, borders_color);
                }

                // Draw inner area
                auto const inner_color = area_templates.at(area.type()).debug_color();
                for (int x = vol.behind + 1; x <= vol.front() - 1; ++x)
                {
                    for (int y = vol.left + 1; y <= vol.right() - 1; ++y)
                    {
                        vertices.push_tile(x, y, 1.f, inner_color);
                    }
                }
            }
        }
    }
}



namespace BuildingExpansionVisualDebugTests
{
    void test_buildingSynchronization()
    {
        // Mock buildings
        auto const fake_bid1 = BuildingId{ 36 };
        auto const fake_bid2 = BuildingId{ 54 };

        auto const mock_buildingExpansionTemplate = std::unordered_map<AreaType, AreaExpansionTemplate>{
                { AreaType::field, AreaExpansionTemplate{ {}, {} } },
            };

        auto old_building = Building{ sim_settings.map.test_farm_expId, 1, 1 };
        old_building.create_area(AreaType::field, { 0, 0, 0 }, { 10, 10 }, mock_buildingExpansionTemplate);

        auto new_building = Building{ sim_settings.map.test_farm_expId, 1, 1 };
        auto const& [fake_aid, fake_area] = new_building.create_area(AreaType::field, { 1, 0, 0 }, { 10, 10 }, mock_buildingExpansionTemplate);

        auto updated_newBuilding = new_building;
        updated_newBuilding.remove_area(fake_aid);
        updated_newBuilding.create_area(AreaType::field, { 2, 0, 0 }, { 10, 10 }, mock_buildingExpansionTemplate);


        // -- Mock map buildings
        DataArray<Building> mock_buildings{ 10u };


        // Initialization
        BuildingExpansionVisualDebug BEVD;
        BEVD.start(100, 150, 50, mock_buildings);


        // Add a fake building to test functionalities in the next chapters:
        BEVD.begin_chapter("TEST - added an original building");
        BEVD.add_building(fake_bid1, old_building);
        BEVD.end_chapter();


        //--- Actual tests
        static auto test_id = 0;
        ++test_id;
        if (test_id > 8) { test_id = 1; }

        g_log << "\nBuildingExpansionVisualDebug test #" << test_id << ": " << std::endl;

        BEVD.begin_chapter("TEST - actual tests");

        switch (test_id)
        {
            case 0: // No test 0
                break;

            case 1:
                g_log << "ADDITION TEST. Must fail (because a building with the same BuildingId has already been added) -> ";
                try 
                {
                    BEVD.add_building(fake_bid1, new_building);
                    g_log << "ERROR (the code didn't throw any exception).";
                }
                catch (std::exception const& e) 
                {
                    g_log << "OK (" << e.what() << ")";
                }
                break;

            case 2:
                g_log << "REMOVAL TEST. Must allow the removal of old_buildind, allow the the addition of new_building and allow the removal of new_building -> ";
                try 
                {
                    BEVD.remove_building(fake_bid1);
                    BEVD.add_building(fake_bid1, new_building);
                    BEVD.remove_building(fake_bid1);
                    g_log << "OK.";
                } 
                catch (std::exception const& e) 
                {
                    g_log << "ERROR (the code did throw an exception: " << e.what() << ")";
                }
                break;

            case 3:
                g_log << "REMOVAL TEST. Must fail (because cannot remove an already removed building) -> ";
                try 
                {
                    BEVD.remove_building(fake_bid1);
                    BEVD.remove_building(fake_bid1);
                    g_log << "ERROR (the code didn't throw any exception).";
                } 
                catch (std::exception const& e) 
                {
                    g_log << "OK (" << e.what() << ")";
                }
                break;

            case 4:
                g_log << "REMOVAL TEST. Must allow the removal of the newly added building -> ";
                try 
                {
                    BEVD.add_building(fake_bid2, new_building);
                    BEVD.remove_building(fake_bid2);
                    g_log << "OK.";
                } 
                catch (std::exception const& e) 
                {
                    g_log << "ERROR (the code did throw an exception: " << e.what() << ")";
                }
                break;

            case 5:
                g_log << "REMOVAL TEST. Must allow the removal of the old building -> ";
                try 
                {
                    BEVD.remove_building(fake_bid1);
                    g_log << "OK.";
                } 
                catch (std::exception const& e) 
                {
                    g_log << "ERROR (the code did throw an exception: " << e.what() << ")";
                }
                break;

            case 6:
                g_log << "UPDATING TEST. Must allow the replacement of new_building with updated_newBuilding -> ";
                try 
                {
                    BEVD.remove_building(fake_bid1);
                    BEVD.add_building(fake_bid1, new_building);
                    BEVD.update_building(fake_bid1, updated_newBuilding);
                    g_log << "OK.";
                } 
                catch (std::exception const& e) 
                {
                    g_log << "ERROR (the code did throw an exception: " << e.what() << ")";
                }
                break;

            case 7:
                g_log << "UPDATING TEST. Must fail (because there is no building to update) -> ";
                try
                {
                    BEVD.remove_building(fake_bid1);
                    BEVD.update_building(fake_bid1, new_building);
                    g_log << "ERROR (the code didn't throw any exception).";
                } 
                catch (std::exception const& e) 
                {
                    g_log << "OK (" << e.what() << ")";
                }
                break;

            case 8:
                g_log << "UPDATING TEST. Must allow the replacement of new_building with updated_newBuilding -> ";
                try
                {
                    BEVD.add_building(fake_bid2, new_building);
                    BEVD.update_building(fake_bid2, updated_newBuilding);
                    g_log << "OK.";
                } 
                catch (std::exception const& e) 
                {
                    g_log << "ERROR (the code did throw an exception: " << e.what() << ")";
                }
                break;

            case 9:
                g_log << "UPDATING TEST. Must allow the replacement of old_building with new_building -> ";
                try
                {
                    BEVD.update_building(fake_bid1, new_building);
                    g_log << "OK.";
                } 
                catch (std::exception const& e) 
                {
                    g_log << "ERROR (the code did throw an exception: " << e.what() << ")";
                }
                break;
        }

        g_log << "\n" << std::endl;

                
        BEVD.end_chapter();

        BEVD.stop();
    }
}



} //namespace tgm


#endif //BUILDEXP_VISUALDEBUG