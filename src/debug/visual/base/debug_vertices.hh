#ifndef GM_DEBUG_VERTICES_HH
#define GM_DEBUG_VERTICES_HH


#include "settings/debug/debug_settings.hh"
#if VISUALDEBUG



#include <vector>

#include "graphics/color.hh"
#include "settings/graphics_settings.hh"
#include "system/parallelepiped.hh"
#include "system/vector3.hh"
#include "visual_debug_utilities.hh"

#include "debug/logger/debug_printers.hh"


namespace tgm 
{



struct DebugVertexData
{
	////
	//	@x, @y, @z: (in pixels -- OpenGL view space reference system)
	////
	DebugVertexData(float const x, float const y, float const z, Color const a_col) noexcept:
		position{ x, y, z }, color{ a_col.r / 255.f, a_col.g / 255.f, a_col.b / 255.f, a_col.a / 255.f } {}

	float position[3]; //world x, world y, world z
	float color[4]; //r, g, b, a
};

auto operator<<(Logger & lgr, DebugVertexData const & dvd) -> Logger &;



class DebugVertices
{
	public:
		DebugVertices() = default;
		DebugVertices(DebugVertices const&) = delete;
		DebugVertices& operator=(DebugVertices const&) = delete;

		auto byte_size() const noexcept { return m_vertices.size() * sizeof(DebugVertexData); }
		auto data() const noexcept { return m_vertices.data(); }
		auto vertex_count() const noexcept { return m_vertices.size(); }

		auto frame() const noexcept { return m_frame; }
		auto frame_inPixels() const noexcept { return VDUtil::frame_to_pixels(m_frame, m_dppt); }


		void reset(int const dppt, Vector3i const cursor, IntParallelepiped const& frame, int const required_capacity);

		////
		//	@x, @y: (in tiles -- map reference system)
		////
		void push_tile(int const x, int const y, float const layer, Color const col);

		////
		//	@x, @y: (in tiles -- map reference system)
		////
		void push_tileOutline(int const x, int const y, float const layer, Color const color);

		////
		//	@positions: (in tiles -- map reference system)
		////
		void push_tiles(std::vector<Vector3i> const& positions, float const layer, Color const col);

		////
		//	@rect: (in units -- map reference system)
		////
		void push_rectangle(FloatRect const rect, int const z_level, float const layer, Color const color);

		////
		//	@map_v0, @map_v1: (in units -- map reference system)
		////
		void push_segment(Vector2f const map_v0, Vector2f const map_v1, int const z_level, float const layer, Color const color);


	private:
		std::vector<DebugVertexData> m_vertices;

		int m_dppt = 0;
		Vector3i m_cursor;
		IntParallelepiped m_frame;

		auto tiles_to_dpixels(int const tiles) const noexcept -> float { return VDUtil::tiles_to_dpixels(tiles, m_dppt); }

		////
		//	Change unit of measure from map "units" to VisualDebug window "pixels".
		////
		inline auto units_to_dpixels(float const units) noexcept -> float { return units / GSet::upt * m_dppt; }

		////
		//	Compute "OpenGL view space coordinates (in pixels)" from "map coordinates (in units)".
		////
		auto mapRS_to_vdRS(Vector2f const v) -> Vector2f;

		////
		//	@wleft, @wright, @wtop, @wbottom: (in pixels -- OpenGL view space reference system)
		////
		void internal_pushRectangle(float const wleft, float const wright, float const wtop, float const wbottom, float const layer, Color const col);

		////
		//	@v0, @v1, @v2, @v3: (in pixels -- OpenGL view space reference system)
		////
		void push_quadrilateral(Vector2f const v0, Vector2f const v1, Vector2f const v2, Vector2f const v3, float const layer, Color const col);
};



} //namespace tgm


#endif //VISUALDEBUG


#endif //GM_DEBUG_VERTICES_HH