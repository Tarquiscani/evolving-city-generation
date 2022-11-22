#ifndef GM_HIP_ROOF_MICROTILE_HH
#define GM_HIP_ROOF_MICROTILE_HH



#include <bitset>
#include <stdexcept>
#include "settings/debug/debug_settings.hh"


namespace tgm
{

	

namespace HipRoofAlgorithm
{
	enum class MicrotileInclination
	{
		Flat,
		North,
		East,
		South,
		West
	};

	enum class MicrotileType
	{
		None,
		Side,
		Edge,
		Vertex
	};

	struct Microtile
	{
		public:
			auto height() const noexcept -> int { return m_height; }
			
			auto type() const noexcept -> MicrotileType;

			bool flat_inclination() const noexcept { return m_flat_inclination; }
			bool north_inclination() const noexcept { return m_north_inclination; }
			bool east_inclination() const noexcept { return m_east_inclination; }
			bool south_inclination() const noexcept { return m_south_inclination; }
			bool west_inclination() const noexcept { return m_west_inclination; }

			void fill(int const max_height)
			{
				#if DYNAMIC_ASSERTS
					if (m_height != -1 && m_height != max_height)
						throw std::runtime_error("Filling an already filled microtile with a different height.");
				#endif

				m_height = max_height;
			}

			void update(int const new_height, MicrotileInclination const new_inclin)
			{
				if (m_height != -1)
				{
					if (new_height < m_height)
					{
						m_height = new_height;

						m_flat_inclination = false;
						m_north_inclination = false;
						m_east_inclination = false;
						m_south_inclination = false;
						m_west_inclination = false;
						add_inclination(new_inclin);
					}
					else if (new_height == m_height)
					{
						add_inclination(new_inclin);
					}
					//else do nothing

					#if DYNAMIC_ASSERTS
						assert_validState();
					#endif
				}
			}

			bool has_multiple_inclinations()
			{
				int count = 0;
				if (m_north_inclination)
					++count;
				if (m_east_inclination)
					++count;
				if (m_south_inclination)
					++count;
				if (m_west_inclination)
					++count;

				return count >= 2;
			}
			
			bool has_inclination(MicrotileInclination const inclin) const
			{
				switch (inclin)
				{
					case MicrotileInclination::Flat:
					{
						return m_flat_inclination;
						break;
					}
					case MicrotileInclination::North:
					{
						return m_north_inclination;
						break;
					}
					case MicrotileInclination::East:
					{
						return m_east_inclination;
						break;
					}
					case MicrotileInclination::South:
					{
						return m_south_inclination;
						break;
					}
					case MicrotileInclination::West:
					{
						return m_west_inclination;
						break;
					}
					default:
						throw std::runtime_error("Unexpected inclination.");
						break;
				}
			}

			auto inclinations() const -> std::bitset<5> 
			{
				std::bitset<5> ret;
				
				ret[0] = m_north_inclination;
				ret[1] = m_east_inclination;
				ret[2] = m_south_inclination;
				ret[3] = m_west_inclination;
				ret[4] = m_flat_inclination;

				return ret;
			}
			
			void remove_inclination(MicrotileInclination const inclin)
			{
				switch (inclin)
				{
					case MicrotileInclination::Flat:
					{
						m_flat_inclination = false;
						break;
					}
					case MicrotileInclination::North:
					{
						m_north_inclination = false;
						break;
					}
					case MicrotileInclination::East:
					{
						m_east_inclination = false;
						break;
					}
					case MicrotileInclination::South:
					{
						m_south_inclination = false;
						break;
					}
					case MicrotileInclination::West:
					{
						m_west_inclination = false;
						break;
					}
					default:
						throw std::runtime_error("Unexpected inclination.");
						break;
				}
			}

			inline static std::bitset<5> const north{0b00001};
			inline static std::bitset<5> const east {0b00010};
			inline static std::bitset<5> const south{0b00100};
			inline static std::bitset<5> const west {0b01000};
			inline static std::bitset<5> const flat {0b10000};


		private:
			// Height of the center of the microtile
			int m_height = -1;
		
			// Indicate in which direction the roof height grows.
			bool m_north_inclination = false;
			bool m_east_inclination = false;
			bool m_south_inclination = false;
			bool m_west_inclination = false;
			bool m_flat_inclination = false;


			void add_inclination(MicrotileInclination const new_inclin) noexcept
			{
				switch (new_inclin)
				{
					case MicrotileInclination::Flat:
					{
						m_flat_inclination = true;
						break;
					}
					case MicrotileInclination::North:
					{
						m_north_inclination = true;
						break;
					}
					case MicrotileInclination::East:
					{
						m_east_inclination = true;
						break;
					}
					case MicrotileInclination::South:
					{
						m_south_inclination = true;
						break;
					}
					case MicrotileInclination::West:
					{
						m_west_inclination = true;
						break;
					}
				}
			}


			////
			//	After a change, check that the microtile is left in a meaningful state.
			////
			void assert_validState()
			{																									// F  N  E  S  W
				if (   (!m_flat_inclination && !m_north_inclination && !m_east_inclination && !m_south_inclination && !m_west_inclination)		// 0  0  0  0  0
					|| ( m_flat_inclination &&  m_north_inclination &&  m_east_inclination &&  m_south_inclination &&  m_west_inclination)		// 1  1  1  1  1
					|| ( m_flat_inclination &&  m_north_inclination && !m_east_inclination &&  m_south_inclination && !m_west_inclination)		// 1  1  0  1  0
					|| ( m_flat_inclination && !m_north_inclination &&  m_east_inclination && !m_south_inclination &&  m_west_inclination)   )	// 1  0  1  0  1
				{
					throw std::runtime_error("Invalid state.");
				}
			}
	};
	
	auto operator<<(std::ostream & os, MicrotileInclination const& inc) -> std::ostream &;

	auto operator<<(std::ostream & os, MicrotileType const& mt)->std::ostream &;
}



} //namespace tgm
using namespace tgm;


#endif //GM_HIP_ROOF_MICROTILE_HH