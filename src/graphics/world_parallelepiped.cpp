#include "world_parallelepiped.hh"



auto operator<<(std::ostream & os, const WorldParallelepiped& wp) -> std::ostream &
{
	os << "WorldParallelepiped { left: "   << wp.left	<< ", front: " << wp.front << ", down: "   << wp.down 
						   << ", length: " << wp.length << ", width: " << wp.width << ", height: " << wp.height << " }";

	return os;
}