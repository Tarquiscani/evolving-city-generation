//"#version 430 core" directive has been automatically declared in shader.cpp
//"TILE_SHADER" definition has been automatically declared in shader.cpp
	//"GSET_TILESET_TEXARRAY" definition has been automatically declared in shader.cpp
//"EDGEABLE_IDS" definition has been automatically declared in shader.cpp

#if TILE_SHADER && EDGEABLE_IDS
	#error "TILE_SHADER and EDGEABLE_IDS cannot be both active"
#endif

////
//	
//	Shader used to render the visible chunk of the map starting from the buffer of the visible entities.
//	This shader, depending on preprocessor directives, could also output to an additional texture that stores 
//	for each pixel the edgeable_id of the visible polygon.
//
////





out vec4 fs_textured_frag_color;
out uint fs_edgeable_id;

#if TILE_SHADER
	in vec3 vs_world_pos;
	in vec2 vs_tex_coords;
	flat in uint vs_layer;
#else
	in vec3 vs_world_pos;
	in vec2 vs_tex_coords;
	#if EDGEABLE_IDS
		flat in uint vs_edgeable_id;
	#endif
#endif


// Texture currently bound to the shader
#if TILE_SHADER && GSET_TILESET_TEXARRAY
	uniform sampler2DArray u_texture;	
#else
	uniform sampler2D u_texture;
#endif

// Position of the light source
uniform vec3 u_light_position;
// Indicate the normal of the surface, useful to compute the diffuse lighting (it's only used with roofs now).
uniform vec3 u_frag_normal;



// Uniforms used to discard fragments that lies above the player.
uniform float u_cameraTarget_zWorldPos;	//Position of the target (i.e. the player).
uniform uint u_noRoofRects_size; //Size of the noRoofAreas array.
uniform vec2 u_noRoofRects[8];	//Contains the Begin position and the End position of the areas with a BEBEBEBE... scheme.
uniform uint u_noRoofIntersections_size;
uniform vec2 u_noRoofIntersections[4];	//Contains the Begin positions and the End positions of the intersections between no-roof areas with a BEBEBEBE... scheme.
uniform vec2 u_viewport;


vec4 discard_if_abovePlayerArea(vec4 color);
vec4 discard_if_abovePlayerCircle(vec4 color);
vec4 show_mipmapLevel(vec4 color);

void main()
{
	float ambient_lighting = 0.2;
	float diffuse_lighting = dot( normalize(u_frag_normal), normalize(u_light_position) );

	#if TILE_SHADER && GSET_TILESET_TEXARRAY
		fs_textured_frag_color = texture(u_texture, vec3(vs_tex_coords, vs_layer));
	#else
		fs_textured_frag_color = texture(u_texture, vs_tex_coords);
	#endif
		

	fs_textured_frag_color.xyz *= min(1, diffuse_lighting + ambient_lighting); //each inclination of the roof has a different shadow. If affects only the RGB channels, and not the alpha one.
	
	#if EDGEABLE_IDS
		//Produce the output with edgeable_id associated to this pixel.
		fs_edgeable_id = vs_edgeable_id;
	#else
		fs_edgeable_id = 0u;		
	#endif


	// Alpha-test
    if(fs_textured_frag_color.a < 0.5)
		discard;


	fs_textured_frag_color = discard_if_abovePlayerArea(fs_textured_frag_color);
	//fs_textured_frag_color = discard_if_abovePlayerCircle(fs_textured_frag_color);
	

	/*
	#if TILE_SHADER
		fs_textured_frag_color = show_mipmapLevel(fs_textured_frag_color);
		mix(fs_textured_frag_color)
	#endif
	*/
}


vec4 show_mipmapLevel(vec4 color)
{
	float mipmap = textureQueryLod(u_texture, vs_tex_coords).y;

	if(mipmap > 1.f)
	{
		vec4 mip_color = vec4(0, 0, 0, 1);

		if(mipmap > 4.f)
			mip_color = vec4(1, 1, 1, 1);
		else if(mipmap > 3.f)
			mip_color = vec4(0, 0, 1, 1);
		else if(mipmap > 2.f)
			mip_color = vec4(0, 1, 0, 1);
		else if(mipmap > 1.f)
			mip_color = vec4(1, 1, 0, 1);

		color = mix(color, mip_color, 0.1);
	}

	return color;
}