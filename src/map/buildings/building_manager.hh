#ifndef GM_BUILDING_MANAGER_HH
#define GM_BUILDING_MANAGER_HH


#include "std_extensions/hash_functions.hh"
#include "data_strctures/data_array.hh"
#include "mediators/tile_graphics_mediator.hh"
#include "mediators/roof_graphics_mediator.hh"
#include "map/map_forward_decl.hh"
#include "map/buildings/building.hh"
#include "map/buildings/building_recipe.hh"
#include "map/buildings/prefab_building.hh"
#include "map/buildings/roof.hh"
#include "map/buildings/block_outline.hh"
#include "map/tiles/tile_set.hh"
#include "map/door_manager.hh"
#include "map/city.hh"
#include "map/city_block.hh"


namespace tgm
{
	


struct DoorablePosition
{
	Vector3i pos;
	BuildingAreaCompleteId area1;
	BuildingAreaCompleteId area2;
	bool vertical;
};



class BuildingManager
{
	public:
		BuildingManager(std::mt19937 & random_generator, TileSet & tiles, DataArray<Building> & buildings, DoorManager & door_manager, 
						TileGraphicsMediator & tg_mediator, RoofGraphicsMediator & rg_mediator);
		~BuildingManager();


		auto const& roofs() const { return m_roofs; }


		void unbuild_building(BuildingId const id);

		void request_buildingExpansion(BuildingId const id);
		void expand_buildings();
		bool expand_building(BuildingId const id, int const queue_id);

		auto buildBuilding_inNearestCity(BuildingRecipe const& recipe) -> BuildingId;

		

		auto debug_getBlock(CityBlockId const cbid) const noexcept -> CityBlock const*const { return cbid == 0 ? nullptr : m_blocks.weak_get(cbid); }

		////
		//  Add @building to the nearest city in the GameMap.
		//	@return: (1) "0" in case of failure. (2) The new BuildingId generated for the building in case of success.
		////
		auto debug_build_prefabBuilding(PrefabBuilding const& building) -> std::pair<BuildingId, Building const*>;
		auto debug_get_buildings() -> DataArray<Building> const& { return m_buildings; }
		
		void debug_createDestroy_door(Vector3i const tile_pos);

		void debug_is_area_buildable(CityBlockId const cbid, BuildingId const bid, Building const& building,
									 Vector3i const position, Vector2i const dims,
									 std::vector<BuildingAreaCompleteId> const& replaceable_areas) { is_area_buildable(cbid, bid, &building, position, dims, replaceable_areas); }

		void debug_expand_random_building();

	private:
		std::mt19937 & m_random_generator;
		TileSet & m_tiles;

		DataArray<City> m_cities{ sim_settings.map.max_cityCount };
		DataArray<CityBlock, true> m_blocks{ sim_settings.map.max_blockCount };
		
		DataArray<Building> & m_buildings;
		DoorManager & m_door_manager;
		DataArray<Roof> m_roofs{ sim_settings.map.max_roofCount };

		TileGraphicsMediator & m_tgraphics_mediator;
		RoofGraphicsMediator & m_rgraphics_mediator;

		static int const max_buildingExpansions = 100;
		std::queue<BuildingId> buildingExpansion_queue;
		std::unordered_set<BuildingId> m_unexpandable_buildings;
		
		//TODO: 12: Gli expansion template verranno caricati da file, ma come verranno scelti gli id? Come faranno ad essere uguali su ogni computer, nonostante 
		//			 la diversità dei file di caricamento? Forse più che un id numerico è meglio una stringa. La soluzione forse è salvare anche questi templates su file,
		//			 in modo che siano associati al file di salvataggio e non a un file esterno.
		std::map<std::string, BuildingExpansionTemplateId> buildingExpansionTemplates_nameToId =
			{
				// id = 0 reserved
				{"farm", sim_settings.map.test_farm_expId},
				{"always_replace", sim_settings.map.test_alwaysReplace_expId}
			};

