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
}



} //namespace tgm
