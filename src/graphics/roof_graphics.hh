#ifndef GM_roof_graphics_HH
#define GM_roof_graphics_HH


#include <vector>

#include "map/map_forward_decl.hh"
#include "graphics/free_triangle_vertices.hh"


namespace tgm
{



struct RoofGraphics
{
	std::vector<FreePolygonId> south_polygons;
	std::vector<FreePolygonId> west_polygons;
	std::vector<FreePolygonId> north_polygons;
	std::vector<FreePolygonId> east_polygons;
};



} //namespace tgm


#endif //GM_roof_graphics_HH