		std::map<BuildingExpansionTemplateId, std::unordered_map<AreaType, AreaExpansionTemplate>> building_expansionTemplates =
			{
				{
					sim_settings.map.test_farm_expId, //ID
					{
						//TODO: Bug in cui c'era ancora posto per alcune aree piccole ma non veniva riempito. (c'erano 4 diversi Building che si espandevano).
						//		Ho scoperto poi che non era un vero bug, in realtà il problema è che cowshed ha come prerequisito AreaType::Field, ma verso la fine
						//		dell'espansione tutti i Field sono stati upgradati a AreaType::super_field. Quindi Cowshed non viene più presa in considerazione
						//		come candidate area.
						//		Il problema è che non è possibile specificare requisiti alternativi (o... oppure...). I prerequisiti o ci stanno tutti oppure niente.
						//		Devo fare in modo da rendere più libero ed esplicito l'albero dei potenziamenti. Per come è fatto adesso è troppo poco flessibile e poco chiaro.
						{ AreaType::field,			AreaExpansionTemplate{ {},										{}					} },
						{ AreaType::farmyard,		AreaExpansionTemplate{ {},										{}					} },
						{ AreaType::super_field,	AreaExpansionTemplate{ {AreaType::field},						{AreaType::field}	} },
						{ AreaType::cowshed,		AreaExpansionTemplate{ {AreaType::field},						{}					} },
						{ AreaType::large_cowshed,	AreaExpansionTemplate{ {AreaType::field, AreaType::cowshed},	{AreaType::cowshed} } },
						{ AreaType::test,			AreaExpansionTemplate{ {},										{}					} }
					}
				},

				{
					sim_settings.map.test_alwaysReplace_expId, //ID
					{
						{ AreaType::field,			AreaExpansionTemplate{ {AreaType::large_cowshed},				{}					} },	//unbuildable since there's no large_cowshed
						{ AreaType::super_field,	AreaExpansionTemplate{ {AreaType::field},						{AreaType::field}	} },
						{ AreaType::cowshed,		AreaExpansionTemplate{ {AreaType::large_cowshed},				{}					} },	//unbuildable since there's no large_cowshed
					}
				}
			};


		auto get_buildingExpansionTemplate(std::string const& name) -> std::unordered_map<AreaType, AreaExpansionTemplate> const&
		{
			return building_expansionTemplates.at(buildingExpansionTemplates_nameToId.at(name));
		}


		
		auto get_area(BuildingAreaCompleteId const acid) const -> BuildingArea const&
		{
			auto const& b = m_buildings.get_or_throw(acid.bid);
			auto const& a = b.getOrThrow_area(acid.aid);

			return a;
		}


		auto build_firstBuilding_inCity(CityId const cid, CityBlockId const cbid, 
										Vector3i const pos, BuildingRecipe const& recipe, 
										std::vector<BuildingAreaCompleteId> const& replaceable_areas,
										CityBlock & cblock) 
			-> BuildingId;
		
		auto get_nearestCity(Vector2f const pos) -> CityId;
		
		auto compute_cityCenter(City const& city) const -> Vector2f;

		auto build_building_inCity(BuildingRecipe const& recipe, CityId const cid) -> BuildingId;
		


		////////

		//	Algorithm that compute the right position for a new area, depending on the situation.

		////////

		bool is_buildingExpansion_possible(CityBlock const& cblock,
										   BuildingId const bid, Building const& building,
										   std::vector<AreaType> const& candidate_areas,
										   Vector3i & best_position,
										   AreaType & selected_area,
										   std::vector<BuildingAreaCompleteId> & replaced_areas);
		

		bool is_blockExpansion_possible(CityBlockId const cbid,
										CityBlock const& block,
										BuildingRecipe const& recipe,
										std::vector<BuildingAreaCompleteId> const& replaceable_areas,
										Vector3i& best_position,
										std::vector<BuildingAreaCompleteId>& replaced_areas) const;
		
		auto compute_suitablePositions_inBlock(CityBlock const& block, 
											   Vector2i const area_dims,
											   std::vector<BuildingAreaCompleteId> const& replaceable_areas) const
			-> std::unordered_set<Vector3i>;

		
		bool is_cityExpansion_possible(City const& city,
									   CityBlockId const cbid,
									   BuildingRecipe const& recipe,
									   std::vector<BuildingAreaCompleteId> const& replaceable_areas,
									   Vector3i & best_position,
									   std::vector<BuildingAreaCompleteId> & replaced_areas) const;
		
		auto order_cityBlocks(City const& city) const -> std::map<float, CityBlock const*>;
		
