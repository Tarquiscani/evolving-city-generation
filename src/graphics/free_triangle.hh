#ifndef GM_FREE_TRIANGLE_HH
#define GM_FREE_TRIANGLE_HH


#include <glad/glad.h>

#include "graphics/free_vertex.hh"
#include "graphics/textures/texture_2d.hh"
#include "system/vector2.hh"
#include "system/vector3.hh"

#include "debug/logger/logger.h"


namespace tgm
{



class FreeTriangle
{
	public:
		FreeTriangle() = default;

		////
		//	@pos: Position of the triangle in the map (in units -- map reference system).
		//  @v0: Relative position of the vertex from @pos (in units). 
		//  Note: The position of each vertex is split in two components because only @pos will be shifted according to the world floor rules. 
		//		  The vertices are freely positioned.
		////
		FreeTriangle(Vector3f const pos,
					 FreeVertex const v0, FreeVertex const v1, FreeVertex const v2,
					 GLuint const entity_id,
					 GLuint const edgeable_id) noexcept :
			m_pos{pos},
			m_v0{v0}, m_v1{v1}, m_v2{v2},
			m_entity_id{entity_id},
			m_edgeable_id{edgeable_id}
		{}

		auto pos() const noexcept -> Vector3f { return m_pos; }

		auto v0() const noexcept -> FreeVertex { return m_v0; }
		auto v1() const noexcept -> FreeVertex { return m_v1; }
		auto v2() const noexcept -> FreeVertex { return m_v2; }

		auto entity_id() const noexcept -> GLuint { return m_entity_id; }
		auto edgeable_id() const noexcept -> GLuint { return m_edgeable_id; }


	private:

		Vector3f m_pos;

		FreeVertex m_v0;
		FreeVertex m_v1;
		FreeVertex m_v2;

		GLuint m_entity_id = 0u;	//id used in the occlusion culling to decide whether this triangle have to be drawn or not

		GLuint m_edgeable_id = 0u;	//id used in the edge-detection filter to identify triangles that belongs to the same surface (between them no edge must be drawn)


	friend auto operator<<(Logger & lgr, FreeTriangle const& ft) -> Logger &;
};



} //namespace tgm


#endif //GM_FREE_TRIANGLE_HH