#ifndef GM_DIRECTION_H
#define GM_DIRECTION_H

#include "system/vector2.hh"
#include "system/vector3.hh"
#include "system/rect.hh"
#include "window/glfw_wrapper.hh"

enum class Direction : unsigned char
{
	none,
	U, UN, UNE, UE, USE, US, USW, UW, UNW,      //upper level directions
		N,  NE,  E,  SE,  S,  SW,  W,  NW,		//same level directions
	L, LN, LNE, LE, LSE, LS, LSW, LW, LNW,       //lower level directions
};
static constexpr int direction_count = 27;

std::ostream& operator<<(std::ostream & os, Direction d) noexcept;


namespace DirectionUtil
{
	inline bool is_diagonal(Direction const drc)
	{
		//ordered according to the probability of usage, to exploit short-circuit evaluation
		return	drc == Direction::NE || drc == Direction::SE || drc == Direction::SW || drc == Direction::NW
			|| drc == Direction::LNE || drc == Direction::LSE || drc == Direction::LSW || drc == Direction::LNW
			|| drc == Direction::UNE || drc == Direction::USE || drc == Direction::USW || drc == Direction::UNW;
	}

	inline auto invert(Direction const drc) -> Direction
	{
		switch (drc)
		{
			case Direction::U:
				return Direction::L;
			case Direction::UN:
				return Direction::LS;
			case Direction::UNE:
				return Direction::LSW;
			case Direction::UE:
				return Direction::LW;
			case Direction::USE:
				return Direction::LNW;
			case Direction::US:
				return Direction::LN;
			case Direction::USW:
				return Direction::LNE;
			case Direction::UW:
				return Direction::LE;
			case Direction::UNW:
				return Direction::LSE;

			case Direction::N:
				return Direction::S;
			case Direction::NE:
				return Direction::SW;
			case Direction::E:
				return Direction::W;
			case Direction::SE:
				return Direction::NW;
			case Direction::S:
				return Direction::N;
			case Direction::SW:
				return Direction::NE;
			case Direction::W:
				return Direction::E;
			case Direction::NW:
				return Direction::SE;

			case Direction::L:
				return Direction::U;
			case Direction::LN:
				return Direction::US;
			case Direction::LNE:
				return Direction::USW;
			case Direction::LE:
				return Direction::UW;
			case Direction::LSE:
				return Direction::UNW;
			case Direction::LS:
				return Direction::UN;
			case Direction::LSW:
				return Direction::UNE;
			case Direction::LW:
				return Direction::UE;
			case Direction::LNW:
				return Direction::USE;

			case Direction::none:
				return Direction::none;

			default:
				throw std::runtime_error("Unhadled case.");
		}
	}

	////
	//	@return: A unit vector that points towards @drc.
	////
	auto planeUnitVector(Direction const drc) -> Vector2i;


	auto planeUnitVector_to_clockWiseId(Vector2i const uvec) -> int;
	
	////
	//	Useful function to iterate counterclockwise through plane directions.
	//	@nghb_id: A number ranging from '0' to '7'. '0' identifies the northern neighbor, '7' identifies the north-eastern neighbor.
	////
	auto counterclockwiseId_to_planeUnitVector(int const ccw_id) -> Vector2i;

	auto planeUnitVector_to_counterclockwiseId(Vector2i const uvec) -> int;

	////
	//	Compute the distance from @u0 to @u1 traversing the angle in a clockwise or counterclockwise sense.
	////
	auto planeUnitVector_distance(Vector2i const lhs, Vector2i const rhs, bool const counterclockwise = false) -> int;


	inline auto orthogonalLeft_planeUnitVector(Vector3i const& uvec) -> Vector3i
	{
		if		(uvec == Versor3i::N)
		{
			return Versor3i::W;
		}
		else if (uvec == Versor3i::NE)
		{
			return Versor3i::NW;
		}
		else if (uvec == Versor3i::E)
		{
			return Versor3i::N;
		}
		else if (uvec == Versor3i::SE)
		{
			return Versor3i::NE;
		}
		else if (uvec == Versor3i::S)
		{
			return Versor3i::E;
		}
		else if (uvec == Versor3i::SW)
		{
			return Versor3i::SE;
		}
		else if (uvec == Versor3i::W)
		{
			return Versor3i::S;
		}
		else if (uvec == Versor3i::NW)
		{
			return Versor3i::SW;
		}
		else
		{
			throw std::runtime_error("Not a unit vector");
		}
	}

