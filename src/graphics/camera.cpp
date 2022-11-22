#include "camera.hh"


#include "utilities.hh"
#include "graphics_utilities.hh"


#include "debug/logger/logger.h"
#include "debug/logger/debug_printers.h"



namespace tgm
{



auto Camera::distance() const noexcept -> float
{
	return GSet::tiles_to_units(m_zoom * 20.f);
}
		
auto Camera::pixel_dim() const noexcept -> float
{
	return GSet::texels_to_units(1.f) * m_zoom;
}


void Camera::set_target(FloatParallelepiped const target_vol, DynamicSubimage const& target_subimage, Direction const target_drc, std::vector<IntParallelepiped> const& noRoofArea_volumes)
{
	// Compute the target sprite
	auto sprite_wvol = GUtil::compute_dynamicSprite(target_vol, target_subimage, pixel_dim(), target_drc, true);

	// Round the center of the sprite so that it always points to the same pixel of the target sprite
	auto rounded_midLength = std::floor(sprite_wvol.length /2.f);
	auto rounded_midWidth = std::floor(sprite_wvol.width / 2.f);


	m_target = { sprite_wvol.left + rounded_midLength, sprite_wvol.front + rounded_midWidth, sprite_wvol.up() };
	GSet::TEST_cameraTargetPosition = m_target;



	// If no roof areas have changed from the previous frame
	if(noRoofArea_volumes != m_old_noRoofAreasVolumes)
	{

		// Compute intersections between no-roof volumes
		std::vector<IntParallelepiped> intersections;
		for (int i = 0; i < noRoofArea_volumes.size(); ++i)
		{
			for (int j = i + 1; j < noRoofArea_volumes.size(); ++j)
			{
				auto vol_i = noRoofArea_volumes[i];
				auto vol_j = noRoofArea_volumes[j];

				auto intersection = vol_i.intersect(vol_j);

				// Choose only actual intersections and only if they are horizontal
				if (!intersection.is_null() && intersection.width > intersection.length)
					intersections.push_back(intersection);
			}
		}


		auto wy_sliding = target_vol.down * GSet::wySliding_ratio();

		
		// Compute beginnings and ends of no-roof rectangles
		m_noRoofRects_size = 0u;
		if (noRoofArea_volumes.size() <= max_noRoofRects)
		{
			m_noRoofRects_size = static_cast<unsigned>(noRoofArea_volumes.size()); //size is never bigger than unsigned limit

			for (int i = 0; i < noRoofArea_volumes.size(); ++i)
			{
				auto vol = noRoofArea_volumes[i];
				// Note that the begin is the left-bottom corner of the parallelepiped because of the reference system of OpenGL world coordinates.
				// Note that the end is the right-top corner of the parallelepiped because of the reference system of OpenGL world coordinates.
				m_noRoofRects[i * 2]     = { GSet::tiles_to_units(vol.left + 0.5f),		   GSet::tiles_to_units(-vol.front_end() + 0.75f) + wy_sliding };
				m_noRoofRects[i * 2 + 1] = { GSet::tiles_to_units(vol.right_end() - 0.5f), GSet::tiles_to_units(-vol.behind - 0.5f)		  + wy_sliding };
			}
		}
		else
		{
			throw std::runtime_error("Unexpected situation. Too many no-roof rectangles.");
		}


		
		// Compute beginnings and ends of no-roof intersections (needed because no-roof rectangles aren't adjacent but there's some space in between)
		m_noRoofIntersections_size = 0u;
		if (intersections.size() <= max_noRoofIntersections)
		{
			m_noRoofIntersections_size = static_cast<unsigned>(intersections.size()); //size is never bigger than unsigned limit

			for (int i = 0; i < intersections.size(); ++i)
			{
				auto vol = intersections[i];
				// Note that the begin is the left-bottom corner of the parallelepiped because of the reference system of OpenGL world coordinates.
				// Note that the end is the right-top corner of the parallelepiped because of the reference system of OpenGL world coordinates.
				m_noRoofIntersections[i * 2]     = { GSet::tiles_to_units(vol.left + 0.6f),		   GSet::tiles_to_units(-vol.front_end()) + wy_sliding };
				m_noRoofIntersections[i * 2 + 1] = { GSet::tiles_to_units(vol.right_end() - 0.6f), GSet::tiles_to_units(-vol.behind)	  + wy_sliding };
			}
		}
		else
		{
			throw std::runtime_error("Unexpected situation. Too many no-roof intersections.");
		}



		m_old_noRoofAreasVolumes = noRoofArea_volumes;
		m_noRoofRects_changed = true;
	}
}



} //namespace tgm