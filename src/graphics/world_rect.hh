#pragma once


#include <iostream>


struct WorldRect
{
	WorldRect(float const a_left, float const a_bottom, float const a_width, float const a_length) noexcept :
		left(a_left), bottom(a_bottom), width(a_width), length(a_length) { }



	////
	//	Rightermost Y of the rectangle.
	////
	auto right() const noexcept -> float { return left + width; }

	////
	//	Uppermost X of the rectangle.
	////
	auto top() const noexcept -> float { return bottom + length; }



	float left = 0.f;
	float bottom = 0.f;
	float length = 0.f;
	float width = 0.f;

	friend auto operator<<(std::ostream & os, const WorldRect& wr) -> std::ostream &;
};