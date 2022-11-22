#include "parallelepiped.hh"


#include <iomanip>


namespace tgm
{



void IntParallelepiped::combine(IntParallelepiped const& rhs)
{
	if (is_null())
	{
		behind = rhs.behind;
		left   = rhs.left;
		down   = rhs.down;
		length = rhs.length;
		width  = rhs.width;
		height = rhs.height;
	}
	else
	{
		auto const min_behind = std::min(behind, rhs.behind);
		auto const min_left   = std::min(left,   rhs.left);
		auto const min_down   = std::min(down,   rhs.down);

		length = std::max(front(), rhs.front())  + 1 - min_behind;
		width  = std::max(right(), rhs.right())  + 1 - min_left;
		height = std::max(up(),    rhs.up())     + 1 - min_down;

		behind = min_behind; //I can't update them before because the original values were needed in front(), right() and up().
		left   = min_left;
		down   = min_down;
	}
}

auto operator<<(std::ostream & os, IntParallelepiped const& p) -> std::ostream &
{
	os << "IntParallelepiped(" << std::setw(4) << p.behind  << ", " << std::setw(4) << p.left    << ", " << std::setw(4) << p.down << ") -> ("
							   << std::setw(4) << p.front() << ", " << std::setw(4) << p.right() << ", " << std::setw(4) << p.up() << ")";

	return os;
}

bool operator==(IntParallelepiped const& lhs, IntParallelepiped const& rhs)
{
	return lhs.behind == rhs.behind
		&& lhs.left   == rhs.left 
		&& lhs.down   == rhs.down
		&& lhs.length == rhs.length
		&& lhs.width  == rhs.width
		&& lhs.height == rhs.height;
}

bool operator!=(IntParallelepiped const& lhs, IntParallelepiped const& rhs)
{
	return !(lhs == rhs);
}



auto operator<<(std::ostream & os, FloatParallelepiped const& p) -> std::ostream &
{
	os << "FloatParallelepiped(" << std::setw(4) << p.behind      << ", " << std::setw(4) << p.left    << ", " << std::setw(4) << p.down << ") -> ("
								 << std::setw(4) << p.front() << ", " << std::setw(4) << p.right() << ", " << std::setw(4) << p.up() << ")";

	return os;
}



} //namespace tgm