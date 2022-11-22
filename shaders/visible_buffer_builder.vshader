#version 430 core
layout (location = 0) in vec2 VAO_pos;
layout (location = 1) in vec2 VAO_tex_coords;

out vec2 vs_tex_coords;

void main()
{
	gl_Position = vec4(VAO_pos.x, VAO_pos.y, 0.0, 1.0);

	vs_tex_coords = VAO_tex_coords;
}