	inline auto orthogonalRight_planeUnitVector(Vector3i const uvec) -> Vector3i
	{
		return -orthogonalLeft_planeUnitVector(uvec);
	}

	////
	//	@return: A new FloatRect with the same dimensions of @origin_rect but moved towards @drc of @distance. 
	////
	inline auto compute_newRect(FloatRect const origin_rect, Direction const drc, float const distance) -> FloatRect
	{
		auto const uv = planeUnitVector(drc);

		return FloatRect{ origin_rect.top + uv.x * distance, origin_rect.left + uv.y * distance, origin_rect.length, origin_rect.width };
	}

	inline auto key_to_direction(int const key) -> Direction
	{
		switch (key)
		{
			/*case GLFW_KEY_W:
				return Direction::U;

			case GLFW_KEY_S:
				return Direction::L;*/

			case GLFW_KEY_W:
				return Direction::N;

			case GLFW_KEY_D:
				return Direction::E;

			case GLFW_KEY_S:
				return Direction::S;

			case GLFW_KEY_A:
				return Direction::W;

			default:
				return Direction::none;
		}
	}

	inline auto key_to_unitVector(int const key) -> Vector3i
	{
		switch (key)
		{
			case GLFW_KEY_KP_8:
				return {-1,  0,  0};
				break;

			case GLFW_KEY_KP_9:
				return {-1,  1,  0};
				break;

			case GLFW_KEY_KP_6:
				return { 0,  1,  0};
				break;

			case GLFW_KEY_KP_3:
				return { 1,  1,  0};
				break;

			case GLFW_KEY_KP_2:
				return { 1,  0,  0};
				break;

			case GLFW_KEY_KP_1:
				return { 1, -1,  0};
				break;

			case GLFW_KEY_KP_4:
				return { 0, -1,  0};
				break;

			case GLFW_KEY_KP_7:
				return {-1, -1,  0};
				break;

			default:
				return { 0,  0,  0};
				break;
		}
	}

	inline auto direction_to_unit_vector(Direction const drc) -> Vector3i
	{
		switch (drc)
		{
			case Direction::N:
				return {-1,  0,  0};
				break;

			case Direction::NE:
				return {-1,  1,  0};
				break;

			case Direction::E:
				return { 0,  1,  0};
				break;

			case Direction::SE:
				return { 1,  1,  0};
				break;

			case Direction::S:
				return { 1,  0,  0};
				break;

			case Direction::SW:
				return { 1, -1,  0};
				break;

			case Direction::W:
				return { 0, -1,  0};
				break;

			case Direction::NW:
				return {-1, -1,  0};
				break;

			default:
				return { 0,  0,  0};
				break;
		}
	}

	inline auto unit_vector_to_direction(Vector3i const unit_vector) -> Direction
	{
		if		(unit_vector == Vector3i{ -1, 0, 0 })
		{
			return Direction::N;
		}
		else if (unit_vector == Vector3i{ -1,  1,  0 })
		{
			return Direction::NE;
		}
		else if (unit_vector == Vector3i{  0,  1,  0 })
		{
			return Direction::E;
		}
		else if (unit_vector == Vector3i{  1,  1,  0 })
		{
			return Direction::SE;
		}
		else if (unit_vector == Vector3i{  1,  0,  0 })
		{
			return Direction::S;
		}
		else if (unit_vector == Vector3i{  1, -1,  0 })
		{
			return Direction::SW;
		}
		else if (unit_vector == Vector3i{  0, -1,  0 })
		{
			return Direction::W;
		}
		else if (unit_vector == Vector3i{ -1, -1,  0 })
		{
			return Direction::NW;
		}
		else
		{
			return Direction::none;
		}
	}
}




#endif