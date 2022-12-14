#version 430 core

out vec4 fs_frag_color;

in vec2 vs_tex_coords;


// Depth&stencil texture (with the parameter GL_DEPTH_STENCIL_TEXTURE_MODE set to GL_STENCIL_INDEX)
layout (binding = 16) uniform usampler2D u_overdraw_depthStencilTex;

void main()
{
    uint draw_count = texture(u_overdraw_depthStencilTex, vs_tex_coords).r;

    // Show stencil buffer
    if(draw_count == 1)
    {
        fs_frag_color = vec4(0, 1, 0, 1);
    }
    else if(draw_count > 1 && draw_count <= 10)
    {
        float orange_shade = float(draw_count + 5) / 15.0;
        fs_frag_color = vec4(orange_shade, orange_shade, 0 ,1);
    }
    else if(draw_count > 10 && draw_count <= 50)
    {
        float red_shade = float(draw_count + 5) / 55.0;
        fs_frag_color = vec4(red_shade, 0, 0, 1);
    }
    else
    {
        fs_frag_color = vec4(0, 0, 0, 1);
    }
    




    /*
    // Example of plain depth&stencil texture usage (without the parameter GL_DEPTH_STENCIL_TEXTURE_MODE set to GL_STENCIL_INDEX). 
    // It doesn't allow to extract informations from the stencil component, but only from the depth component.


    // USAGE #1
    // Show depth buffer
    vec4 depth_and_stencil = texture(u_overdraw_depthStencilTex, vs_tex_coords);
    float depth = depth_and_stencil.r; 
    //float depth = depth_and_stencil.g; //same value as .r channel
    //float depth = depth_and_stencil.b; //same value as .r channel

    float depth_k = 10000.0; //implementation depending variable
    float wellDistributed_depth = clamp((1 - depth) * depth_k, 0.0, 1.0);
    fs_frag_color = vec4(wellDistributed_depth, 0, 0, 1);

    
    // USAGE #2
    vec4 depth_and_stencil = texture(u_overdraw_depthStencilTex, vs_tex_coords);
    float depth = depth_and_stencil.r; 

    if(depth >= 0.5, depth <= 0.99)
        fs_frag_color = vec4(1,0,0,1);
    else if(depth > 0.999 && depth < 1.0)
        fs_frag_color = vec4(0,1,0,1);
    else
        fs_frag_color = vec4(0,0,1,1);
    */
}