		////
		//  Compute the suitable positions around @building, i.e. the tiles around each area where the construction of the new area is plausible. 
		//	Neither it takes into account if that tile exists nor if it's buildable. See documentation for further details. 
		//	There are three possible cases, depending on how @is_expansion and @is_newBlock are set:
		//  (1) If the new area is the first area of a new block, then the suitable positions are those that lies one road away from the border of the area.
		//	(2) If the new area is a new area of an existing block, then the suitable positions are those in which the new area would share at least one border 
		//		with the other areas of the block.
		//	(3) If the new area is a new area of an existing building, then the suitable positions are those in which the new area would share at least three 
		//		borders with the other areas of the building (so that a door can be built).
		////
		static void compute_suitablePositions_aroundBuilding(BuildingId const bid, Building const& building,
															 Vector2i const area_dims,
															 bool const is_expansion,
															 bool const is_newBlock,
															 std::vector<BuildingAreaCompleteId> const& replaceable_areas,
															 std::unordered_set<Vector3i> & suitable_positions);
		


		struct BuildablePosition
		{
			BuildablePosition(Vector3i const a_pos, std::unordered_set<BuildingAreaCompleteId> a_replaced_areas) : 
				pos(a_pos), replaced_areas(a_replaced_areas) {}

			Vector3i pos;
			std::unordered_set<BuildingAreaCompleteId> replaced_areas;
		};
		
		////
		//	For each @suitable_positions check if an area of @area_dims could be built there (taking into account also the @replaceable_areas). 
		////
		auto compute_buildablePositions(CityBlockId const cbid,
										std::unordered_set<Vector3i> const& suitable_positions,
										Vector2i const area_dims,
										std::vector<BuildingAreaCompleteId> const& replaceable_areas) const -> std::vector<BuildablePosition>;

		auto compute_buildablePositions(CityBlockId const cbid, 
										BuildingId const bid, Building const*const bld,
										std::unordered_set<Vector3i> const& suitable_positions,
										Vector2i const area_dims,
										std::vector<BuildingAreaCompleteId> const& replaceable_areas) const -> std::vector<BuildablePosition>;
		
		////
		//	Check that a BuildingArea belonging to @cbid with dimension @dims is buildable in @position.
		//  @replaceable_areas: Areas that could be replaced by the new area.
		//
		//	@return: A bool indicating if the area is available and a vector indicating which areas must be replaced. 
		//			 Note: when the area isn't free the vector must be discarded
		////
		auto is_area_buildable(CityBlockId const cbid,
							   Vector3i const position, Vector2i const dims, 
							   std::vector<BuildingAreaCompleteId> const& replaceable_areas) const
			-> std::pair<bool, std::unordered_set<BuildingAreaCompleteId>> { return is_area_buildable(cbid, 0, nullptr, position, dims, replaceable_areas); }

		auto is_area_buildable(CityBlockId const cbid,
							   BuildingId const bid, Building const*const building,
							   Vector3i const position, Vector2i const dims, 
							   std::vector<BuildingAreaCompleteId> const& replaceable_areas) const
			-> std::pair<bool, std::unordered_set<BuildingAreaCompleteId>>;

		
		////
		//	Check if the inner area tile is buildable. If it belongs to one of the @replaceable_areas, add that area to the @replaced_areas.
		////
		bool is_tile_buildableWithInnerArea_inBlock(int const x, int const y, int const z,
													std::vector<BuildingAreaCompleteId> const& replaceable_areas,
													std::unordered_set<BuildingAreaCompleteId> & replaced_areas) const;
		
		////
		//	Check if a border can be built on a tile. Take into account the @replaceable_areas and if the border can be only built with
		//	the replacement of an existent area, then add that area do @replaced_areas.
		////
		bool is_tile_buildableWithBorder_inBlock(int const x, int const y, int const z, 
												 std::vector<BuildingAreaCompleteId> const& replaceable_areas,
												 std::unordered_set<BuildingAreaCompleteId> & replaced_areas) const;

		bool is_tile_blockFree(CityBlockId const cbid, int const x, int const y, int const z) const;
		
