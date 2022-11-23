#include "debug_vertices.hh"


#include "settings/debug/debug_settings.hh"
#if VISUALDEBUG



#include "debug/logger/streams.h"


namespace tgm
{



auto operator<<(Logger & lgr, DebugVertexData const & dvd) -> Logger &
{
	lgr << "DebugVertexData{ ("
		<< std::setw(5) << dvd.position[0] << ", "
		<< std::setw(5) << dvd.position[1] << ", "
		<< std::setw(5) << dvd.position[2] << "), ("
		<< std::setprecision(5) << std::setw(5) << dvd.color[0] << ", "
		<< std::setprecision(5) << std::setw(5) << dvd.color[1] << ", "
		<< std::setprecision(5) << std::setw(5) << dvd.color[2] << ", "
		<< std::setprecision(5) << std::setw(5) << dvd.color[1] << ") }";

	return lgr;
}


void DebugVertices::reset(int const dppt, Vector3i const cursor, IntParallelepiped const& frame, int const required_capacity)
{
	m_dppt = dppt;
	m_cursor = cursor;
	m_frame = frame;

	m_vertices.clear();
	m_vertices.reserve(required_capacity);
}


void DebugVertices::push_tile(int const x, int const y, float const layer, Color const col)
{
	// From "map reference system (in tiles)" to "VisualDebug frame reference system (in tiles)"
	auto fx = x - m_frame.behind,
		 fy = y - m_frame.left;

	// From "frame reference system (in tiles)" to "OpenGL view reference system (in pixels)"
	auto const wleft   =   tiles_to_dpixels(fy),
			   wright  =   tiles_to_dpixels(fy + 1),
			   wtop    = - tiles_to_dpixels(fx),
			   wbottom = - tiles_to_dpixels(fx + 1);
	 
	internal_pushRectangle(wleft, wright, wtop, wbottom, layer, col);
}


void DebugVertices::push_tileOutline(int const x, int const y, float const layer, Color const color)
{
	auto fx = x - m_frame.behind,
		 fy = y - m_frame.left;

	auto const outline_width = m_dppt / 10.f; //in pixels

	// N rectangle
	{
		// From "frame reference system (in tiles)" to "OpenGL view reference system (in pixels)"
		auto const wleft  =   tiles_to_dpixels(fy),
				   wright =   tiles_to_dpixels(fy + 1) - outline_width,
				   wtop   = - tiles_to_dpixels(fx),
				   wdown  = - tiles_to_dpixels(fx) - outline_width;

		internal_pushRectangle(wleft, wright, wtop, wdown, layer, color);
	}
	// E rectangle
	{
		// From "frame reference system (in tiles)" to "OpenGL view reference system (in pixels)"
		auto const wleft  =   tiles_to_dpixels(fy + 1) - outline_width,
				   wright =   tiles_to_dpixels(fy + 1),
				   wtop   = - tiles_to_dpixels(fx),
				   wdown  = - tiles_to_dpixels(fx + 1) + outline_width;

		internal_pushRectangle(wleft, wright, wtop, wdown, layer, color);
	}
	// S rectangle
	{
		// From "frame reference system (in tiles)" to "OpenGL view reference system (in pixels)"
		auto const wleft  =   tiles_to_dpixels(fy) + outline_width,
				   wright =   tiles_to_dpixels(fy + 1),
				   wtop   = - tiles_to_dpixels(fx + 1) + outline_width,
				   wdown  = - tiles_to_dpixels(fx + 1);

		internal_pushRectangle(wleft, wright, wtop, wdown, layer, color);
	}
	// W rectangle
	{
		// From "frame reference system (in tiles)" to "OpenGL view reference system (in pixels)"
		auto const wleft  =   tiles_to_dpixels(fy),
				   wright =   tiles_to_dpixels(fy) + outline_width,
				   wtop   = - tiles_to_dpixels(fx) - outline_width,
				   wdown  = - tiles_to_dpixels(fx + 1);

		internal_pushRectangle(wleft, wright, wtop, wdown, layer, color);
	}
}


void DebugVertices::push_tiles(std::vector<Vector3i> const& positions, float const layer, Color const col)
{
	std::vector<Vector3i> visible_tiles;

	// Make a selection in order to draw only visible tiles
	for (auto const pos : positions)
	{
		if (pos.z == m_cursor.z && m_frame.contains(pos))
		{
			visible_tiles.push_back(pos);
		}
	}

	for (auto const pos : visible_tiles)
	{
		push_tile(pos.x, pos.y, layer, col);
	}
}


void DebugVertices::push_rectangle(FloatRect const rect, int const z_level, float const layer, Color const color)
{
	if (z_level != m_cursor.z) { return; }
				
	// Offset of the portion of map actually framed (in pixels)
	auto const frame_left   = tiles_to_dpixels(m_frame.left), 
			   frame_behind = tiles_to_dpixels(m_frame.behind);

	// From units (map reference system) to OpenGL view space reference system (in pixels)
	auto const vleft   =   units_to_dpixels(rect.left)     - frame_left, 
			   vright  =   units_to_dpixels(rect.right())  - frame_left,
			   vtop    = - units_to_dpixels(rect.top)      + frame_behind,
			   vbottom = - units_to_dpixels(rect.bottom()) + frame_behind;

	internal_pushRectangle(vleft, vright, vtop, vbottom, layer, color);
}


void DebugVertices::push_segment(Vector2f const map_v0, Vector2f const map_v1, int const z_level, float const layer, Color const color)
{
	if (z_level != m_cursor.z)
		return;


	auto const v0 = mapRS_to_vdRS(map_v0),
			   v1 = mapRS_to_vdRS(map_v1);

	#if VISUALDEBUG_DEBUGLOG
		VDlog << Logger::nltb << "vertex " << v0 << " (in pixels -- OpenGL view reference system)";
	#endif

	auto thickness = static_cast<float>(GSet::ppi_adjustment());


	auto diff = v1 - v0;

	if (!std::isfinite(diff.x) || !std::isfinite(diff.y))
		throw std::runtime_error("Unexpected vertices.");
				

    //TODO: 11: It should be std::sqtrf. It's been changed to being compatible with g++
    auto versor = diff / ::sqrtf(diff.x * diff.x + diff.y * diff.y);      // Versor of the v0->v1 segment.

	Vector2f ortho_versor{ - versor.y, versor.x };							// Versor orthogonal to the segment.


	push_quadrilateral(v0 - ortho_versor * thickness,
					   v0 + ortho_versor * thickness,
					   v1 + ortho_versor * thickness,
					   v1 - ortho_versor * thickness,
					   layer, color);
}

void DebugVertices::internal_pushRectangle(float const wleft, float const wright, float const wtop, float const wbottom, float const layer, Color const col)
{
	DebugVertexData NE{ wright, wtop   , layer, col };
	DebugVertexData SE{ wright, wbottom, layer, col };
	DebugVertexData SW{ wleft , wbottom, layer, col };
	DebugVertexData NW{ wleft , wtop   , layer, col };


	//top-left triangle
	m_vertices.push_back(NE);
	m_vertices.push_back(SW);
	m_vertices.push_back(NW);

	//bottom-right triangle
	m_vertices.push_back(NE);
	m_vertices.push_back(SE);
	m_vertices.push_back(SW);
}


void DebugVertices::push_quadrilateral(Vector2f const v0, Vector2f const v1, Vector2f const v2, Vector2f const v3, float const layer, Color const col)
{
	DebugVertexData dvd0{ v0.x, v0.y, layer, col };
	DebugVertexData dvd1{ v1.x, v1.y, layer, col };
	DebugVertexData dvd2{ v2.x, v2.y, layer, col };
	DebugVertexData dvd3{ v3.x, v3.y, layer, col };


	//top-left triangle
	m_vertices.push_back(dvd0);
	m_vertices.push_back(dvd1);
	m_vertices.push_back(dvd3);

	//bottom-right triangle
	m_vertices.push_back(dvd1);
	m_vertices.push_back(dvd2);
	m_vertices.push_back(dvd3);
}


auto DebugVertices::mapRS_to_vdRS(Vector2f const v) -> Vector2f
{
	// From "map reference system (in units)" to "VisualDebug frame reference system (in units)".
	Vector2f rel{ v.x - GSet::tiles_to_units(m_frame.behind),
					v.y - GSet::tiles_to_units(m_frame.left)   };

	// From "VisualDebug frame reference system (in units)" to "OpenGL view space reference system (in pixels)"
	return Vector2f{ units_to_dpixels(rel.y), - units_to_dpixels(rel.x) };
}




} //namespace tgm


#endif //VISUALDEBUG
