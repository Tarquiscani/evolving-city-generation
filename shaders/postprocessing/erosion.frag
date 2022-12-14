#version 430 core
out vec4 fs_frag_color;

in vec2 vs_tex_coords;

//Texture representing the edges of the scene (somewhat greyscale, but mostly black and white)
layout (binding = 16) uniform sampler2D u_edfEdges_colorTex;

uniform int u_edge_thickness;


void main()
{
    float pixel_strength = 1;
    float delta = 0.3;

    int half_thickness = u_edge_thickness / 2;

    for (int i = 0; i < u_edge_thickness; i++) 
    {
        for (int j = 0; j < u_edge_thickness; j++) 
        {
            float current = texelFetch(u_edfEdges_colorTex, ivec2(gl_FragCoord) + ivec2(i - half_thickness, j - half_thickness), 0 ).r;
            pixel_strength -= (1 - current) * delta;
        }
    }

    float pixel_strength_clamped = max(0, pixel_strength);

    fs_frag_color = vec4(pixel_strength_clamped, pixel_strength_clamped, pixel_strength_clamped, 1);
} 