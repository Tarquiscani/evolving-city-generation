#ifndef GM_roof_graphics_mediator_HH
#define GM_roof_graphics_mediator_HH


#include <unordered_set>

#include "map/map_forward_decl.hh"


namespace tgm
{



class RoofGraphicsMediator
{
	public:
		auto const& removed_roofs() { return m_removed_roofs; }
		auto const& added_roofs() { return m_added_roofs; }

		void record_roofRemoval(RoofId const rid) 
		{
			auto it = m_added_roofs.find(rid);
			if (it != m_added_roofs.cend())			//if it were a roof added in this tick, then the GraphicsManager should just ignore it
			{
				m_added_roofs.erase(it);
			}
			else
			{
				m_removed_roofs.insert(rid);
			}
		}
		void record_roofAddition(RoofId const rid) { m_added_roofs.insert(rid); }

		void changes_acquired()
		{
			m_removed_roofs.clear();
			m_added_roofs.clear();
		}

	private:
		std::unordered_set<RoofId> m_removed_roofs;
		std::unordered_set<RoofId> m_added_roofs;
};



} //namespace tgm


#endif //GM_roof_graphics_mediator_HH