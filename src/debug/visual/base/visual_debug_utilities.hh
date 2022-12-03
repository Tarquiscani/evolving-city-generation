#ifndef GM_VISUAL_DEBUG_UTILITIES_HH
#define GM_VISUAL_DEBUG_UTILITIES_HH



#include "system/parallelepiped.hh"

namespace tgm
{



namespace VisualDebugUtilities
{
	inline auto tiles_to_dpixels(int const tiles, int const dppt) noexcept -> float { return static_cast<float>(tiles * dppt); }

	////
	//	@frame: VisualDebug frame (in tiles -- map reference system)
	////
	inline auto frame_to_pixels(IntParallelepiped const& frame, int const dppt) noexcept -> FloatParallelepiped
	{
		return {
			tiles_to_dpixels(frame.behind, dppt), tiles_to_dpixels(frame.left, dppt),  tiles_to_dpixels(frame.down, dppt),
			tiles_to_dpixels(frame.length, dppt), tiles_to_dpixels(frame.width, dppt), tiles_to_dpixels(frame.height, dppt)
		};
	}

} //namespace VisualDebugUtilities


namespace VDUtil = VisualDebugUtilities;



} //namespace tgm


#endif //GM_VISUAL_DEBUG_UTILITIES_HH