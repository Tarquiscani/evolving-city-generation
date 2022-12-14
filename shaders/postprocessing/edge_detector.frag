#version 430 core


////
//	
//	This shader uses a texture where each pixel corresponds to an edgeable_id. Then it creates black edges between 
//	the areas who have an homogenous id.
//
////


out vec4 fs_frag_color;

in vec2 vs_tex_coords;

//Texture containing in each texel the edgeable_id of a given polygon.
layout (binding = 16) uniform usampler2D u_edgeableIds_tex;


//definitions linked to the sobel filter example
/*mat3 sx = mat3 ( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);

mat3 sy = mat3 ( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);*/

void main()
{
    // edgeable_id of the central pixel
    uint central_id = texelFetch(u_edgeableIds_tex, ivec2(gl_FragCoord), 0 ).r;

    // Initially this pixel is white
    float g = 1.0;
    



    // METHOD 1
    // The following method takes into account only the N, E, S, W neighboring pixels. 
    // It's useful to avoid differences in the thickness of diagonal vs orthogonal edges.

    // edgeable_ids of the neighboring pixels
    uint N_id = texelFetch(u_edgeableIds_tex, ivec2(gl_FragCoord) + ivec2( 0,-1), 0 ).r;
    uint E_id = texelFetch(u_edgeableIds_tex, ivec2(gl_FragCoord) + ivec2( 1, 0), 0 ).r;
    uint S_id = texelFetch(u_edgeableIds_tex, ivec2(gl_FragCoord) + ivec2( 0, 1), 0 ).r;
    uint W_id = texelFetch(u_edgeableIds_tex, ivec2(gl_FragCoord) + ivec2(-1, 0), 0 ).r;
    
    // If there is at least one pixel who has an edgeable_id different from that of the central pixel, then this pixel is colored black.
    g =   float(central_id == N_id) 
        * float(central_id == E_id)
        * float(central_id == S_id) 
        * float(central_id == W_id);
    

    /*
    // METHOD 2
    // Whereas the following method takes into account all the neighboring pixels. 
    // It has a small flaw, because the thickness of the diagonal edges results a little bigger than that of the orthogonal edges.
    for (int i=0; i<3; i++) 
    {
        for (int j=0; j<3; j++) 
        {
            uint current_id = texelFetch(u_edgeableIds_tex, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).r;

            // If the edgeable_ids of the central pixel and that of the current pixel are the same, then this pixel remains white,
            // otherwise this pixel becomes black.
            g *= float(central_id == current_id);

            // It's equivalent to:
            //if((central_id - current_id) != 0)
            //{
            //	g = 0.0;
            //}
        }
    }
    */
    


    fs_frag_color = vec4(vec3(g), 1.0);







    //-------------------- SOBEL FILTER EXAMPLE (not used here) -------------------------//
    
    // Sobel filter example (unuseful since I only need black or white pixel when the surrounding pixels have different colors)
    /*
    //vec3 diffuse = texture(u_untextured_colorTex, vs_tex_coords).rgb;
    mat3 I;
    for (int i=0; i<3; i++) 
    {
        for (int j=0; j<3; j++) 
        {
            vec3 mysample = texelFetch(u_untextured_colorTex, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
            I[i][j] = length(mysample); 
        }
    }

    float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]); 
    float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);
    

    float g = sqrt( pow(gx, 2.0) + pow(gy, 2.0) );
    
    //make the pixel either black or white, getting rid of grey shades
    g = step(0.01, g); 
    
    //fs_frag_color = vec4(diffuse - vec3(g), 1.0);
    fs_frag_color = vec4(vec3(1 - g), 1.0);

    */
    
    //-----------------------------------------------------------------------------------//


} 