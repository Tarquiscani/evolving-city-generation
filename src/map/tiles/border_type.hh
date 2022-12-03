#ifndef GM_BORDER_TYPE_HH
#define GM_BORDER_TYPE_HH


#include <iostream>


namespace tgm
{



enum class BorderType
{
	none,

	// No branch
	solo,

	// One branch
	N,
	E,
	S,
	W,

	// Two branches
	NS,
	EW,
	NE,
	ES,
	SW,
	NW,

	//three branches
	NEW,
	NES,
	NSW,
	ESW,

	//four branches
	NESW,
};

auto operator<<(std::ostream & os, BorderType type) -> std::ostream &;



} //namespace tgm


#endif //GM_BORDER_TYPE_HH