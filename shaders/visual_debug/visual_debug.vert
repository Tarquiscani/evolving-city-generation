#version 430 core

layout(location = 0) in vec3 VAO_world_pos;
layout(location = 1) in vec4 VAO_color;

out vec4 vs_color;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_view * vec4(VAO_world_pos, 1.0);

    vs_color = VAO_color;
}