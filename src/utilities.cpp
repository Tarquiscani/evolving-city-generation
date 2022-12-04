#include "utilities.hh"


#include <cmath>


namespace tgm
{
	


namespace Utilities
{
	auto floor(Vector3f const v) -> Vector3f
	{
                return { ::floorf(v.x), ::floorf(v.y), ::floorf(v.z) }; //TODO: 11: It should be "std::floorf", but g++ doesn't allow it.
	}

	auto round(Vector3f const v) -> Vector3f
	{
                return { std::round(v.x), std::round(v.y), std::round(v.z) };
	}

	auto interp(float const current, float const target, float const delta_time, float const interp_speed) -> float
	{
		if (interp_speed <= 0.f)
		{
			return target;
		}

		auto const total_delta = target - current;

		// If the delta is very small, then don't interpolate
		if (total_delta * total_delta < 0.0000001f)
		{
			return target;
		}

		// Distance traveled in this frame
		auto const partial_delta = total_delta * std::clamp(delta_time * interp_speed, 0.f, 1.f);

		return current + partial_delta;
	}
}



} //namespace tgm
