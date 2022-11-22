#ifndef GM_FREE_VERTEX_HH
#define GM_FREE_VERTEX_HH


#include "debug/logger/logger.h"


namespace tgm
{



struct FreeVertex
{
	float x = 0.f,		// Map RS (in units)
		  y = 0.f, 
		  z = 0.f;
	float tex_u = 0.f,  //GIMP-like RS (in texels)
		  tex_v = 0.f;
};

auto operator<<(Logger & lgr, FreeVertex const fv) -> Logger &;



} //namespace tgm
using namespace tgm;


#endif //GM_FREE_VERTEX_HH