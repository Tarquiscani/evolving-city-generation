#ifndef GM_AREA_TEMPLATE_HH
#define GM_AREA_TEMPLATE_HH


#include "settings/simulation/simulation_settings.hh"
#include "system/vector2.hh"
#include "graphics/color.hh"
#include "map/tiles/tile.hh"

#include "debug/logger/logger.h"


namespace tgm
{



enum class AreaType
{
	none,
	// Farm areas
	field, cowshed, large_cowshed, xxl_cowshed, super_field, farmyard, test,
};

auto operator<<(std::ostream & os, AreaType const at) -> std::ostream &;


////
//	It contains basic informations for a specific type of area.
////
class AreaTemplate
{
	public:
		AreaTemplate(Vector2i const min_dimensions, bool const roofable, TileType const tile_style, 
					 BorderStyle const border_style, Color const debug_col, int const cost):
			m_min_dims(min_dimensions), m_roofable(roofable),
			m_tile_style(tile_style), m_border_style(border_style), 
			m_cost(cost), 
			m_debug_color(debug_col) { }


		auto min_dims() const noexcept { return m_min_dims; }
		bool is_roofable() const noexcept { return sim_settings.map.roof_every_area ? true : m_roofable; }
		auto tile_style() const noexcept { return m_tile_style; }
		auto border_style() const noexcept { return m_border_style; }
		auto debug_color() const noexcept { return m_debug_color; }

	private:
		//Type of the area. Note that for each building the same value has a different meaning
		Vector2i m_min_dims{ 0, 0 };
		bool m_roofable = false;
		TileType m_tile_style = TileType::ground;
		BorderStyle m_border_style = BorderStyle::brickWall;
		int m_cost = 0;
		Color m_debug_color = Color::Black;					// Color of the area in BuildingExpansionVisualDebug

	friend class Building;
};



inline std::unordered_map<AreaType, AreaTemplate> const area_templates = 
		{
			{ AreaType::field,			AreaTemplate{ Vector2i(10,10), false, TileType::wooden, BorderStyle::brickWall, Color::Yellow,		0  } },
			{ AreaType::farmyard,		AreaTemplate{ Vector2i(10,10), true,  TileType::wooden, BorderStyle::brickWall, Color::Grey,		0  } },
			{ AreaType::super_field,	AreaTemplate{ Vector2i(10,10), true,  TileType::wooden, BorderStyle::brickWall, Color(92, 46, 145),	2  } },
			{ AreaType::cowshed,		AreaTemplate{ Vector2i(8,8),   true,  TileType::wooden, BorderStyle::brickWall, Color(204, 51, 0) ,	2  } },
			{ AreaType::large_cowshed,	AreaTemplate{ Vector2i(12,12), true,  TileType::wooden, BorderStyle::brickWall, Color(204, 51, 0),	20 } },
			{ AreaType::test,			AreaTemplate{ Vector2i(11,11), true,  TileType::wooden, BorderStyle::brickWall, Color::Grey,		0  } }
		};



} //namespace tgm


#endif //GM_AREA_TEMPLATE_HH