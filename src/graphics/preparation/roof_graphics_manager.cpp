#include "roof_graphics_manager.hh"


#include "graphics/algorithms/hip_roof/hip_roof_algorithm.hh"


namespace tgm
{



void RoofGraphicsManager::prepare(GameMap const& simulation)
{
	auto const& roofs = simulation.building_manager().roofs();

	for (auto const rid : m_mediator.removed_roofs())
	{
		auto const& rg = m_roof_graphics.at(rid);

		for (auto const fpid : rg.south_polygons)
		{
			m_roof_vertices.south_roof.destroy_polygon(fpid);
		}

		for (auto const fpid : rg.west_polygons)
		{
			m_roof_vertices.west_roof.destroy_polygon(fpid);
		}

		for (auto const fpid : rg.north_polygons)
		{
			m_roof_vertices.north_roof.destroy_polygon(fpid);
		}

		for (auto const fpid : rg.east_polygons)
		{
			m_roof_vertices.east_roof.destroy_polygon(fpid);
		}

		m_roof_graphics.erase(rid);
	}


	auto const& tiles = simulation.tiles();

	for (auto const rid : m_mediator.added_roofs())
	{
		auto const& r = roofs.get_or_throw(rid);

		#if DYNAMIC_ASSERTS
			if (m_roof_graphics.find(rid) != m_roof_graphics.cend()) { ; }
		#endif

		auto & r_graphics = m_roof_graphics[rid];


		auto const polygons = HipRoofAlgorithm::generate_hipRoof(r.roofed_poss, r.roofed_poss.front().z, tiles.length(), tiles.width());
		
		for (auto const& poly : polygons.south)
		{
			auto const fpid = m_roof_vertices.south_roof.create_polygon(poly);
			r_graphics.south_polygons.push_back(fpid);
		}

		for (auto const& poly : polygons.west)
		{
			auto const fpid = m_roof_vertices.west_roof.create_polygon(poly);
			r_graphics.west_polygons.push_back(fpid);
		}

		for (auto const& poly : polygons.north)
		{
			auto const fpid = m_roof_vertices.north_roof.create_polygon(poly);
			r_graphics.north_polygons.push_back(fpid);
		}

		for (auto const& poly : polygons.east)
		{
			auto const fpid = m_roof_vertices.east_roof.create_polygon(poly);
			r_graphics.east_polygons.push_back(fpid);
		}
	}

	m_mediator.changes_acquired();
}



} //namespace tgm