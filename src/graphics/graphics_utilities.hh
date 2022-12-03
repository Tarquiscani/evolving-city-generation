#ifndef GM_GRAPHICS_UTILITIES_HH
#define GM_GRAPHICS_UTILITIES_HH


#include <tuple>

#include "dynamic_subimage.hh"
#include "map/direction.h"
#include "system/parallelepiped.hh"
#include "world_parallelepiped.hh"


namespace tgm
{



namespace GraphicsUtilities
{
	////
	//	Starting from the @volume and the @subimage compute the parallelepiped whose diagonal plane is the rectangle of the sprite.
	//	@volume: (in units -- map r.s.)
	//	@drc: Direction towards which the sprite is moving.
	//
	//	@return: (in units -- world space r.s.)
	////
	auto compute_dynamicSprite(FloatParallelepiped const volume, DynamicSubimage const& subimage, float const pixel_dim, Direction const drc, bool const round_to_the_nearest_pixel) -> WorldParallelepiped;
}


namespace GUtil = GraphicsUtilities;



} //namespace tgm


#endif //GM_GRAPHICS_UTILITIES_HH