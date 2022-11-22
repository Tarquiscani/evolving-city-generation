#ifndef GM_AREA_EXPANSION_TEMPLATE_HH
#define GM_AREA_EXPANSION_TEMPLATE_HH


#include "map/buildings/area_template.hh"


namespace tgm
{



using BuildingExpansionTemplateId = std::uint32_t;

////
//	It contains the expansion behavior for a specific type of area in a specific type of building.
////
class AreaExpansionTemplate
{
	public:
		AreaExpansionTemplate(std::vector<AreaType> const& required_areas, std::vector<AreaType> const& replaceable_areas):
			m_required_areas(required_areas), m_replaceable_areas(replaceable_areas) { }


	private:
		std::vector<AreaType> m_required_areas;
		std::vector<AreaType> m_replaceable_areas;

	friend auto operator<<(Logger & lgr, AreaExpansionTemplate const& aet) -> Logger &;
	friend class Building;
};


inline auto operator<<(Logger & lgr, AreaExpansionTemplate const&) -> Logger &
{
	lgr << "AreaExpansionTemplate{}";

	return lgr;
}



} //namespace tgm


#endif //GM_AREA_EXPANSION_TEMPLATE_HH