//"#version 430 core" directive has been automatically declared in shader.cpp
//"GSET_TILESET_TEXARRAY" definition has been automatically declared in shader.cpp

////
//
//	This shader is part of the occlusion culling system.
//	Starting from the buffer of all the entites, it computes a framebuffer of the whole downsized map 
//	where each texel is the entity_id of the entity that is visible in that pixel.
//
////


out uint fs_entity_id;

in vec3 vs_world_pos;
in vec2 vs_tex_coords;
flat in uint vs_layer;
flat in uint vs_entity_id;

#if GSET_TILESET_TEXARRAY
    uniform sampler2DArray u_texture;
#else
    uniform sampler2D u_texture;
#endif


// Uniforms used to discard fragments that lies above the player.
uniform float u_cameraTarget_zWorldPos;	//Position of the target (i.e. the player).
uniform uint u_noRoofRects_size; //Size of the noRoofAreas array.
uniform vec2 u_noRoofRects[8];	//Contains the Begin position and the End position of the areas with a BEBEBEBE... scheme.
uniform uint u_noRoofIntersections_size;
uniform vec2 u_noRoofIntersections[4];	//Contains the Begin positions and the End positions of the intersections between no-roof areas with a BEBEBEBE... scheme.
uniform vec2 u_viewport;


vec4 discard_if_abovePlayerArea(vec4 color);
vec4 discard_if_abovePlayerCircle(vec4 color);


void main()
{
    #if GSET_TILESET_TEXARRAY
        vec4 tex_color = texture(u_texture, vec3(vs_tex_coords, vs_layer));
    #else
        vec4 tex_color = texture(u_texture, vs_tex_coords);
    #endif

    // Alpha-test
    if(tex_color.a < 0.5)
        discard;
        

    discard_if_abovePlayerArea(vec4(0, 0, 0, 0));
    //discard_if_abovePlayerCircle(vec4(0, 0, 0, 0));


    // Fill the texel corresponding to this fragment with the entity_id.
    fs_entity_id = vs_entity_id;
}