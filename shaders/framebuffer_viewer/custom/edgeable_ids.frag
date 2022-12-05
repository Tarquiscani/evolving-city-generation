#version 430 core
out vec4 fs_frag_color;

in vec2 vs_tex_coords;

layout (binding = 0) uniform usampler2D u_edgeableIds_tex;


void main()
{
	uint edgeable_id = texture(u_edgeableIds_tex, vec2(vs_tex_coords.x, vs_tex_coords.y)).r;

	float random1 = fract( sin(edgeable_id * 12.9898f) * 43758.5453f ); //random generator one-liner
	float random2 = fract( sin(random1 * 12.9898f) * 43758.5453f );
	float random3 = fract( sin(random2 * 12.9898f) * 43758.5453f );

	fs_frag_color = vec4(random1, random2, random3, 1.f);
}