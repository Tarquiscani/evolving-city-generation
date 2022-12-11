#include "free_vertex.hh"


#include "debug/logger/debug_printers.hh"


namespace tgm
{



auto operator<<(Logger& lgr, FreeVertex const fv) -> Logger &
{
	lgr << "{ " << fv.x << ", " << fv.y << ", " << fv.z << ", " << fv.tex_u << ", " << fv.tex_v << "}";

	return lgr;
}



} //namespace tgm