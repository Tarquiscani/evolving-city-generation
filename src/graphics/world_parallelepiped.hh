#ifndef GM_WORLD_PARALLELEPIPED_HH
#define GM_WORLD_PARALLELEPIPED_HH


#include <iostream>
#include <cmath>

#include "system/vector3.hh"


namespace tgm
{



////
//
//	Parallelepiped in OpenGL world space.
//
////
struct WorldParallelepiped
{
	////
	//	@left: Lowest x belonging to the parallelepiped.
	//  @front: Lowest y belonging to the parallelepiped.
	//	@down: Lowest z belonging to the parallelepiped.
	//	@length: Dimension along the x-axis.
	//	@width: Dimension along the y-axis.
	//	@height: Dimension along the z-axis.
	////
	WorldParallelepiped(float const a_left, float const a_front, float const a_down, float const a_length, float const a_width, float const a_height) noexcept :
		left(a_left), front(a_front), down(a_down), length(a_length), width(a_width), height(a_height) { }


	auto right() const noexcept -> float { return left + length; }
	auto behind() const noexcept -> float { return front + width; }
	auto up() const noexcept -> float { return down + height; }


	auto center() -> Vector3f
	{
		return {
			left + length / 2.f,
			front + width / 2.f,
			down + height / 2.f
		};
	}

	////
	//	Compute the length of the diagonal of the parallelepiped.
	////
	auto diagonal_length() const -> float
	{
		return ::sqrtf(length * length + width * width + height * height); //TODO: 11: It should be "std::sqrtf", but g++ doesn't allow it.
	}


	float left = 0.f;
	float front = 0.f;
	float down = 0.f;
	float length = 0.f;
	float width = 0.f;
	float height = 0.f;
	
	friend auto operator<<(std::ostream & os, const WorldParallelepiped& wp) -> std::ostream &;
};



} // namespace tgm


#endif //GM_WORLD_PARALLELEPIPED_HH
