#include "free_triangle.hh"


#include "debug/logger/debug_printers.hh"


namespace tgm
{



auto operator<<(Logger & lgr, FreeTriangle const& ft) -> Logger &
{
    lgr << Logger::nltb << "FreeTriangle {"
        << Logger::addt
        << Logger::nltb << "position: " << ft.m_pos
        << Logger::nltb << "v0: " << ft.m_v0
        << Logger::nltb << "v1: " << ft.m_v1
        << Logger::nltb << "v2: " << ft.m_v2
        << Logger::nltb << "entity_id: " << ft.m_entity_id
        << Logger::nltb << "edgeable_id: " << ft.m_edgeable_id
        << Logger::remt
        << Logger::nltb << "}";

    return lgr;
}



} //namespace tgm