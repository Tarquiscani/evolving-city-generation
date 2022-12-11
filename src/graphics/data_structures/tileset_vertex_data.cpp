#include "tileset_vertex_data.hh"


#include <iomanip>


namespace tgm
{



auto operator<<(Logger & lgr, TilesetVertexData const& tvd) -> Logger &
{
    lgr << "TilesetVertexData{ (" << std::setw(5) << tvd.world_pos[0] << ", " << std::setw(5) << tvd.world_pos[1] << ", " << std::setw(5) << tvd.world_pos[2] << "), ("
        << std::setprecision(6) << std::setw(8) <<  tvd.tex_coords[0] << ", " << std::setprecision(6) << std::setw(8) << tvd.tex_coords[1] << "), "
        << std::setw(6) << tvd.layer << ", "
        << std::setw(6) << tvd.entity_id << " }";

    return lgr;
}



} //namespace tgm