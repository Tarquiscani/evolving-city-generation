//"#version 430 core" directive has been automatically declared in shader.cpp
//"TILE_SHADER" definition has been automatically declared in shader.cpp
	//"GSET_TILESET_TEXARRAY" definition has been automatically declared in shader.cpp
//"EDGEABLE_IDS" definition has been automatically declared in shader.cpp

#if TILE_SHADER
	layout(location = 0) in vec3 VAO_world_pos;
	layout(location = 1) in vec2 VAO_tex_coords;
	layout(location = 2) in uint VAO_layer;
	layout(location = 3) in uint VAO_entity_id;
#else
	layout(location = 0) in vec3 VAO_world_pos;
	layout(location = 1) in vec2 VAO_tex_coords;
	layout(location = 2) in uint VAO_entity_id;
	#if EDGEABLE_IDS
		layout(location = 3) in uint VAO_edgeable_id;
	#endif
#endif


#if TILE_SHADER
	out vec3 vs_world_pos;
	out vec2 vs_tex_coords;
	flat out uint vs_layer;
#else
	out vec3 vs_world_pos;
	out vec2 vs_tex_coords;
	#if EDGEABLE_IDS
		flat out uint vs_edgeable_id;
	#endif
#endif

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * u_view * vec4(VAO_world_pos, 1.0);

	vs_world_pos = VAO_world_pos;
	vs_tex_coords = VAO_tex_coords;

	#if TILE_SHADER
		vs_layer = VAO_layer;
	#endif

	#if EDGEABLE_IDS
		vs_edgeable_id = VAO_edgeable_id;
	#endif
}