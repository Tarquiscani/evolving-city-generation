#ifndef GM_UTILITIES_HH
#define GM_UTILITIES_HH


#include <cstdint>
#include <cmath>

#include "settings/graphics_settings.hh"
#include "system/vector2.hh"
#include "system/vector3.hh"
#include "system/rect.hh"


#include "settings/debug/debug_settings.hh"


namespace tgm
{



namespace Utilities
{
	////
	//	Compare two floats for non-strict equality.
	////
	inline bool are_equal(float const lhs, float const rhs, float const abs_tolerance, float const rel_tolerance)
	{
		return std::fabs(lhs - rhs) <= abs_tolerance
			|| std::fabs(lhs - rhs) <= rel_tolerance * std::max(lhs, rhs);
	}


	inline bool are_equal(Vector2f const lhs, Vector2f const rhs, float const abs_tolerance, float const rel_tolerance)
	{
		return are_equal(lhs.x, rhs.x, abs_tolerance, rel_tolerance)
			&& are_equal(lhs.y, rhs.y, abs_tolerance, rel_tolerance);
	}

	template <typename T> inline auto uint32_to_int32(T) = delete; // Disallow implicit conversions for this function
	inline auto uint32_to_int32(std::uint32_t const n) -> std::int32_t 
	{  
		auto const casted = static_cast<std::int64_t>(n); // Cast to a type large enough that can reasonably contain the value.

		if (casted >= INT32_MAX) { throw std::runtime_error("Cannot cast uint32 to int32, because it's too big."); }

		return static_cast<std::int32_t>(n);
	}


	inline auto v2f_to_v3i(Vector2f const v) -> Vector3i { return { static_cast<int>(v.x), static_cast<int>(v.y), 0 }; }
	inline auto v3i_to_v2f(Vector3i const v) -> Vector2f { return { static_cast<float>(v.x), static_cast<float>(v.y) }; }

	inline auto units_to_tiles(Vector2f const v) -> Vector2i { return { GSet::units_to_tiles(v.x), GSet::units_to_tiles(v.y) }; }

	auto round(Vector3f const v) -> Vector3f;
}

namespace Util = Utilities;



inline auto bug_bigNotification() -> std::string
{
	return std::string(R"(
///////////////////////////////////////////////////////////////////
//                                                               //
//                                                               //
//           |||||         ||     ||         |||||               //
//           ||   ||       ||     ||       ||     ||             //
//           ||   ||       ||     ||       ||                    //
//           |||||         ||     ||       ||   ||||             //
//           ||   ||       ||     ||       ||     ||             //
//           ||   ||       ||     ||       ||     ||             //
//           |||||           |||||           |||||               //
//                                                               //
//                                                               //
///////////////////////////////////////////////////////////////////
						)");
}



} //namespace tgm


#endif //GM_UTILITIES_HH