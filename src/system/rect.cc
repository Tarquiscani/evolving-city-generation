#include "rect.hh"

#include <iomanip>

#include "utilities.hh"


namespace tgm
{



std::ostream& operator<<(std::ostream& os, IntRect r)
{
	os	<< "IntRect(" << std::setw(4) << r.top	    << ", " << std::setw(4) << r.left	 << ", " 
					  << std::setw(4) << r.bottom() << ", " << std::setw(4) << r.right() << ")";

	return os;
}

bool operator==(IntRect const & lhs, IntRect const & rhs)
{
	return lhs.top == rhs.top
		&& lhs.left == rhs.left
		&& lhs.length == rhs.length
		&& lhs.width == rhs.width;
}

bool operator!=(IntRect const & lhs, IntRect const & rhs)
{
	return !(lhs == rhs);
}



bool are_equal(FloatRect const& lhs, FloatRect const& rhs, float const abs_tolerance, float const rel_tolerance)
{
	return Util::are_equal(lhs.top, rhs.top, abs_tolerance, rel_tolerance)
		&& Util::are_equal(lhs.left, rhs.left, abs_tolerance, rel_tolerance)
		&& Util::are_equal(lhs.length, rhs.length, abs_tolerance, rel_tolerance)
		&& Util::are_equal(lhs.width, rhs.width, abs_tolerance, rel_tolerance);
}


auto operator<<(std::ostream & os, FloatRect const& fr) -> std::ostream &
{
	os << "FloatRect(" << std::setw(4) << fr.top << ", " << std::setw(4) << fr.left << ", "
					   << std::setw(4) << fr.bottom() << ", " << std::setw(4) << fr.right() << ")";

	return os;
}



} //namespace tgm