		////
		//	Check if the areas of the building are all connected (as in a graph). 
		//	Two areas are connected if their intersection is at least 3x1 (or 1x3) tile rectangle. 
		////
		bool is_building_connected(BuildingId const bid, Building const& bld, 
								   IntParallelepiped const& ghost_vol = {},
								   std::unordered_set<BuildingAreaCompleteId> const& replaced_areas = {}) const;

		static bool is_building_subsetOf(BuildingId const bid, Building const& bld, std::unordered_set<BuildingAreaCompleteId> const& replaced_areas);

		void explore_areaNeighbors(BuildingId const bid, Building const& bld,
								   IntParallelepiped const& ghost_vol,
								   std::unordered_set<BuildingAreaCompleteId> const& replaced_areas,
								   IntParallelepiped const& current_vol, std::set<BuildingAreaId>& examined_nodes) const;

		static bool are_areas_connected(IntParallelepiped const lhs, IntParallelepiped const rhs);


		void gather_adjacent_externalDoors(Vector3i const position, Vector2i const dims, 
										   std::unordered_set<Vector3i> & removed_doors) const;

		void gather_externalDoor(Vector3i const pos,
								 std::unordered_set<Vector3i> & removed_doors) const;
		
		void gather_surroundingExternalDoor(Vector3i const pos, bool const vertical,
											std::unordered_set<Vector3i>& removed_doors) const;

		void gather_occluded_externalDoors(IntParallelepiped const& vol,
										   std::unordered_set<BuildingAreaCompleteId> const& replaced_areas,
										   std::unordered_set<Vector3i> & removed_doors) const;

