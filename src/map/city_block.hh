#ifndef GM_CITY_BLOCK_HH
#define GM_CITY_BLOCK_HH


#include <set>

#include "settings/simulation/simulation_settings.hh"
#include "system/parallelepiped.hh"
#include "system/vector2.hh"

#include "settings/debug/debug_settings.hh"


namespace tgm
{



class CityBlock
{
    public:
        auto center() const -> Vector2f { return m_center; }

        bool empty() const { return m_buildings.empty(); }
        bool contains(BuildingId const bid) const { return std::find(m_buildings.cbegin(), m_buildings.cend(), bid) != m_buildings.end(); }
        auto const& buildings() const { return m_buildings; }

        bool has_room_for(int const newArea_surface) const { return m_surface + newArea_surface <= sim_settings.map.max_cityBlockSurface; }
        

        void add_building(BuildingId const bid) 
        {
            #if DYNAMIC_ASSERTS
                if (std::find(m_buildings.cbegin(), m_buildings.cend(), bid) != m_buildings.cend()) { throw std::runtime_error("Adding an already added building."); }
            #endif

            m_buildings.push_back(bid);
        }

        void remove_building(BuildingId const bid)
        {
            auto it = std::find(m_buildings.cbegin(), m_buildings.cend(), bid);

            #if DYNAMIC_ASSERTS
                if (it == m_buildings.cend()) { throw std::runtime_error("Removing a never-added building."); }
            #endif

            m_buildings.erase(it);
        }

        void increase_surface(IntParallelepiped const& newArea_vol) 
        {
            int const newArea_surface = newArea_vol.length * newArea_vol.width;

            #if DYNAMIC_ASSERTS
                if (!has_room_for(newArea_surface)) { throw std::runtime_error("Can't increase so much the surface, since it would exceed the maximum."); }
            #endif

            m_surface += newArea_surface; 
            

            // Update the center of the block
            auto const area_center = newArea_vol.center();
            auto const ac = Vector2f(static_cast<float>(area_center.x), static_cast<float>(area_center.y));
            m_center = (m_center * (m_areas_count * 1.f) + ac) / (m_areas_count + 1.f);

            ++m_areas_count;
        }

        void decrease_surface(IntParallelepiped const& removedArea_vol) 
        {
            int const removedArea_surface = removedArea_vol.length * removedArea_vol.width;

            #if DYNAMIC_ASSERTS
                if (m_surface - removedArea_surface < 0) { throw std::runtime_error("Can't decrease the surface below 0."); }
                if (m_areas_count == 0) { throw std::runtime_error("Can't decrease the surface of a block that doesn't contain any area."); }
            #endif

            m_surface -= removedArea_surface;

            
            // Update the center of the block
            auto const area_center = removedArea_vol.center();
            auto const ac = Vector2f(static_cast<float>(area_center.x), static_cast<float>(area_center.y));
            m_center = (m_center * (m_areas_count * 1.f) - ac) / (m_areas_count - 1.f);

            --m_areas_count;
        }


    private:
        std::vector<BuildingId> m_buildings;
        
        int m_surface = 0;
        Vector2f m_center;
        int m_areas_count = 0;
};



} //namespace tgm


#endif //GM_CITY_BLOCK_HH