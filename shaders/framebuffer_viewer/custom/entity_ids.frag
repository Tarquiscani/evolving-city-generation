#version 430 core
out vec4 fs_frag_color;

in vec2 vs_tex_coords;

layout (binding = 0) uniform usampler2D u_visibility_tex;


void main()
{
	uint entity_id = texture(u_visibility_tex, vec2(vs_tex_coords.x, vs_tex_coords.y)).r;

	if(entity_id != 0u)
	{
		uint tile_id = entity_id - 1; //"+1" was added to avoid "entity_id == 0"
		
		const uint map_length = 100;
		const uint map_width  = 150;

		const uint map_area = map_length * map_width;

		uint floor = tile_id / map_area;


		uint relative_pos = tile_id - map_area * floor;			//relative position of the tile in the floor
		float shade = float(relative_pos) / float(map_area);	//shade of tile (tile with lower ids have a darker color)
		shade = (shade + 1.f) / 2.f;							//avoid too dark shades

		float floor_color1 = fract( sin(floor		 * 12.9898f) * 43758.5453f );	//random generator one-liner
		float floor_color2 = fract( sin(floor_color1 * 12.9898f) * 43758.5453f );
		float floor_color3 = fract( sin(floor_color2 * 12.9898f) * 43758.5453f );

		fs_frag_color = vec4(	vec3(floor_color1, floor_color2, floor_color3) * shade	, 1);

	}
	else
	{
		fs_frag_color = vec4(0, 0, 0, 1);
	}


}