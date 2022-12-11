#ifndef GM_FREE_TRIANGLE_VERTICES_HH
#define GM_FREE_TRIANGLE_VERTICES_HH


#include <cstdint>
#include <iostream>
#include <iomanip>
#include <vector>

#include <glad/glad.h>

#include "data_strctures/slot_manager.hh"
#include "data_strctures/data_array.hh"
#include "dynamic_subimage.hh"
#include "free_polygon.hh"
#include "free_triangle.hh"
#include "graphics/textures/texture_2d.hh"
#include "settings/graphics_settings.hh"
#include "system/parallelepiped.hh"

#include "debug/logger/logger.hh"
#include "settings/debug/debug_settings.hh"


namespace tgm
{



struct EdgeableVertexData
{
    GLfloat world_pos[3];
    GLfloat tex_coords[2];
    GLuint entity_id;
    GLuint edgeable_id;
};

auto operator<<(Logger & lgr, EdgeableVertexData const& evd) -> Logger &;



using FreeTriangleId = CompleteId;
using FreePolygonId = CompleteId;

//TODO: Make triangles of FreeTriangleVertices take part in occlusion culling
class FreeTriangleVertices
{
    private:
        struct PolygonInfo
        {
            std::vector<FreeTriangleId> triangle_ids;	//ids of the triangles associated to this polygon
            CompleteId edgeable_id = 0u;				//id used to draw a black edge around the polygon
        };


    public:
        using size_type = uint32_t;

        FreeTriangleVertices(size_type const max_size, Texture2D & texture);

        ////
        //	Size of the buffer (in bytes).
        ////
        auto buffer_byteSize() const { return m_vertices.size() * sizeof(EdgeableVertexData); }
        auto vertices_count() const { return m_vertices.size(); }
        auto get_ptr() const { return m_vertices.data(); }

        bool has_changed() const { return m_changed; }
        void flushed() { m_changed = false; }
        

        auto create_polygon(FreePolygon const& polygon) -> DataArrayId;
        auto create_triangle(FreeTriangle const triangle) -> FreeTriangleId;
        
        void set_triangle(FreeTriangleId const id, FreeTriangle const triangle);

        void destroy(FreeTriangleId const id);
        void destroy_polygon(FreePolygonId const id);


        void clear();


        auto debug_verticesVecSize() { return m_vertices.size(); }
        auto debug_verticesVec() { return m_vertices;	}
        auto & debug_slotManager() { return m_slot_mgr; }


    private:

        bool m_changed = false;

        std::vector<EdgeableVertexData> m_vertices;				//m_vertices != slots. A slot corresponds to a whole triangle, i.e. 3 vertices.

        SlotManager m_slot_mgr;

        Texture2D & m_texture;


        DataArray<PolygonInfo, true> m_poly_to_triangles;   //map each polygon to its constituent triangles

        static constexpr int max_edgeableEntities = 100000u;
        static auto init_edgeableIds() -> SlotManager;
        //TODO: THREAD-SAFETY: edgeable_ids is a global variable and it isn't thread safe.
        static inline SlotManager edgeable_ids = init_edgeableIds();


        static constexpr int vpt = 3; //vertices per triangle



        void internal_setTriangle(SlotId const triangle_slot, FreeTriangle const triangle);
        
        ////
        //	Compute the world space position (in units) from the map position (in units).
        //	@pos: Position of the triangle in the map (in units -- map RS).
        //	@fv: Relative position of the vertex from @pos (in units -- map RS) and texture coordinates (in texels -- GIMP-like RS).
        //  Note: The position of each vertex is split in two components because only @pos will be shifted according to the world floor rules. 
        //		  The vertices are freely positioned inside the world space RS.
        ////
        static auto compute_worldCoordinates(Vector3f const pos, FreeVertex const fv) -> Vector3f;

        
        ////
        //	Transform GIMP texture coordinates in @fv to OpenGL texture coordinates:
        //
        //		GIMP reference system: The origin is the top-left corner of the texture, the X-axis is oriented towards the right
        //			and the Y-axis is oriented towards the bottom. A unit correspond to 1 pixel. Each pixel is identified by its 
        //			top-left corner coordinates.
        //
        //		OpenGL reference system: The origin is the bottom-left corner of the texture, the X-axis is oriented towards the left 
        //			and the Y-axis is oriented towards the top. A unit correspond to the whole texture. Each pixel is identified by
        //			its center coordinates.
        ////
        static auto compute_openGLTextureCoordinates(FreeVertex const fv, Texture2D const& texture) noexcept -> Vector2f;


        friend auto operator<<(Logger & lgr, FreeTriangleVertices const& dv) -> Logger &;
};



} //namespace tgm


#endif //GM_FREE_TRIANGLE_VERTICES_HH