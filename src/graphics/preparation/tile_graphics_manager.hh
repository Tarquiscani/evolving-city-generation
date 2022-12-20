#ifndef GM_TILE_GRAPHICS_MANAGER_HH
#define GM_TILE_GRAPHICS_MANAGER_HH


#include <queue>

#include "graphics/tile_vertices.hh"
#include "mediators/tile_graphics_mediator.hh"
#include "map/gamemap.h"

#include "debug/asserts.hh"


namespace tgm
{



class TileGraphicsManager
{
    public:
        TileGraphicsManager(TileGraphicsMediator & m, TileVertices & tv) : m_mediator{ m }, m_tile_vertices{ tv } {}
        TileGraphicsManager(TileGraphicsManager const&) = delete;
        auto operator=(TileGraphicsManager const&) -> TileGraphicsManager & = delete;

        void prepare(GameMap const& simulation);

    private:
        TileGraphicsMediator & m_mediator;
        TileVertices & m_tile_vertices;

        void compute_tileGraphics(TileSet const& tiles, int const x, int const y, int const z, bool const recursive_on_neighbors = true);

        static auto compute_border_type(int const neighbor_signature, int const shadow_neighbor_signature) -> BorderType;
        static void computeTileGraphics_error(bool const is_N_border, bool const is_E_border, bool const is_S_border, bool const is_W_border);
};



} //namespace tgm


#endif //GM_TILE_GRAPHICS_MANAGER_HH