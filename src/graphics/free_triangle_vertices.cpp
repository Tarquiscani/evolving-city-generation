#include "free_triangle_vertices.hh"

#include <array>
#include "mapbox/earcut.hpp"


namespace tgm
{



auto operator<<(Logger & lgr, EdgeableVertexData const& evd) -> Logger &
{
    lgr << "EdgeableVertexData{ (" << std::setw(5) << evd.world_pos[0] << ", " << std::setw(5) << evd.world_pos[1] << ", " << std::setw(5) << evd.world_pos[2] << "), ("
        << std::setprecision(6) << std::setw(8) <<  evd.tex_coords[0] << ", " << std::setprecision(6) << std::setw(8) << evd.tex_coords[1] << "), "
        << std::setw(6) << evd.entity_id << ", " << std::setw(6) << evd.edgeable_id << " }";

    return lgr;
}




FreeTriangleVertices::FreeTriangleVertices(size_type const max_size, Texture2D & texture) 
    : m_slot_mgr(max_size), m_poly_to_triangles(max_size / 2), m_texture(texture)
{
    auto const max_vertexSize = static_cast<decltype(m_vertices)::size_type>(max_size) * vpt;

    if (max_vertexSize > m_vertices.max_size() )
        throw std::runtime_error("Such a big vertices_count is not allowed.");


    try	
    {
        m_vertices.reserve(max_vertexSize);
    }
    catch (std::exception const& e)
    {
        std::ostringstream oss; oss << "No space in RAM for such a big triangle count. " << e.what();
        throw std::runtime_error(oss.str());
    }
}


auto FreeTriangleVertices::create_polygon(FreePolygon const& polygon) -> DataArrayId
{

    auto & el = m_poly_to_triangles.create();
    auto & triangle_set = el.value.triangle_ids;

    auto const& [recycling, complete_edgId, slot_edgId] = edgeable_ids.create_id();
    el.value.edgeable_id = complete_edgId;


    auto const& poly_vertices = polygon.vertices();


    //--- Triangulate the polygon (projecting it on the XY plane).
    using EarcutPoint = std::array<double, 2>;
    std::vector<std::vector<EarcutPoint>> earcut_poly; 
    auto & main_poly = earcut_poly.emplace_back();	//The first vector of points refers to the external edge of the polygon. The other vectors refer to holes.
    for (auto const v : poly_vertices)
    {
        main_poly.push_back({ static_cast<double>(v.x), static_cast<double>(v.y) });
    }

    auto const earcut_indices = mapbox::earcut(earcut_poly);

    #if DYNAMIC_ASSERTS
        if (earcut_indices.size() == 0) { throw std::runtime_error("Mapbox earcut algorithm can't triangulate this polygon. It's probably degenerate or lies "
                                                                   "on a vertical plane, so that its projection is degenerate."); }
    #endif


    for (decltype(earcut_indices)::size_type current = 0; current < earcut_indices.size(); current += 3)
    {
        auto const tv0 = poly_vertices[earcut_indices[current]];
        auto const tv1 = poly_vertices[earcut_indices[current + 1]];
        auto const tv2 = poly_vertices[earcut_indices[current + 2]];

        triangle_set.push_back(create_triangle(FreeTriangle{ polygon.pos(), tv0, tv1, tv2, 0u, slot_edgId }));
    }

    return el.id();
}

auto FreeTriangleVertices::create_triangle(FreeTriangle const triangle) -> FreeTriangleId
{
    auto [recycling, new_id, slot] = m_slot_mgr.create_id();

    if (recycling)
    {
        internal_setTriangle(slot, triangle);
    }
    else
    {
        m_vertices.insert(m_vertices.end(), vpt, {{0.f, 0.f, 0.f}, {0.f, 0.f}, 0u, 0u});

        internal_setTriangle(slot, triangle);
    }

    m_changed = true;

    return new_id;
}


void FreeTriangleVertices::set_triangle(FreeTriangleId const id, FreeTriangle const triangle)
{
    auto slot = m_slot_mgr.validate_id(id);

    if (slot)
    {
        internal_setTriangle(*slot, triangle);

        m_changed = true;
    }
}


void FreeTriangleVertices::destroy_polygon(FreePolygonId const fpid)
{
    auto & poly_info = m_poly_to_triangles.get(fpid);

    edgeable_ids.destroy_id(poly_info.edgeable_id);

    for (auto const ftid : poly_info.triangle_ids)
    {
        destroy(ftid);
    }

    m_poly_to_triangles.destroy(fpid);

}


void FreeTriangleVertices::destroy(FreeTriangleId const id)
{
    auto slot = m_slot_mgr.destroy_id(id);

    if (slot)
    {
        internal_setTriangle(*slot, FreeTriangle{});

        m_changed = true;
    }
}

void FreeTriangleVertices::clear()
{
    FreeTriangleId ftid = m_slot_mgr.first_nonFreeId();

    while (ftid != 0)
    {
        destroy(ftid);

        ftid = m_slot_mgr.first_nonFreeId();
    }
}

void FreeTriangleVertices::internal_setTriangle(SlotId const triangle_slot, FreeTriangle const triangle)
{
    auto const fv0 = triangle.v0();
    auto const fv1 = triangle.v1();
    auto const fv2 = triangle.v2();

    auto const v0_world = compute_worldCoordinates(triangle.pos(), fv0);
    auto const v1_world = compute_worldCoordinates(triangle.pos(), fv1);
    auto const v2_world = compute_worldCoordinates(triangle.pos(), fv2);
    
    auto const v0_tex = compute_openGLTextureCoordinates(fv0, m_texture);
    auto const v1_tex = compute_openGLTextureCoordinates(fv1, m_texture);
    auto const v2_tex = compute_openGLTextureCoordinates(fv2, m_texture);
    
    
    auto const vertex0_id = static_cast<decltype(m_vertices)::size_type>(triangle_slot) * vpt;  // id of the first vertex of the triangle in m_vertices

    auto const v0 = &m_vertices[vertex0_id];
    auto const v1 = v0 + 1;
    auto const v2 = v0 + 2;

    v0->world_pos[0] = v0_world.x;
    v0->world_pos[1] = v0_world.y;
    v0->world_pos[2] = v0_world.z;
    v0->tex_coords[0] = v0_tex.x;
    v0->tex_coords[1] = v0_tex.y;
    v0->entity_id = triangle.entity_id();
    v0->edgeable_id = triangle.edgeable_id();
    

    v1->world_pos[0] = v1_world.x;
    v1->world_pos[1] = v1_world.y;
    v1->world_pos[2] = v1_world.z;
    v1->tex_coords[0] = v1_tex.x;
    v1->tex_coords[1] = v1_tex.y;
    v1->entity_id = triangle.entity_id();
    v1->edgeable_id = triangle.edgeable_id();
    

    v2->world_pos[0] = v2_world.x;
    v2->world_pos[1] = v2_world.y;
    v2->world_pos[2] = v2_world.z;
    v2->tex_coords[0] = v2_tex.x;
    v2->tex_coords[1] = v2_tex.y;
    v2->entity_id = triangle.entity_id();
    v2->edgeable_id = triangle.edgeable_id();
}


auto FreeTriangleVertices::compute_worldCoordinates(Vector3f const pos, FreeVertex const fv) -> Vector3f
{
    // The map is subdivided in floors. Each floor in OpenGL world reference system is much taller than the floor in
    // map reference system. This allows to have multiple layers and proper z-ordering.
    auto const z_floor = ::floorf(pos.z) * GSet::floorsSpacing_ratio; //TODO: 11: It should be "std::floorf", but g++ doesn't allow it.

    // World space z-axis position of the sprite.
    auto const world_z = z_floor + fv.z;


    // World y offset. Each floor is slightly shifted compared to the previous one, in order to reproduce a top-down camera angle of ~30 degree.
    auto const y_offset = pos.z * GSet::wySliding_ratio();
    
    // The axes of the world space are different from those of the map space. 
    // The world y-axis corresponds to the map x-axis but goes in the opposite direction.
    auto const world_y = ( - (pos.x + fv.x)) + y_offset;



    return { pos.y   + fv.y,		// The world x-axis corresponds to the map y-axis.
             world_y,
             world_z		};
}

auto FreeTriangleVertices::compute_openGLTextureCoordinates(FreeVertex const fv, Texture2D const& texture) noexcept -> Vector2f
{
    return { fv.tex_u / texture.width(),
             (texture.height() - fv.tex_v)  /  texture.height()   };
}

auto FreeTriangleVertices::init_edgeableIds() -> SlotManager
{
    SlotManager sm{ max_edgeableEntities };

    // Create an unused id just to reserve the slot 0. An edgeable id mustn't be 0, because 0 is reserved for the background and 
    // for the entities that doesn't have any edge.
    sm.create_id(); 

    return sm;
}

auto operator<<(Logger & lgr, FreeTriangleVertices const& ftv) -> Logger &
{
    lgr << Logger::nltb << "FreeTriangleVertices {"
        << Logger::addt
        << Logger::nltb << "m_slot_mgr: " << ftv.m_slot_mgr
        << Logger::nltb << "vertices_count: " << ftv.vertices_count();

    for (auto id : ftv.m_slot_mgr.debug_idsVecRef())
    {
        if(!SlotManager::debug_isFree(id))
        {
            auto slot_id = SlotManager::debug_slot(id);

            auto const vertex0_id = static_cast<decltype(ftv.m_vertices)::size_type>(slot_id) * ftv.vpt;  // id of the first vertex of the triangle

            lgr << "\n\n\n" << Logger::nltb << "Slot: " << slot_id;
            lgr << Logger::addt;


            EdgeableVertexData const* a = &ftv.m_vertices[vertex0_id];

            lgr	<< Logger::nltb << "Vertex 0:\t" << *a;
            lgr << Logger::nltb << "Vertex 1:\t" << *(a + 1);
            lgr << Logger::nltb << "Vertex 2:\t" << *(a + 2);


            lgr << Logger::remt;
        }
    }

    lgr << Logger::remt
        << Logger::nltb << "}";

    return lgr;
}



} //namespace tgm
