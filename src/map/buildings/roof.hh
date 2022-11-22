#ifndef GM_ROOF_HH
#define GM_ROOF_HH


#include <vector>

#include "system/vector3.hh"
#include "graphics/free_triangle_vertices.hh"


namespace tgm
{



struct Roof
{
	//TODO: 04: PERFORMANCE: Maybe a std::vector of positions isn't the best way to save roofs. Maybe it's better to save them as a tree of IntRect.
	//						 It would save a lot of memory for sure, but does it also improve performance? One should check throughout the code base
	//						 comparing overhead of each different operation.
	std::vector<Vector3i> roofed_poss;
};



}


#endif //GM_ROOF_HH