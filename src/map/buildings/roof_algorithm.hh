#ifndef GM_roof_algorithm_HH
#define GM_roof_algorithm_HH


#include <unordered_set>

#include "data_strctures/data_array.hh"
#include "map/map_forward_decl.hh"
#include "map/tiles/tile_set.hh"
#include "map/buildings/building.hh"


namespace tgm
{



namespace RoofAlgorithm
{

	auto compute_roofablePositions_fromArea(BuildingAreaCompleteId const starting_area, DataArray<Building> const& buildings, TileSet const& tiles)
		-> std::unordered_set<Vector3i>;
	
	auto compute_roofablePositions_fromTile(BuildingId const bid, Vector3i const starting_pos, DataArray<Building> const& buildings, TileSet const& tiles)
		-> std::unordered_set<Vector3i>;

} //namespace RoofAlgorithm



} //namespace tgm


#endif //GM_roof_algorithm_HH