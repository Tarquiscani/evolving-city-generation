#version 430 core

////
//	
//	This shader is a fundamental step of the occlusion culling system.
//	It's used to build, starting from the texture of the visible tiles (where each texel is an entity_id),
//	a buffer containing only the visible entities.
//
////


out vec4 fs_unused_outputColor;

in vec2 vs_tex_coords;



layout(binding = 10) uniform usampler2D u_entityIds_tex;

// Buffer that will register all the entites already copied in the SSBO_visible_tiles, used to avoid duplicates.
layout(std430, binding = 10) buffer evaluated_entities
{
    uint SSBO_evaluated_entities[];
};

// Counter used to make the indirect draw
layout(binding = 0, offset = 0) uniform atomic_uint ABO_visibleVertices_count;

//TODO: PERFORMANCE: Using OpenGL 4.6 I could get rid of the following atomic counter, I could use only ABO_visibleVertices_count in conjuntion with atomicCounterAdd.
//					 But in order to make the game compatible with the integrated GPU, I had to use a prior version of OpenGL, the 4.5, so that function isn't available. 
//					 In the future it's better to switch to that single variable design getting rid of the following counter.
layout(binding = 1, offset = 0) uniform atomic_uint ABO_visibleTiles_count;


struct TilesetVertexData
{
    float position[3];

    float tex_coords[2];

    uint layer;

    uint entity_ID;
};

// Buffer, determined in CPU, containing all the tiles of the map
layout(std430, binding = 11) buffer tileBuffer_SSBO
{
    TilesetVertexData tiles[];
};


// Buffer filled in this fragment shader with the visible tiles of the map.
layout(std430, binding = 12) buffer visibleTileBuffer_SSBO
{
    TilesetVertexData visible_tiles[];
};



void main()
{
    // Retrieve from the texture of the visible tiles, the entity id associated to this tile.
    uint entity_id = texture(u_entityIds_tex, vec2(vs_tex_coords.x, vs_tex_coords.y)).r;
    

    // Check the corresponding flag of the analyzed entities
    uint index = entity_id / 32;
    uint bit =  uint(0x1) << (entity_id % 32);

    uint previous_value = atomicOr(SSBO_evaluated_entities[index], bit); // It's needed an atomic operation because all the fragment shaders could simultaneuosly access
                                                                         // this integer (note that a |= operation is both a read and write operation). If the memory access 
                                                                         // is incoherent, the result is messy as well.

    uint TEST_v0_index = 0;
    uint TEST_vertexId = 0;
    
    uint TEST_visibleTiles_count = 0;
    uint TEST_visibleVertices_count = 0;


    // If the entity has not been copied yet
    if((previous_value & bit) == 0)
    {
        //// Simultaneously update the count of the visible vertices and retrieve an unused index for the visible tile vertices.
        //uint v0_index = atomicCounterAdd(ABO_visibleVertices_count, 6);
        
        //-------------
        //TODO: PERFORMANCE: Using OpenGL 4.6 I could get rid of the following atomic counter, I could use only ABO_visibleVertices_count in conjuntion with atomicCounterAdd.
        //					 But in order to make the game compatible with the integrated GPU, I had to use a prior version of OpenGL, the 4.5, so that function isn't available. 
        //					 In the future it's better to switch to that single variable design getting rid of the following counter.

        // Find an unused index for the visibile tile buffer.
        uint v0_index = atomicCounterIncrement(ABO_visibleTiles_count) * 6;
        
        
        // Update the visible vertices count with the new 6 vertices of the current tile
        atomicCounterIncrement(ABO_visibleVertices_count);
        atomicCounterIncrement(ABO_visibleVertices_count);
        atomicCounterIncrement(ABO_visibleVertices_count);
        atomicCounterIncrement(ABO_visibleVertices_count);
        atomicCounterIncrement(ABO_visibleVertices_count);
        atomicCounterIncrement(ABO_visibleVertices_count);
        //-------------
        
        uint tile_index = entity_id - 1; //"+1" was added to avoid "entity_id == 0"

        visible_tiles[v0_index]     = tiles[tile_index * 6];
        visible_tiles[v0_index + 1] = tiles[tile_index * 6 + 1];
        visible_tiles[v0_index + 2] = tiles[tile_index * 6 + 2];
        visible_tiles[v0_index + 3] = tiles[tile_index * 6 + 3];
        visible_tiles[v0_index + 4] = tiles[tile_index * 6 + 4];
        visible_tiles[v0_index + 5] = tiles[tile_index * 6 + 5];



        
        TEST_v0_index = v0_index;
        TEST_vertexId = tile_index * 6u + 5u;
    
        TEST_visibleTiles_count = atomicCounter(ABO_visibleTiles_count);
        TEST_visibleVertices_count = atomicCounter(ABO_visibleVertices_count);
    }

    float color = float(TEST_v0_index) / 15450.f;
    if(TEST_v0_index > 92000)
        fs_unused_outputColor = vec4(1, 0, 0, 1);
    else
        fs_unused_outputColor = vec4(0, 0, 0, 1);
}