#ifndef GM_TILE_HH
#define GM_TILE_HH


#include <array>
#include <bitset>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <flatbuffers/flatbuffers.h>

#include "io/flatbuffers/tile_generated.h"
#include "map/map_forward_decl.hh"
#include "map/direction.h"
#include "settings/graphics_settings.hh"
#include "system/vector2.hh"
#include "system/vector3.hh"

#include "debug/logger/logger.hh"
#include "settings/debug/debug_settings.hh"


namespace tgm
{



enum class TileType
{
    none,
    underground,
    ground,
    sky,
    wooden,
    grout,
};


enum class BorderStyle
{
    none,
    brickWall,
};


class TileBuildingInfo
{
    public:
        auto bid() const -> BuildingId
        {
            assert_nonEmpty();

            return m_bid;
        }
        auto aid() const -> BuildingAreaId
        {
            assert_nonEmpty();

            return m_aid;
        }
        auto acid() const -> BuildingAreaCompleteId
        {
            assert_nonEmpty();

            return { m_bid, m_aid };
        }

        bool is(BuildingId const bid, BuildingAreaId const aid) const noexcept { return m_bid == bid && m_aid == aid; }
        bool is_empty() const noexcept { return m_bid == 0 && m_aid == 0; }
        void set(BuildingId const bid, BuildingAreaId const aid) 
        { 
            if (bid == 0 || aid == 0) { throw std::runtime_error("Cannot set a TileBuildingInfo bid or aid to 0. Use reset() instead."); }

            m_bid = bid; m_aid = aid;
        }

        void reset() noexcept { m_bid = 0; m_aid = 0; }

    private:
        BuildingId m_bid = 0;
        BuildingAreaId m_aid = 0;

        void assert_nonEmpty() const
        {
            if (is_empty()) { throw std::runtime_error("The info is empty."); }
        }

    friend auto operator<<(std::ofstream & ofs, TileBuildingInfo const& tbi) -> std::ofstream &;
    friend auto operator>>(std::ifstream & ifs, TileBuildingInfo & tbi) -> std::ifstream &;
};


struct RoofInfo
{
    BuildingId bid = 0;
    RoofId roof_id = 0;

    friend auto operator<<(std::ofstream & ofs, RoofInfo const& ri) -> std::ofstream &;
    friend auto operator>>(std::ifstream & ifs, RoofInfo & ri) -> std::ifstream &;
};


class Tile
{
    private: 
        // Maximum number of areas that can share the same tile (as a border)
        static auto constexpr max_borders = 4;
        static auto constexpr max_roofs = 4;

    public:
        Tile(const Vector3i& coord, const TileType& typ) :
            coordinates(coord), type(typ) { }

        Tile(const Tile&) = delete;
        Tile& operator=(const Tile&) = delete;

        const Vector3i& get_coordinates() const { return coordinates; }
        void set_coordinates(const Vector3i& new_coordinates) { coordinates = new_coordinates; }

        const TileType &get_type() const noexcept { return type; }
        void set_type(const TileType &new_type) noexcept { type = new_type; }


        auto border_style() const noexcept -> BorderStyle { return m_border_style; }


        void build_innerArea(CityBlockId const cbid, BuildingId const bid, BuildingAreaId const aid, TileType const new_style);
        void unbuild_innerArea(BuildingId const bid, BuildingAreaId const aid);

        ////
        //	Make this Tile the border of a new area.
        ////
        void build_border(CityBlockId const cbid, BuildingId const bid, BuildingAreaId const aid, BorderStyle const style);
        void unbuild_border(BuildingId const bid, BuildingAreaId const aid);

        void build_roof(BuildingId const bid, RoofId const rid);
        void unbuild_roof(BuildingId const bid);

        bool is_built() const noexcept { return is_innerArea() || is_border(); }
        bool is_innerArea() const noexcept { return inner_area; }
        bool is_border() const noexcept { return m_borders > 0; }
        bool is_impassable() const noexcept { return (is_border() && !is_door()) || (is_door() && !door_open); }
        bool is_fullOfBorders() const noexcept { return m_borders == max_borders; }
        auto borders_count() const noexcept -> unsigned { return m_borders; }

        bool is_roofed_for(BuildingId const bid) const;

        bool is_door() const noexcept { return door; }
        bool is_externalDoor() const noexcept { return door && m_borders == 1; }
        auto furniture_id() const noexcept -> DataArrayId { assert_door(); return m_furniture_id; }

        bool does_host_mobiles() const noexcept { return hosted_mobiles > 0; }

