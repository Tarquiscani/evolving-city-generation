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
                m_tile_vertices.set_tileGraphics(pos.x, pos.y, pos.z, false, t.get_type(), BorderType{}, BorderStyle::none);
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
            m_tile_vertices.set_tileGraphics(pos.x, pos.y, pos.z, false, style, BorderType{}, BorderStyle::none);
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
        m_tile_vertices.set_tileGraphics(x, y, z, false, t.get_type(), BorderType{}, BorderStyle::none);
    }
    else
    {
        auto neighbor_signature = 0;
        auto shadow_neighbor_signature = 0;
        
        neighbor_signature |= (S_tile && S_tile->is_border() && !S_tile->is_door()) ? 0b00'01'00 : 0;
        neighbor_signature |= (N_tile && N_tile->is_border() && !N_tile->is_door()) ? 0b00'10'00 : 0;
        neighbor_signature |= (E_tile && E_tile->is_border() && !E_tile->is_door()) ? 0b01'00'00 : 0;
        neighbor_signature |= (W_tile && W_tile->is_border() && !W_tile->is_door()) ? 0b10'00'00 : 0;

        auto const border_type = compute_border_type(neighbor_signature, shadow_neighbor_signature);

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


auto TileGraphicsManager::compute_border_type(int const neighbor_signature, int const shadow_neighbor_signature) -> BorderType
{
    auto ret = BorderType{};

    switch (neighbor_signature)
    {
        case 0b00'00'00:
            ret.background = BorderBackgroundType::SideCutTopShortFull;
            ret.section = BorderSectionType::SectionBehindShadowFrontShadowBehind;
            break;
        case 0b00'00'01:
            ret.background = BorderBackgroundType::SideCutTopLongFull;
            ret.section = BorderSectionType::SectionBehindShadowBehind;
            break;
        case 0b00'00'10:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::NoSectionShadowLeftShadowRightShadowFront;
            break;
        case 0b00'00'11:
        case 0b00'10'11:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::NoSectionShadowLeftShadowRight;
            break;
        case 0b00'01'00:
        case 0b00'01'01:
            ret.background = BorderBackgroundType::SideFull;
            ret.section = BorderSectionType::SectionBehindFrontShadowBehind;
            break;
        case 0b00'01'10:
        case 0b00'01'11:
        case 0b00'11'10:
        case 0b00'11'11:
            ret.background = BorderBackgroundType::SideCutBottomFull;
            ret.section = BorderSectionType::SectionFrontShadowLeftShadowRight;
            break;
        case 0b00'10'00:
            ret.background = BorderBackgroundType::SideCutTopShortFull;
            ret.section = BorderSectionType::SectionBehindShadowFront;
            break;
        case 0b00'10'01:
            ret.background = BorderBackgroundType::SideCutTopLongFull;
            ret.section = BorderSectionType::SectionBehind;
            break;
        case 0b00'11'00:
        case 0b00'11'01:
            ret.background = BorderBackgroundType::SideFull;
            ret.section = BorderSectionType::SectionBehindFront;
            break;


        case 0b01'00'00:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::SectionRightShadowFront;
            break;
        case 0b01'00'01:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::SectionRight;
            break;
        case 0b01'00'10:
        case 0b01'10'10:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::NoSectionShadowLeftShadowFront;
            break;
        case 0b01'00'11:
        case 0b01'10'11:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::NoSectionShadowLeft;
            break;
        case 0b01'01'00:
        case 0b01'01'01:
            ret.background = BorderBackgroundType::SideCutBottomRight;
            ret.section = BorderSectionType::SectionFrontRight;
            break;
        case 0b01'01'10:
        case 0b01'01'11:
        case 0b01'11'10:
        case 0b01'11'11:
            ret.background = BorderBackgroundType::SideCutBottomFull;
            ret.section = BorderSectionType::SectionFrontShadowLeft;
            break;
        case 0b01'10'00:
            ret.background = BorderBackgroundType::SideCutTopShortLeft;
            ret.section = BorderSectionType::SectionBehindRightShadowFront;
            break;
        case 0b01'10'01:
            ret.background = BorderBackgroundType::SideCutTopShortLeft;
            ret.section = BorderSectionType::SectionBehindRight;
            break;
        case 0b01'11'00:
            ret.background = BorderBackgroundType::SideCutBottomRight;
            ret.section = BorderSectionType::SectionFrontBehindRight;
            break;


        case 0b10'00'00:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::SectionLeftShadowFront;
            break;
        case 0b10'00'01:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::SectionLeft;
            break;
        case 0b10'00'10:
        case 0b10'10'10:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::NoSectionShadowRightShadowFront;
            break;
        case 0b10'00'11:
        case 0b10'10'11:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::NoSectionShadowRight;
            break;
        case 0b10'01'00:
        case 0b10'01'01:
            ret.background = BorderBackgroundType::SideCutBottomLeft;
            ret.section = BorderSectionType::SectionFrontLeft;
            break;
        case 0b10'01'10:
        case 0b10'01'11:
        case 0b10'11'10:
        case 0b10'11'11:
            ret.background = BorderBackgroundType::SideCutBottomFull;
            ret.section = BorderSectionType::SectionFrontShadowRight;
            break;
        case 0b10'10'00:
        case 0b10'10'01:
            ret.background = BorderBackgroundType::SideCutTopShortRight;
            ret.section = BorderSectionType::SectionBehindLeftShadowFront;
            break;
        case 0b10'11'00:
        case 0b10'11'01:
            ret.background = BorderBackgroundType::SideCutBottomLeft;
            ret.section = BorderSectionType::SectionFrontBehindLeft;
            break;


        case 0b11'00'00:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::SectionRightLeft;
            break;
        case 0b11'00'01:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::SectionFrontRightLeft;
            break;
        case 0b11'00'10:
        case 0b11'10'10:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::NoSectionShadowFront;
            break;
        case 0b11'00'11:
        case 0b11'10'11:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::NoSection;
            break;
        case 0b11'01'00:
        case 0b11'01'01:
            ret.background = BorderBackgroundType::SideCutBottomFull;
            ret.section = BorderSectionType::SectionFrontRightLeft;
            break;
        case 0b11'01'10:
        case 0b11'01'11:
        case 0b11'11'10:
        case 0b11'11'11:
            ret.background = BorderBackgroundType::SideCutBottomFull;
            ret.section = BorderSectionType::SectionFront;
            break;
        case 0b11'10'00:
        case 0b11'10'01:
            ret.background = BorderBackgroundType::NoSides;
            ret.section = BorderSectionType::SectionBehindRightLeftShadowFront;
            break;
        case 0b11'11'00:
        case 0b11'11'01:
            ret.background = BorderBackgroundType::SideCutBottomFull;
            ret.section = BorderSectionType::SectionFrontBehindRightLeft;
            break;
    }

    return ret;
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