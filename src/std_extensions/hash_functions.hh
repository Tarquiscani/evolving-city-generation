#ifndef GM_HASH_FUNCTIONS_HH
#define GM_HASH_FUNCTIONS_HH

#include <functional>
#include <cstddef>

#include "system/vector2.hh"
#include "system/vector3.hh"
#include "map/map_forward_decl.hh"

////
//	Note: It's the same as boost::hash_combine()
////
template <class T>
inline void hash_combine(std::size_t & seed, T const& v)
{
	//TODO: Be sure that it works right for 64-bit values.
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
	//it allows using Vector3i in unordered sets and maps.
	template <>
	struct hash<tgm::Vector3i>
	{
		std::size_t operator()(tgm::Vector3i const& v) const
		{
			std::size_t seed = 0;

			::hash_combine(seed, v.x);
			::hash_combine(seed, v.y);
			::hash_combine(seed, v.z);

			return seed;
		}
	};

	//it allows using Vector2i in unordered sets and maps.
	template <>
	struct hash<tgm::Vector2i>
	{
		std::size_t operator()(tgm::Vector2i const& v) const
		{
			std::size_t seed = 0;

			::hash_combine(seed, v.x);
			::hash_combine(seed, v.y);

			return seed;
		}
	};
	
	template <>
	struct hash<tgm::BuildingAreaCompleteId>
	{
		std::size_t operator()(tgm::BuildingAreaCompleteId const& acid) const
		{
			std::size_t seed = 0;

			::hash_combine(seed, acid.bid);
			::hash_combine(seed, acid.aid);

			return seed;
		}
	};
}

#endif //GM_HASH_FUNCTIONS_HH