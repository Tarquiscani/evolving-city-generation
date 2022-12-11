#ifndef GM_BUILDING_AREA_HH
#define GM_BUILDING_AREA_HH


#include "data_strctures/data_array.hh"
#include "map/buildings/area_template.hh"
#include "system/parallelepiped.hh"

#include "debug/logger/log_streams.hh"


namespace tgm
{



class BuildingArea
{
    public:
        BuildingArea() :
            BuildingArea(AreaType::none, Vector3i(0, 0, 0), Vector2i(0, 0)) { }

        BuildingArea(AreaType const type, Vector3i const pos, Vector2i const dimensions) :
            m_type(type), m_volume(pos.x, pos.y, pos.z, dimensions.x, dimensions.y, 1) 
        {
            #if DYNAMIC_ASSERTS
                if (pos.x < 0 || pos.y < 0 || pos.z < 0 || dimensions.x < 0 || dimensions.y < 0) 
                { 
                    throw std::runtime_error("An area cannot have negative position or negative dimensions."); 
                }
            #endif
        }


        auto type() const noexcept -> AreaType { return m_type; }
        auto volume() const noexcept -> IntParallelepiped { return m_volume; }
        auto inner_volume() const noexcept -> IntParallelepiped 
        { 
            return {m_volume.behind + 1, m_volume.left + 1, m_volume.down, m_volume.length - 2, m_volume.width - 2, m_volume.height}; 
        }


        auto get_internalConnections() -> std::vector<std::pair<BuildingAreaId, DoorId>>& { return internal_connections; }


    private:
        //Type of the area. Note that for each building the same value has a different meaning
        AreaType m_type = AreaType::none;
        IntParallelepiped m_volume;
        std::vector<std::pair<BuildingAreaId, DoorId>> internal_connections; //connection to another area of the same building
        std::vector<std::tuple<BuildingId, BuildingAreaId, DoorId>> external_connections;  //connection to another building or to a road


    friend auto operator<<(Logger & lgr, BuildingArea const& ba) -> Logger &;
    friend class BuildingExpansionVisualDebug;
};


inline auto operator<<(Logger & lgr, BuildingArea const& ba) -> Logger &
{
    lgr << "BuildingArea: " << ba.volume();

    return lgr;
}


inline auto operator<<(std::ostream & os, BuildingAreaCompleteId const& acid) -> std::ostream &
{
        os << "{ " << human_did(acid.bid) << ", " << human_did(acid.aid) << " }";

        return os;
}



} // namespace tgm


#endif //GM_BUILDING_AREA_HH