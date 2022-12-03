#ifndef GM_FREE_POLYGON_HH
#define GM_FREE_POLYGON_HH


#include <vector>

#include <glad/glad.h>

#include "free_vertex.hh"
#include "system/vector3.hh"

#include "debug/logger/logger.h"


namespace tgm
{



class FreePolygon
{
	public:
		auto pos() const { return m_pos; }
		auto vertices() const -> std::vector<FreeVertex> const& { return m_vertices; }

		void set_pos(Vector3f const pos) { m_pos = pos; }

		////
		//	@v: Position (in units -- map RS) it's the relative distance of the vertex from m_pos. Texture coordinates (in texels -- GIMP coordinates). 
		////
		void push_vertex(FreeVertex const v)
		{
			m_vertices.push_back(v);
		}



	private:
		Vector3f m_pos;
		std::vector<FreeVertex> m_vertices;

	friend auto operator<<(Logger & lgr, FreePolygon const& fp) -> Logger &;
};



} //namespace tgm


#endif //GM_FREE_POLYGON_HH