#version 430 core
out vec4 fs_frag_color;

in vec2 vs_tex_coords;

layout (binding = 0) uniform sampler2D u_texture;


void main()
{
	fs_frag_color = texture(u_texture, vs_tex_coords);
}