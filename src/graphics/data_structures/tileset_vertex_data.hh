#ifndef GM_TILESET_VERTEX_DATA_HH
#define GM_TILESET_VERTEX_DATA_HH


#include <glad/glad.h>

#include "debug/logger/logger.h"


namespace tgm
{



struct TilesetVertexData
{
	GLfloat world_pos[3];	//OpenGL world reference system
	GLfloat tex_coords[2];
	GLuint layer;
	GLuint entity_id;
};

auto operator<<(Logger & lgr, TilesetVertexData const& tvd) -> Logger &;



} //namespace tgm


#endif //GM_TILESET_VERTEX_DATA_HH