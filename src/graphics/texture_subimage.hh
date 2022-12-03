#ifndef GM_TEXTURE_SUBIMAGE_HH
#define GM_TEXTURE_SUBIMAGE_HH


#include "graphics/textures/texture_2d.hh"

#include "debug/logger/logger.h"


namespace tgm 
{



struct TextureSubimage
{
	////
	//	Accept GIMP coordinates and transform them in OpenGL texture coordinates:
	//
	//		GIMP reference system: The origin is the top-left corner of the texture, the X-axis is oriented towards the right
	//			and the Y-axis is oriented towards the bottom. A unit correspond to 1 pixel. Each pixel is identified by its 
	//			top-left corner coordinates.
	//
	//		OpenGL reference system: The origin is the bottom-left corner of the texture, the X-axis is oriented towards the left 
	//			and the Y-axis is oriented towards the top. A unit correspond to the whole texture. Each pixel is identified by
	//			its center coordinates.
	//	
	//	@left, @top: First pixel coordinates of the subimage (GIMP reference system).
	//	@width, @height: Dimensions of the subimage in pixels.
	////
	TextureSubimage(float const GIMP_left, float const GIMP_top, float const GIMP_width, float const GIMP_height, Texture2D const& texture)
	{
		left = GIMP_left / texture.width();
		bottom = ( texture.height() - (GIMP_top + GIMP_height) ) / texture.height();
		right = (GIMP_left + GIMP_width) / texture.width();
		top = ( texture.height() - GIMP_top ) / texture.height();
	}

	//OpenGL-like texture coordinates
	float left = 0.f;
	float bottom = 0.f;
	float right = 0.f;
	float top = 0.f;
};

inline auto operator<<(Logger & lgr, TextureSubimage const& ts) -> Logger &
{
	lgr << "TextureSubimage{ left: " << ts.left << ", bottom: " << ts.bottom << ", right: " << ts.right << ", top: " << ts.top << "}";

	return lgr;
}



} // namespace tgm


#endif //GM_TEXTURE_SUBIMAGE_HH