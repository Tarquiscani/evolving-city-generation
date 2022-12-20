#ifndef GM_TILESET_VERTEX_DATA_HH
#define GM_TILESET_VERTEX_DATA_HH


#include <glad/glad.h>

#include "debug/logger/logger.hh"


namespace tgm
{



struct TilesetVertexData
{
    GLfloat world_pos[3];	                    // OpenGL world reference system.
    GLfloat background_sprite_coords[2];        // Starting UVs in the texture atlas
    GLfloat section_sprite_coords[2];
    GLfloat corner_shadow_sprite_coords[2];
    GLuint background_sprite_layer;             // Layer in the TexArray2D
    GLuint section_sprite_layer;
    GLuint corner_shadow_sprite_layer;
    GLuint entity_id;
};

auto operator<<(Logger & lgr, TilesetVertexData const& tvd) -> Logger &;



} //namespace tgm


#endif //GM_TILESET_VERTEX_DATA_HH