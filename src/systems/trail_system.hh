#ifndef GM_TRAIL_SYSTEM_HH
#define GM_TRAIL_SYSTEM_HH


#include <unordered_set>

#include "map/direction.h"
#include "map/tiles/tile_set.hh"
#include "system/parallelepiped.hh"

#include "settings/debug/debug_settings.hh"


namespace tgm
{



namespace TrailSystem
{
	////
	//	Computes the collision of @orig_square with the other elements of the map, giving back the nearest possible position
	//	to a possible obstacle.
	//
	//	@orig_square: (in units -- map reference system).
	//	@velocity: Distance covered in a single step (in units/frame -- map reference system).
	//	@return: (1) The nearest position towards a possibly found obstacle or the expected final position of the movement (in units -- map reference system). 
	//			 (2) A vector containinig the ids of all the closed doors found along the trail.
	////
	auto compute_nearestPosition(FloatRect const orig_square, int const z_pos, Direction const drc, float const velocity, TileSet const& tiles)
		-> std::pair< FloatRect, std::vector<DoorId> >;


	////
	//	Brute collision algorithm that makes a check moving feet_dim pixel by pixel. 
	//	Used as touchstone to test if the refined algorithm properly works.
	////
	auto debug_brute_computeNearestPosition(FloatRect const orig_square, int const z_pos, Direction const drc, float const velocity, TileSet const& tiles) -> FloatRect;
	

	////
	//	Run a comparison between the two different versions of trail algorithm with different sizes of feet_square and 
	//	different velocities (the output is showed in console).
	//	@x, @y: Starting position for the comparison (in units - map reference system). 
	//	@z_pos: (in tiles - map reference system)
	////
	void debug_compareMoveAlgorithms(float const x, float const y, int const z_floor, Direction const drc, TileSet const& tiles);


	inline void debug_compareMoveAlgorithms() 
	{ 
		DoorEventQueues door_evcont_mock;
		debug_compareMoveAlgorithms(50.f, 50.f, 23, Direction::SE, TileSet{ 100, 150, 50, door_evcont_mock });
	}
	
	auto compute_tilesFromVolume(FloatRect const volume_base, int const z_pos) -> std::vector<Vector3i>;
};



} // namespace tgm


#endif //GM_TRAIL_SYSTEM_HH