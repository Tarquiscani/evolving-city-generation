#include "mobile_subimage_set.hh"


#include "debug/logger/logger.h"
#include "debug/logger/debug_printers.h"

MobileSubimageSet::MobileSubimageSet(GIMPSquare const gimp, Texture2D const& texture) :
	m_width(gimp.width), m_height(gimp.height),
	m_subimages{ {	{gimp.left + gimp.width,		gimp.top,	gimp.width, gimp.height, texture},
					{gimp.left + gimp.width * 3,	gimp.top,  -gimp.width, gimp.height, texture},
					{gimp.left,						gimp.top,	gimp.width, gimp.height, texture},
					{gimp.left + gimp.width * 2,	gimp.top,	gimp.width, gimp.height, texture}	} } //double braces required because std::array contains a single array as a member
{ }



auto MobileSubimageSet::pick_subimage(Direction const drc) const -> DynamicSubimage const&
{
	switch (drc)
	{
		case Direction::N:
		case Direction::U:
			return m_subimages[0];
			break;

		case Direction::NE:
		case Direction::E:
		case Direction::SE:
			return m_subimages[1];
			break;

		case Direction::none:
		case Direction::S:
		case Direction::L:
			return m_subimages[2];
			break;

		case Direction::NW:
		case Direction::W:
		case Direction::SW:
			return m_subimages[3];
			break;

		default:
			throw std::runtime_error("Trying to pick the texture subimage of a mobile that has an unexpected move_direction.");
			break;
	}
}