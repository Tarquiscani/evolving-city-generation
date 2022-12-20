//"#version 430 core" directive has been automatically declared in shader.cpp
//"GSET_TILESET_TEXARRAY" definition has been automatically declared in shader.cpp

// Must be kept in sync with the CPU-side m_tile_VAO and TilesetVertexData
layout(location = 0) in vec3 VAO_world_pos;
layout(location = 1) in vec2 VAO_background_sprite_coords;
layout(location = 2) in vec2 VAO_section_sprite_coords;
layout(location = 3) in vec2 VAO_corner_shadow_sprite_coords;
layout(location = 4) in uint VAO_background_sprite_layer;
layout(location = 5) in uint VAO_section_sprite_layer;
layout(location = 6) in uint VAO_corner_shadow_sprite_layer;
layout(location = 7) in uint VAO_entity_id;

out vec3 vs_world_pos;
out vec2 vs_background_sprite_coords;
flat out uint vs_background_sprite_layer;
flat out uint vs_entity_id;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    vs_world_pos = VAO_world_pos;

    gl_Position = u_projection * u_view * vec4(VAO_world_pos, 1.0);

    vs_background_sprite_coords = VAO_background_sprite_coords;
    vs_background_sprite_layer = VAO_background_sprite_layer;
    vs_entity_id = VAO_entity_id;
}