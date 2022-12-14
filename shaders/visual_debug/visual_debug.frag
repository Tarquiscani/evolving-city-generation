#version 430 core


out vec4 fs_frag_color;

in vec4 vs_color;


void main()
{
    fs_frag_color = vs_color;
}