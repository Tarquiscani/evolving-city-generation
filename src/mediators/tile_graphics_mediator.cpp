#include "tile_graphics_mediator.hh"


namespace tgm
{



void TileGraphicsMediator::record_areaChange(IntParallelepiped const& vol) 
{
	// Inner area
	for (auto y = vol.left + 1; y <= vol.right() - 1; ++y)
	{
		for (auto x = vol.behind + 1; x <= vol.front() - 1; ++x)
		{
			m_changes[{x, y, vol.down}];	//insert "false" if the position doesn't exist or do nothing if it already exists
		}
	}

	// Horizontal borders
	auto const vol_right = vol.right();
	for (auto y = vol.left; y <= vol_right; ++y)
	{
		auto const x_top = vol.behind;
		auto const x_bottom = vol.front();
				
		m_changes[{x_top,    y, vol.down}] = true;
		m_changes[{x_bottom, y, vol.down}] = true;
	}
	
	// Vertical borders
	auto const vol_front = vol.front();
	for (auto x = vol.behind + 1; x <= vol_front - 1; ++x)
	{
		auto const y_left = vol.left;
		auto const y_right = vol.right();
		
		m_changes[{x, y_left,  vol.down}] = true;
		m_changes[{x, y_right, vol.down}] = true;
	}
}



} //namespace tgm