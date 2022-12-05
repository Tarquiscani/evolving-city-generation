#version 430 core
out vec4 fs_frag_color;

in vec2 vs_tex_coords;

// Texture of the scene rendered with textures
layout (binding = 15) uniform sampler2D u_edfScene_texturedColorTex;
// Texture of the edges of the scene
layout (binding = 16) uniform sampler2D u_edfEdges_colorTex;


void main()
{
    float edge_strength = texture(u_edfEdges_colorTex, vs_tex_coords).r;
	vec3 texture = texture(u_edfScene_texturedColorTex, vs_tex_coords).rgb;

	fs_frag_color = vec4(texture * edge_strength, 1);
} 