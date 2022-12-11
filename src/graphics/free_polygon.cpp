#include "free_polygon.hh"


#include "debug/logger/debug_printers.hh"


namespace tgm
{



auto operator<<(Logger & lgr, FreePolygon const& fp) -> Logger &
{
    lgr << "FreePolygon{"
        << Logger::addt
        << Logger::nltb << "pos: " << fp.m_pos
        << Logger::nltb << "vertices: " << fp.m_vertices
        << Logger::remt
        << Logger::nltb << "}";

    return lgr;
}



} //namespace tgm