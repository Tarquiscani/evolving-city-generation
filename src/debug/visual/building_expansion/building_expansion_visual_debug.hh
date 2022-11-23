#ifndef GM_BUILDING_EXPANSION_VISUAL_DEBUG_HH
#define GM_BUILDING_EXPANSION_VISUAL_DEBUG_HH


#include "settings/debug/buildingexpansion_visualdebug.hh"
#if BUILDEXP_VISUALDEBUG



#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <stdexcept>

#include "debug/visual/base/visual_debug.hh"
#include "map/direction.h"
#include "map/tiles/tile_set.hh"
#include "map/buildings/prefab_building.hh"
#include "map/buildings/building.hh"


namespace tgm
{



////
//	Store the original state of the map when BuildingExpansionVisualDebug is started
////
class BuildingExpansionVisualDebugState
{
	public:
		BuildingExpansionVisualDebugState() = default;

		BuildingExpansionVisualDebugState(const std::map<BuildingId, Building>& buildings) :
			m_buildings(buildings) {	}

	private:
		std::map<BuildingId, Building> m_buildings;
};

////
//	Store the changes from a current_state to another of the BuildingExpansionVisualDebug.
////
class BuildingExpansionVisualDebugChange
{
	public:
		BuildingExpansionVisualDebugChange(int const change_id) noexcept:
			m_change_id(change_id) { }

		auto removed_buildings() const noexcept -> std::map<BuildingId, Building> const& { return m_removed_buildings; }
		auto added_buildings() const noexcept -> std::map<BuildingId, Building> const& { return m_added_buildings; }


		void record_buildingAddition(BuildingId const bid, Building const& new_building)
		{
			auto it = m_added_buildings.find(bid);

			if (it != m_added_buildings.cend()) { throw std::runtime_error("BuildingExpansionVisualDebugChange: Cannot add the same building for the second time."); }

			m_added_buildings.insert({ bid, new_building });
		}

		////
		//	Note: The same building could be updated multiple times in the same step. Still the function keep only the first
		//	old_building removed and the last updated_building added.
		////
		void record_buildingUpdate(BuildingId const bid, Building const& old_building, Building const& updated_building)
		{
			auto remIt = m_removed_buildings.find(bid);
			auto addIt = m_added_buildings.find(bid);

			bool const removed = remIt != m_removed_buildings.cend();
			bool const added = addIt != m_added_buildings.cend();

			if (removed && added) //let the previous recorded removal and replace the recorded addition 
			{
				m_added_buildings.insert_or_assign(bid, updated_building);
			}
			else if (removed && !added) //error 
			{
				throw std::runtime_error("BuildingExpansionVisualDebugChange: Cannot update a removed building.");
			}
			else if (!removed && added) //replace the recorded addition 
			{
				m_added_buildings.insert_or_assign(bid, updated_building);
			}
			else if (!removed && !added) //record a removal and record an addition 
			{
				m_removed_buildings.insert({ bid, old_building });
				m_added_buildings.insert({ bid, updated_building });
			}
		}

		////
		//	Note: The same building could be added and removed in the same step. In such a case the function only removes the recorded addition.
		////
		void record_buildingRemoval(BuildingId const bid, Building const& removed_building)
		{
			auto remIt = m_removed_buildings.find(bid);
			auto addIt = m_added_buildings.find(bid);

			bool const removed = remIt != m_removed_buildings.cend();
			bool const added = addIt != m_added_buildings.cend();

			if (removed && added) //delete the recorded addition
			{
				m_added_buildings.erase(addIt);
			}
			else if (removed && !added) //error
			{
				throw std::runtime_error("BuildingExpansionVisualDebugChange: Cannot remove a building for the second time.");
			}
			else if (!removed && added) //delete the recorded addition
			{
				m_added_buildings.erase(addIt);
			}
			else if (!removed && !added) //record the removal
			{
				m_removed_buildings.insert({ bid, removed_building });
			}
		}

	private:
		int m_change_id = 0;
		std::map<BuildingId, Building> m_removed_buildings;
		std::map<BuildingId, Building> m_added_buildings;

	friend auto operator<<(Logger& lgr, const BuildingExpansionVisualDebugChange& bec) -> Logger&;
};

class GameMap;

class BuildingExpansionVisualDebug final : public VisualDebug
{
	public:
		void start(int const map_length, int const map_width, int const map_height, DataArray<Building> const& map_buildings);

		void focus_onBuilding(Building const& bldg) { focus_onVolume(bldg.compute_volume()); }
		void focus_onPrefabBuilding(PrefabBuilding const& prefab) { focus_onVolume(prefab.compute_volume()); }
		void focus_onPosition(Vector2f const pos);

		void add_building(BuildingId const bid, Building const& new_building);
		void update_building(BuildingId const bid, Building const& updated_building);
		void remove_building(BuildingId const bid);

	private:
		BuildingExpansionVisualDebugState m_custom_originalState;

		std::vector<BuildingExpansionVisualDebugChange> m_custom_changes;
		std::map<BuildingId, Building> m_buildings; //buildings in the current state

		void copy_mapBuildings(DataArray<Building> const& map_buildings);
		void init_originalState();

		void focus_onVolume(IntParallelepiped vol);

		virtual bool custom_has_been_activated() const noexcept override { return visualDebug_runtime_openWindowForBuildingExpansion; }
		virtual void custom_deactivate() const noexcept override { visualDebug_runtime_openWindowForBuildingExpansion = false; }

		virtual void init_chapterAlias() override { m_chapter_alias = "expansion"; };

		virtual void custom_stop() override;

		virtual auto window_title() const noexcept -> std::string override { return "Building Expansion Visual Debug"; }
		
		virtual void init_newChange(int const current_st) override;

		virtual void custom_goToPreviousStep(int const change_id) override;
		virtual void custom_goToNextStep(int const change_id) override;


		virtual void custom_pushVertices(DebugVertices & vertices) const override;
};



namespace BuildingExpansionVisualDebugTests
{
	void test_buildingSynchronization();
}



} //namespace tgm
using namespace tgm;


#endif //BUILDEXP_VISUALDEBUG

#endif //GM_BUILDING_EXPANSION_VISUAL_DEBUG_HH