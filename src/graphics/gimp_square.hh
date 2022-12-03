#ifndef GM_GIMP_SQUARE_HH
#define GM_GIMP_SQUARE_HH


#include <iostream>


namespace tgm
{



//////
//	Just an utility class to create a GIMP square in all the possible texture definitions. The actual position and dimension 
//	of the square is decided runtime depending on the present texture definition.
//////
struct GIMPSquare
{
	GIMPSquare(float const left_ld,  float const top_ld,
			   float const left_hd,  float const top_hd);

	GIMPSquare(float const left_ld,  float const top_ld,  float const width_ld,  float const height_ld,
			   float const left_hd,  float const top_hd,  float const width_hd,  float const height_hd);

	float left = 0.f;
	float top = 0.f;
	float width = 0.f;
	float height = 0.f;

	friend auto operator<<(std::ostream & os, GIMPSquare const& gs) -> std::ostream &;
};



} // namespace tgm


#endif //GM_GIMP_SQUARE_HH