        void add_mobile() noexcept { ++hosted_mobiles; }
        void remove_mobile() 
        { 
            #if DYNAMIC_ASSERTS
                if(hosted_mobiles == 0)
                    throw std::runtime_error("This tile already hosted no mobiles.");
            #endif
            --hosted_mobiles; 
        }


        void assert_innerArea() const
        {
            if (is_border())
                throw std::runtime_error("The tile holds a border and can subtend multiple areas.");
            if (!inner_area)
                throw std::runtime_error("The tile doesn't hold an inner area.");
        }
        void assert_border() const
        {
            if (inner_area)
                throw std::runtime_error("The tile holds an inner area.");
            if (!is_border())
                throw std::runtime_error("The tile doesn't hold a border.");
        }
        void assert_built() const
        {
            if (!is_built())
                throw std::runtime_error("The tile isn't built.");
        }
        void assert_door() const
        {
            assert_border();
            if (!is_door()) { throw std::runtime_error("The tile doesn't hold a door."); }
        }

        auto get_innerAreaInfo() const -> TileBuildingInfo
        {
            assert_innerArea();

            return m_building_infos[0];
        }

        auto get_borderInfos() const -> std::array<TileBuildingInfo, max_borders> const&
        {
            assert_border();

            return m_building_infos;
        }

        auto areas() const -> std::vector<BuildingAreaCompleteId>
        {
            assert_built();

            std::vector<BuildingAreaCompleteId> areas; //NRVO

            for (auto const& info : m_building_infos)
            {
                if (!info.is_empty()) { areas.push_back({ info.bid(), info.aid() }); }
            }

            return areas;
        }

        auto roof_infos() const -> std::array<RoofInfo, max_roofs> const& { return m_roof_infos; }


        auto block() const -> CityBlockId { return m_block; }
        
        void build_internalDoor(DoorId const did, TileType const tile_style)
        {
            #if DYNAMIC_ASSERTS
                if (m_borders != 2) { throw std::runtime_error("An internal door can't be built. This isn't a border between two areas"); }
                if (door) { throw std::runtime_error("A door has been already built here."); }
            #endif

            door = true;
            door_open = false;
            m_furniture_id = did;
            type = tile_style;
        }

        void build_externalDoor(DoorId const did, TileType const tile_style)
        {
            #if DYNAMIC_ASSERTS
                if (m_borders != 1) { throw std::runtime_error("An extenral door can't be built. This isn't a border between an area and the outside."); }
                if (door) { throw std::runtime_error("A door has been already built here."); }
            #endif

            door = true;
            door_open = false;
            m_furniture_id = did;
            type = tile_style;
        }

        void unbuild_door()
        {
            #if DYNAMIC_ASSERTS
                assert_door();
            #endif

            door = false;
            door_open = false;
            m_furniture_id = 0;
            type = TileType::ground;
        }

        void open_door()
        {
            assert_door();
            if(door_open)
                throw std::runtime_error("The door is already open");

            door_open = true;
        }

        void close_door()
        {
            assert_door();
            if (!door_open)
                throw std::runtime_error("The door is already closed");

            door_open = false;
        }

        auto write(flatbuffers::FlatBufferBuilder & fbb) const -> flatbuffers::Offset<tgmschema::Tile>;
        void read(tgmschema::Tile const*const t);


    private:
        Vector3i coordinates{ -1, -1, -1 }; //TODO: 01: Forse inutile conservare le coordinate dentro Tile. Per ora è utilizzato solo nel Tile panel.
        unsigned short penetrability = 0;

        bool inner_area = false;
        CityBlockId m_block = 0;
        bool door = false;
        bool door_open = false;
        
        unsigned m_borders = 0;
        std::array<TileBuildingInfo, max_borders> m_building_infos{};
        TileType type = TileType::none; 
        BorderStyle m_border_style = BorderStyle::none;

        unsigned m_roof_count = 0;
        std::array<RoofInfo, max_roofs> m_roof_infos{};
        
        DataArrayId m_furniture_id = 0; // Either the id of the furniture or the id of the door currently hosted on this tile. 

        int hosted_mobiles = 0; //count of the mobiles lying on this tile


        void assert_built_with(BuildingId const bid) const;
        void assert_notRoofed_for(BuildingId const bid) const;

        
    friend auto operator<<(std::ofstream & ofs, Tile const& t) -> std::ofstream &;
    friend auto operator>>(std::ifstream & ifs, Tile & t) -> std::ifstream &;
    friend auto operator<<(Logger & lgr, Tile const& t) -> Logger &;
    friend class TileGui;
    friend class BuildingExpansionVisualDebug;

    //friend void allocatedMemory_forecast();//DEBUG
};



} //namespace tgm


#endif //GM_TILE_HH