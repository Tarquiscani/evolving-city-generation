//"#version 430 core" directive has been automatically declared in shader.cpp
//"GSET_TILESET_TEXARRAY" definition has been automatically declared in shader.cpp


layout (location = 0) in vec3 VAO_world_pos;
layout (location = 1) in vec2 VAO_tex_coords;
layout (location = 2) in uint VAO_layer;
layout (location = 3) in uint VAO_entity_id;

out vec3 vs_world_pos;
out vec2 vs_tex_coords;
flat out uint vs_layer;
flat out uint vs_entity_id;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	vs_world_pos = VAO_world_pos;

    gl_Position = u_projection * u_view * vec4(VAO_world_pos, 1.0);

	vs_tex_coords = VAO_tex_coords;
	vs_layer = VAO_layer;
	vs_entity_id = VAO_entity_id;
}