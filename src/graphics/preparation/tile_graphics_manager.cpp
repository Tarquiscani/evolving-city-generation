#include "tile_graphics_manager.hh"


namespace tgm
{



void TileGraphicsManager::prepare(GameMap const& simulation)
{
    auto & tiles = simulation.tiles();


    //--- Reset if required
    if (m_tile_vertices.uninitialized() || m_mediator.reset()) 
    {
        auto const length = tiles.length(),
                   width  = tiles.width(),
                   height = tiles.height();

        m_tile_vertices.reset(length, width, height);

        for (auto z = 0; z < height; ++z)				//this iteration order preserves cache locality
        {
            for (auto y = 0; y < width; ++y)
            {
                for (auto x = 0; x < length; ++x)
                {
                    compute_tileGraphics(tiles, x, y, z, false); 
                }
            }
        }

        m_mediator.reset_acquired();
        m_mediator.changes_acquired();  // Every tile has been updated. Moreover there could be old tiles belonging to a different map, possibly a larger one, and their positions may not be valid anymore.
    }
    else
    {
        for (auto const& [pos, involves_border] : m_mediator.changes())
        {
            if (involves_border)
            {
                compute_tileGraphics(tiles, pos.x, pos.y, pos.z);
            }
            else
            {
                auto const& t = tiles.get_existent(pos);
                m_tile_vertices.set_tileGraphics(pos.x, pos.y, pos.z, false, t.get_type(), BorderType::none, BorderStyle::none);
            }
        }

        m_mediator.changes_acquired();
    }

    

    //--- Process debug events
    auto const& debug_tileStyle_change = m_mediator.debug_tileStyle_change();
    if (debug_tileStyle_change) 
    { 
        auto const& [pos, style] = *debug_tileStyle_change;
        if (simulation.tiles().contains(pos))
        {
            m_tile_vertices.set_tileGraphics(pos.x, pos.y, pos.z, false, style, BorderType::none, BorderStyle::none); 
        }
    }
    m_mediator.debug_tileStyleChange_acquired();
}


//TODO: PERFORMANCE: Instead of computing the graphics of a tile from scratch every time, maybe it could be useful to use a branch system, 
//					 in which each time a new border is added, then the near borders will have a new branch attached.
//TODO: 13: Non mi sconfinfera questa funzione. In particolare accedere ai tile vicini sempre e comunque, forse questo può essere evitato.
void TileGraphicsManager::compute_tileGraphics(TileSet const& tiles, int const x, int const y, int const z, bool const recursive_on_neighbors)
{
    auto const& t = tiles.get_existent(x, y, z);
    
    auto const N_tile = tiles.get(x - 1, y    , z);
    auto const E_tile = tiles.get(x    , y + 1, z);
    auto const S_tile = tiles.get(x + 1, y    , z);
    auto const W_tile = tiles.get(x    , y - 1, z);

    if (!t.is_border() || t.is_door())
    {
        m_tile_vertices.set_tileGraphics(x, y, z, false, t.get_type(), BorderType::none, BorderStyle::none);
    }
    else
    {
        bool const is_N_border = N_tile && N_tile->is_border() && !N_tile->is_door(),
                   is_E_border = E_tile && E_tile->is_border() && !E_tile->is_door(),
                   is_S_border = S_tile && S_tile->is_border() && !S_tile->is_door(),
                   is_W_border = W_tile && W_tile->is_border() && !W_tile->is_door();

        auto const border_type = compute_borderType(is_N_border, is_E_border, is_S_border, is_W_border);

        m_tile_vertices.set_tileGraphics(x, y, z, true, TileType::none, border_type, t.border_style());
    }

    if (recursive_on_neighbors)
    {
        if (N_tile && N_tile->is_border()) { compute_tileGraphics(tiles, x - 1, y    , z, false); }
        if (E_tile && E_tile->is_border()) { compute_tileGraphics(tiles, x    , y + 1, z, false); }
        if (S_tile && S_tile->is_border()) { compute_tileGraphics(tiles, x + 1, y    , z, false); }
        if (W_tile && W_tile->is_border()) { compute_tileGraphics(tiles, x    , y - 1, z, false); }
    }
}


auto TileGraphicsManager::compute_borderType(bool const is_N_border, bool const is_E_border, bool const is_S_border, bool const is_W_border) -> BorderType
{
    // solo
    if		(!is_N_border && !is_E_border && !is_S_border && !is_W_border)
    {
        return BorderType::solo;
    }
    // N
    else if ( is_N_border && !is_E_border && !is_S_border && !is_W_border)
    {
        return BorderType::N;
    }
    // E
    else if (!is_N_border &&  is_E_border && !is_S_border && !is_W_border)
    {
        return BorderType::E;
    }
    // S
    else if (!is_N_border && !is_E_border &&  is_S_border && !is_W_border)
    {
        return BorderType::S;
    }
    // W
    else if (!is_N_border && !is_E_border && !is_S_border &&  is_W_border)
    {
        return BorderType::W;
    }
    // NS
    else if (is_N_border && !is_E_border &&  is_S_border && !is_W_border)
    {
        return BorderType::NS;
    }
    // EW
    else if (!is_N_border &&  is_E_border && !is_S_border &&  is_W_border)
    {
        return BorderType::EW;
    }
    // ES
    else if (!is_N_border &&  is_E_border &&  is_S_border && !is_W_border)
    {
        return BorderType::ES;
    }
    // SW
    else if (!is_N_border && !is_E_border &&  is_S_border &&  is_W_border)
    {
        return BorderType::SW;
    }
    // NW
    else if ( is_N_border && !is_E_border && !is_S_border &&  is_W_border)
    {
        return BorderType::NW;
    }
    // NE
    else if ( is_N_border &&  is_E_border && !is_S_border && !is_W_border)
    {
        return BorderType::NE;
    }
    // NES
    else if (is_N_border &&  is_E_border &&  is_S_border && !is_W_border)
    {
        return BorderType::NES;
    }
    // NEW
    else if ( is_N_border &&  is_E_border && !is_S_border &&  is_W_border)
    {
        return BorderType::NEW;
    }
    // NSW
    else if ( is_N_border && !is_E_border &&  is_S_border &&  is_W_border)
    {
        return BorderType::NSW;
    }
    // ESW
    else if (!is_N_border &&  is_E_border &&  is_S_border &&  is_W_border)
    {
        return BorderType::ESW;
    }
    // NESW
    else if ( is_N_border &&  is_E_border &&  is_S_border &&  is_W_border)
    {
        return BorderType::NESW;
    }
    else
    {
        computeTileGraphics_error(is_N_border, is_E_border, is_S_border, is_W_border);
        return BorderType::none;
    }
}

void TileGraphicsManager::computeTileGraphics_error(bool const is_N_border,  bool const is_E_border,   bool const is_S_border,  bool const is_W_border)
{
    g_log << "Unexpected neighbors pattern\n"
          << std::boolalpha
          << "\tis_N_border: " << is_N_border
          << "\tis_E_border: " << is_E_border 
          << "\tis_S_border: " << is_S_border
          << "\tis_W_border: " << is_W_border
          << std::noboolalpha << std::endl;

    throw std::runtime_error("Unexpected neighbors pattern.");
}



} //namespace tgm