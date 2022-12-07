#ifndef GM_GAMEMAP_H
#define GM_GAMEMAP_H


#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "characters/mobile.h"
#include "data_strctures/data_array.hh"
#include "direction.h"
#include "door_manager.hh"
#include "graphics/dynamic_manager.hh"
#include "graphics/free_triangle_vertices.hh"
#include "graphics/tile_vertices.hh"
#include "io/flatbuffers/gamemap_generated.h"
#include "map_graph.h"
#include "map/buildings/building.hh"
#include "map/buildings/building_manager.hh"
#include "map/tiles/tile.hh"
#include "map/tiles/tile_set.hh"
#include "mediators/queues/door_ev.hh"
#include "mediators/queues/gui_ev.hh"
#include "mediators/queues/mobile_ev.hh"
#include "mediators/queues/player_ev.hh"
#include "objects/door.hh"
#include "settings/graphics_settings.hh"
#include "systems/mobile_manager.h"
#include "systems/player_manager.hh"


namespace tgm
{



class GameMap
{
	public:
		////
		//	Create an unitialized Map.
		////
		GameMap(unsigned seed, DynamicManager & dynamic_manager, Camera & camera, 
				TileGraphicsMediator & tgraphics_mediator, RoofGraphicsMediator & rg_mediator, AudioManager & audio_manager, GuiEventQueues & gui_events);
		~GameMap();
	
		GameMap(const GameMap&) = delete;
		GameMap& operator=(const GameMap&) = delete;


		template<typename T, typename ...Args>
		void new_input(Args&& ...args) { m_input_events.push<T>(std::forward<Args>(args)...); }

		template<typename T, typename ...Args>
		void new_mobileEvent(Args&& ...args) { m_mobile_events.push<T>(std::forward<Args>(args)...); }

		void update();

	

		auto const& tiles() const { return m_tiles; }
		auto const& building_manager() const { return m_building_manager; }
		auto debug_get_buildings() -> DataArray<Building> const& { return m_building_manager.debug_get_buildings(); } //TODO: 99: Strano giro di passaggi. I buildings sono qui.

		auto debug_getTile(const Vector3i& coos) const noexcept -> Tile const* { return m_tiles.get(coos.x, coos.y, coos.z); }
		auto debug_getBlock(CityBlockId const cbid) const noexcept -> CityBlock const*const { return m_building_manager.debug_getBlock(cbid); }
		auto debug_build_prefabBuilding(PrefabBuilding const& prefab) -> std::pair<BuildingId, Building const*> { return m_building_manager.debug_build_prefabBuilding(prefab); }
		void debug_remove_building(BuildingId const bid) { m_building_manager.unbuild_building(bid); };
		void debug_request_buildingExpansion(BuildingId const bid) { m_building_manager.request_buildingExpansion(bid); }
		void debug_expand_buildings() { m_building_manager.expand_buildings(); }
		void debug_expand_random_building() { m_building_manager.debug_expand_random_building(); }
		auto debug_buildBuilding_inNearestCity(BuildingRecipe const& recipe) -> std::optional<BuildingId> 
		{ 
			return m_building_manager.buildBuilding_inNearestCity(recipe); 
		};

		auto debug_getPlayerPosition_inTiles() const noexcept -> Vector3i
		{
			return player_manager.debug_getPlayerPosition_inTiles();
		}

		auto debug_getPlayerManager() const noexcept -> PlayerManager const& { return player_manager; }

		void debug_compareMoveAlgorithms() const;

		void debug_interactWithAllDoors() { m_door_events.push<DebugInteractWithAllDoorsEv>(); }

		//TODO: 01: This function should be refactored in order to insert it in the BuildingAlgorithm, assigning to each border the proper 
		//		   BuildingAreaId, BuildingId, CityBlockId, CityId
		//TODO: 01: This function should be refactored in order to to use TileGraphicsMediator to record the changed tile
		void debug_buildBorder(Vector3i const tile_pos)
		{
			auto const t = m_tiles.get(tile_pos);

			if (t)
			{
				if (t->is_border())
				{
					debug_buildBorder(tile_pos + Vector3i{ 0, 0, 1 });
				}
				else if (!t->is_built())
				{
					m_tiles.build_border(tile_pos.x, tile_pos.y, tile_pos.z, 0, 1, 1, BorderStyle::brickWall); //TODO: 01: Aggiungi il giusto CityBlockId
					//m_tiles.compute_tileGraphics(tile_pos.x, tile_pos.y, tile_pos.z);  //TODO: 01: Use the TileGraphicsMediator to record the changed tile
				}
			}
		}

		void debug_createDestroy_door(Vector3i const tile_pos) { m_building_manager.debug_createDestroy_door(tile_pos); }

		void debug_is_area_buildable(CityBlockId const cbid, BuildingId const bid, Building const& building,
									 Vector3i const position, Vector2i const dims,
									 std::vector<BuildingAreaCompleteId> const& replaceable_areas) 
		{
			m_building_manager.debug_is_area_buildable(cbid, bid, building, position, dims, replaceable_areas);
		}


		
		auto write(flatbuffers::FlatBufferBuilder & fbb) const -> flatbuffers::Offset<tgmschema::GameMap>;
		void read(tgmschema::GameMap const*const ts);


	private:

		std::mt19937 m_random_generator;

		TileSet m_tiles;
	
		DataArray<Building> m_buildings{ sim_settings.map.max_buildingCount };

		DoorEventQueues m_door_events;

		DataArray<Door> m_doors{ compute_maxDoorCount() }; 

		PlayerEventQueues m_input_events;
		MobileEventQueues m_mobile_events;

		MobileBody m_player_body; //physics and style
		DataArray<MobileBody> m_npc_bodies{ sim_settings.map.max_npcCount };


		// Initialized in constructor because they requires external parameters
		PlayerManager player_manager;
		MobileManager mobile_manager;
		DoorManager door_manager;
		BuildingManager m_building_manager;

		TileGraphicsMediator & m_tgraphics_mediator;
		GuiEventQueues & m_gui_events;

		
		////
		//  Estimate of the maximum number of doors in the map.
		////
		auto compute_maxDoorCount() const noexcept -> DataArray<Door>::size_type { return static_cast<DataArray<Door>::size_type>(m_tiles.length()) * m_tiles.width() * m_tiles.height() / 25; }

	
	friend auto operator<<(std::ofstream & ofs, GameMap const& map) -> std::ofstream &;
	friend auto operator>>(std::ifstream & ifs, GameMap & map) -> std::ifstream &;

	//\\  				 Debug					//\\

	friend void allocatedMemory_forecast();//DEBUG
	friend class BuildingExpansionVisualDebug;//DEBUG
};



} //namespace tgm


#endif // GM_GAMEMAP_H