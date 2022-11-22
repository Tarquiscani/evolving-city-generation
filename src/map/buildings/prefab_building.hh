#ifndef GM_PREFAB_BUILDING_HH
#define GM_PREFAB_BUILDING_HH


#include "system/vector3.hh"
#include "map/buildings/area_expansion_template.hh"
#include "map/buildings/building_area.hh"

#include "settings/debug/debug_settings.hh"


namespace tgm
{



struct PrefabBuildingArea
{
	AreaType type = AreaType::none;
	IntParallelepiped volume = IntParallelepiped{};

	auto inner_volume() const noexcept -> IntParallelepiped 
	{ 
		return {volume.behind + 1, volume.left + 1, volume.down, volume.length - 2, volume.width - 2, volume.height}; 
	}
};


////
//	This is a possible input for the BuildingManager, it's a rigid prefabricated building that doesn't need to pass 
//	trough all the area placement algorithm logic. Used for debug purpose only.
////
class PrefabBuilding
{
	public:
		PrefabBuilding(std::string const& expansionTemplate_name) : 
			m_expansion_template{ expansionTemplate_name } {}
		
		auto operator=(PrefabBuilding const& rhs) -> PrefabBuilding& = default;

		bool is_empty() const { return m_areas.empty(); }
		auto const& areas() const { return m_areas; }
		auto expansion_template() const { return m_expansion_template; }
		auto const& doors() const { return m_doors; }

		auto compute_volume() const -> IntParallelepiped
		{
			#if DYNAMIC_ASSERTS
				if (is_empty()) { throw std::runtime_error("Cannot compute the volume of an empty PrefabBuilding."); }
			#endif

			auto vol = IntParallelepiped{};

			for (auto const& a : m_areas)
			{
				vol.combine(a.volume);
			}

			return vol;
		}

		void append_area(AreaType const type, Vector3i const pos, Vector2i const dims)
		{
			if(does_area_overlap({pos.x, pos.y, pos.z, dims.x, dims.y, 1})) { throw std::runtime_error("Areas cannot overlap"); }

			m_areas.emplace_back(PrefabBuildingArea{ type, {pos.x, pos.y, pos.z, dims.x, dims.y, 1} });
		}
		
		bool does_area_overlap(IntParallelepiped const vol) const
		{
			// Neither the borders nor the inner area should overlap the inner area of other BuildingAreas.
			for (auto const& a : m_areas)
			{
				if (!vol.intersect(a.inner_volume()).is_null()) { return true; }
			}

			return false;
		}


		void add_door(Vector3i const pos) { m_doors.push_back(pos); }
		

	private:
		std::vector<PrefabBuildingArea> m_areas;
		std::string m_expansion_template;

		std::vector<Vector3i> m_doors;
};



} //namespace tgm


#endif //GM_PREFAB_BUILDING_HH