		////
		//	@vol: Volume of the area whose outlines must be computed. The outlines are traced pretending that the area is built, 
		//		  also if the area hasn't been built yet.
		////
		auto compute_areaOutlines(IntParallelepiped const& vol, std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const 
			-> std::vector<BlockOutline>;

		auto is_outlineConcaveStartingAngle(Vector3i const pos, Vector3i const extern_drc,
											std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const
			-> std::optional<Vector3i>;

		bool is_outlineConvexStartingAngle(Vector3i const pos, Vector3i const forward_drc,
										   std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const;
		
		auto compute_outline(OutlinePivot const starting_pivot, BuildingAreaCompleteId const ignored_area) const -> BlockOutline;

		////
		//	Compute the outlines
		////
		auto compute_outlines(std::vector<OutlinePivot> starting_pivots, 
							  IntParallelepiped const& ghost_area,
							  std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const
			-> std::vector<BlockOutline>;

		auto advance_outlinePos(Vector3i const pos, Vector3i const drc, 
								IntParallelepiped const& vol,
								std::unordered_set<BuildingAreaCompleteId> const& replaced_areas,
								BlockOutline & outline) const 
			-> OutlinePivot;
		
		bool is_tile_builtWithUnreplacedArea(Tile const& t, std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const;

		bool is_tile_builtWithUnreplacedArea(Vector3i const pos, std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const;

		////
		//	Choose the best among the @buildable_positions. There are three cases: 
		//  (1) If the new area is the first area of a new block, then the best position is that in which the area, enlarged with roads, 
		//		would be adjacent to the higher number of borders of the other blocks.
		//	(2) If the new area is a new area of an existing block, then the best position is that in which the the area would share the
		//		higher number of borders with the areas of same blocks.
		//	(3) If the new area is a new area of an existing building, then the best position is that in which the the area would share the
		//		higher number of borders with the areas of same building.
		////
		bool compute_bestPosition(Vector2i const area_dims,
								  int const road_dist,
								  std::vector<BuildablePosition> const& buildable_positions, 
								  Vector3i & best_position,
								  std::vector<BuildingAreaCompleteId> & replaced_areas) const;

		//TODO: 04: Fare in modo che un edificio abbia al massimo 2-3 porte sull'esterno e non meno di 1.

		////
		//  Check that not all the areas that share/are ajacent to the border have to be replaced. If that is the case, then the border
		//  can't be counted as a shared border, since those areas will be removed and their borders too.
		////
		static bool is_adjacencyPoint(Tile const& t, std::unordered_set<BuildingAreaCompleteId> const& areas_to_replace);


		
		////////

		//	Actual building, area and roof construction.

		////////

		auto internal_build_building(CityId const cid, CityBlockId const cbid,
									 Vector3i const pos, BuildingRecipe const& recipe, 
									 std::vector<BuildingAreaCompleteId> const& replaced_areas,
									 CityBlock & cblock)
			-> BuildingId;
		
		void internal_expand_building(BuildingId const bid,
									  AreaType const selected_area, Vector3i const best_position,
									  std::vector<BuildingAreaCompleteId> const& replaced_areas,
									  CityBlock& cblock, Building& building);

		////
		//	If the new area will be built in a new building (not created yet), then set @newArea_bid to 0.
		////
		void unbuild_replacedArea(BuildingAreaCompleteId const ra_acid, CityId const newArea_cid, CityBlockId const newArea_cbid, BuildingId const newArea_bid);


		////
		//	Build an area on the tiles covered by its volume.
		//	@bid: Index of the building in GameMap "buildings" vector.
		//	@aid: Index of the area in @building "areas" vector.
		//	@area: Area to build.
		////
		void build_buildingArea(CityBlockId const cbid, BuildingId const bid, BuildingAreaId const aid, BuildingArea const& area, Building & building);
		

		////
		//	@beg, @end, @drc: Beginning, end and direction of the segment to analyze.  
		//	@internal_doorablePoss: Collection of the positions where an internal door (connecting two areas of the same building) can be placed.
		//	@external_doorablePoss: Collection of the positions where an external door (connecting an area to the outside) can be placed.
		////
		void check_borderDoorablePoss(Vector3i const beg, Vector3i const end, Vector3i const drc,
									  std::vector<DoorablePosition> & internal_doorablePoss,
									  std::vector<DoorablePosition> & external_doorablePoss) const;
		
		bool is_verticalDoor(Vector3i const pos) const;

		////
		//	Check whether a door built in @pos would be a blind door.
		//	@ignored_vol: Pretend that this are doesn't exist when tracing the outline related to the door.
		////
		bool is_externalBlindDoor(Vector3i const pos) const { return is_externalBlindDoor(pos, {}); }
		bool is_externalBlindDoor(Vector3i const pos, BuildingAreaCompleteId const ignored_area) const;
		
		////
		//	Given a list of external doorable positions, identify and remove the positions where a door 
		//	would be blind (i.e. would face an internal outline).
		//	@outlines: Collection of outlines used to identify the blind doors.
		//	@poss: Collection of suitable positions for external doors.
		////
		static void remove_blindDoors(std::vector<BlockOutline> const& outlines, std::vector<DoorablePosition> & poss);

		void internal_build_internalDoor(Vector3i const pos, bool const vertical, TileType const tile_style);
		void internal_build_externalDoor(Vector3i const pos, bool const vertical, TileType const tile_style, Building & bldg);
		void internal_build_blindDoor(Vector3i const pos, bool const vertical, TileType const tile_style, Building & bld);

		void reshape_existentRoofs(BuildingId const bid, IntParallelepiped const& volume);

		////
		//	Try to build a roof starting from the provided tile position.
		////
		void try_buildRoof(int const x, int const y, int const z);

		void create_roof(BuildingId const bid, std::unordered_set<Vector3i> const& roofed_poss);

		void unbuild_buildingArea(BuildingId const bid, Building const& building, BuildingAreaId const aid);
		
		////
		//	If the tile can host a door, then a new door of the right type is built.
		////
		void try_build_door(Vector3i const pos);

		void try_unbuild_door(int const x, int const y, int const z);

		
		
		////////

		//	VisualDebug helper functions.

		////////
		
		void visualDebug_highlightOrderedCityBlocks(Vector2f const city_center, std::map<float, CityBlock const*> const& ordered_blocks) const;

		void visualDebug_highlightCityBlock(CityBlock const& cblock, Color const color) const;

		void visualDebug_highlightBuilding(Building const& b, Color const color) const;

		void visualDebug_replaceableAreasStep(std::vector<BuildingAreaCompleteId> const& replaceable_areas) const;
		
		void visualDebug_buildablePositionsStep(Vector2i const area_dims, std::vector<BuildablePosition> const& buildable_positions) const;

		void visualDebug_doorablePositionsStep(std::string const& title, std::vector<DoorablePosition> const& doorable_poss) const;

};



} //namespace tgm


#endif //GM_BUILDING_MANAGER_HH
