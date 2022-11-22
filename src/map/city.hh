#ifndef GM_CITY_HH
#define GM_CITY_HH


#include <unordered_set>

#include "system/vector2.hh"
#include "data_strctures/data_array.hh"
#include "map/map_forward_decl.hh"
#include "map/buildings/building.hh"


namespace tgm
{



class City
{
	public:
		bool empty() const { return m_blocks.empty(); }
		auto const& blocks() const { return m_blocks; }


		void add_block(CityBlockId const cbid) 
		{
			#if DYNAMIC_ASSERTS
				if (std::find(m_blocks.cbegin(), m_blocks.cend(), cbid) != m_blocks.cend()) { throw std::runtime_error("Adding an already added block."); }
			#endif

			m_blocks.push_back(cbid);
		}

		void remove_block(CityBlockId const cbid)
		{
			auto it = std::find(m_blocks.cbegin(), m_blocks.cend(), cbid);

			#if DYNAMIC_ASSERTS
				if (it == m_blocks.cend()) { throw std::runtime_error("Cannot remove a never-added block."); }
			#endif

			m_blocks.erase(it);
		}

	private:
		std::vector<CityBlockId> m_blocks;
};



} //namespace tgm


#endif //GM_CITY_HH