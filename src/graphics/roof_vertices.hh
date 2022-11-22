#ifndef GM_ROOF_VERTICES_HH
#define GM_ROOF_VERTICES_HH


#include "free_triangle_vertices.hh"


namespace tgm
{



struct RoofVertices
{
	FreeTriangleVertices south_roof{ 30000u, roof_texture };
	FreeTriangleVertices west_roof{ 30000u, roof_texture };
	FreeTriangleVertices north_roof{ 30000u, roof_texture };
	FreeTriangleVertices east_roof{ 30000u, roof_texture };

	void clear()
	{
		south_roof.clear();
		west_roof.clear();
		north_roof.clear();
		east_roof.clear();
	}
};



} //namespace tgm
using namespace tgm;



#endif //GM_ROOF_VERTICES_HH