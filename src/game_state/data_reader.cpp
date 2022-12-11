#include "data_reader.hh"


#include "settings/game_state_settings.hh"
#include "map/gamemap.h"
#include "utilities/filesystem_utilities.hh"

#include "debug/logger/debug_printers.hh"


namespace tgm
{



auto operator>>(std::ifstream & ifs, GameMap & map) -> std::ifstream &
{
    //TODO: 12: Caricare tutti i dati in map, non solo i tiles
    ifs >> map.m_tiles;
    map.m_tgraphics_mediator.record_reset();

    return ifs;
}


auto operator>>(std::ifstream & ifs, TileSet & ts) -> std::ifstream &
{
    auto length = 0, 
         width = 0, 
         height = 0;

    ifs >> length >> width >> height;

    if (!ifs) { throw std::runtime_error("Error while reading TileSet dimensions."); }
    
    //TODO: 12: Il reset del TileSet va anche ad impattare sui VisualDebug, resettare anche quello.
    ts.reset(length, width, height);

    for (auto z = 0; z < ts.height(); ++z)
    {
        for (auto y = 0; y < ts.width(); ++y)
        {
            for (auto x = 0; x < ts.length(); ++x)
            {
                auto temp = Vector3i{};
                if (!(ifs >> temp)) { throw std::runtime_error("Error while reading Tile position."); }

                ifs >> ts.get_existentMutable(x, y, z);
            }
        }
    }



    return ifs;
}


//TODO: PERFORMANCE: Maybe it's faster to write the enum as an int (sacrificing file readability)
auto operator>>(std::ifstream & ifs, TileType & tt) -> std::ifstream &
{
    auto temp = std::string{};

    if (ifs >> temp)
    {
        if (temp == "none") { tt = TileType::none; }
        else if (temp == "underground") { tt = TileType::underground; }
        else if (temp == "ground") { tt = TileType::ground; }
        else if (temp == "sky") { tt = TileType::sky; }
        else if (temp == "wooden") { tt = TileType::wooden; }
        else if (temp == "grout") { tt = TileType::grout; }
        else { throw std::runtime_error("Unexpected value while reading TileType input."); }
    }
    else
    {
        throw std::runtime_error("Unexpected value while reading TileType input.");
    }

    return ifs;
}

//TODO: PERFORMANCE: Maybe it's faster to write the enum as an int (sacrificing file readability)
auto operator>>(std::ifstream & ifs, BorderStyle & bs) -> std::ifstream &
{
    auto temp = std::string{};

    if (ifs >> temp)
    {
        if (temp == "none") { bs = BorderStyle::none; }
        else if (temp == "brickWall") { bs = BorderStyle::brickWall; }
        else { throw std::runtime_error("Unexpected value while reading BorderStyle input."); }
    }
    else
    {
        throw std::runtime_error("Unexpected value while reading BorderStyle input.");
    }

    return ifs;
}

auto operator>>(std::ifstream & ifs, Tile & t) -> std::ifstream &
{
    ifs >> t.inner_area;
    ifs >> t.m_block;
    ifs >> t.door;
    ifs >> t.door_open;

    ifs >> t.m_borders;
    for (auto i = decltype(t.m_borders){0}; i < t.m_borders; ++i)
    {
        ifs >> t.m_building_infos[i];
    }
    ifs >> t.type;
    ifs >> t.m_border_style;

    ifs >> t.m_roof_count;
    for (auto i = decltype(t.m_roof_count){0}; i < t.m_roof_count; ++i)
    {
        ifs >> t.m_roof_infos[i];
    }

    ifs >> t.m_furniture_id;

    ifs >> t.hosted_mobiles;


    if (!ifs) { throw std::runtime_error("Error while reading Tile input."); }


    return ifs;
}

auto operator>>(std::ifstream & ifs, TileBuildingInfo & tbi) -> std::ifstream &
{
    ifs >> tbi.m_bid >> tbi.m_aid;

    return ifs;
}

auto operator>>(std::ifstream & ifs, RoofInfo & ri) -> std::ifstream &
{
    ifs >> ri.bid >> ri.roof_id;

    return ifs;
}

namespace DataReader
{
    auto generate_loadStream(std::string const& filename) -> std::ifstream
    {
        return FsUtil::open(GStateSet::saves_folder + filename + GStateSet::saves_ext);
    }
} //namespace DataReader



} //namespace tgm