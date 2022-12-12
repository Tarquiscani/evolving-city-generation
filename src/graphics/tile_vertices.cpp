#include "tile_vertices.hh"



#include <random>

#include "map/direction.h"


namespace tgm
{




TileSubimages::TileSubimages(Vector2i const pos, int const subimages_count)
{
    auto const pptxu = GraphicsSettings::pptxu();
    
    //GIMP coordinates
    auto const top    = pos.y * pptxu;
    auto const bottom = (pos.y + 1) * pptxu;
    auto const left   = pos.x * pptxu;
    auto const right  = (pos.x + 1) * pptxu;

    
    #if GSET_TILESET_TEXARRAY 
        auto const atlas_columns = default_texture_tileset.atlas_width() / static_cast<int>(pptxu);
    #endif
        
    // Distinguish between even and odd number of sprites
    auto const subimages_in_firstRow = subimages_count % 2 == 1   ?   (subimages_count / 2) + 1   :   subimages_count / 2;

    // Load the first row of sprites
    for (int i = 0; i < subimages_in_firstRow; ++i)
    {

        #if GSET_TILESET_TEXARRAY 
            m_layers.push_back(pos.y * atlas_columns + pos.x + i);
        #else
            m_subimages.emplace_back(left + i * pptxu, top, pptxu, pptxu, default_texture_tileset);
        #endif
    }

    // Load the second row of sprites
    #pragma warning(disable: 4189)
    float secondRow_top = top + pptxu;
    #pragma warning(default: 4189)
    for (int i = 0; i < subimages_count - subimages_in_firstRow; ++i)
    {
        #if GSET_TILESET_TEXARRAY 
            m_layers.push_back((pos.y + 1) * atlas_columns + pos.x + i);
        #else
            m_subimages.emplace_back(left + i * pptxu, secondRow_top, pptxu, pptxu, default_texture_tileset);
        #endif
    }

    #if DEBUGLOG
        Logger lgr(g_log);
        lgr << "Loaded subimages for this TileType: " << m_subimages << std::endl;
    #endif
}



BorderSubimages::BorderSubimages(int const n, int const m)
{
    auto const pptxu = GSet::pptxu();
    
    // Relative position of the other border subimages with respect to the leader one (GIMP texture unit reference system).
    std::map<BorderType, Vector2i> relative_positions = {
                { BorderType::solo, { 0, 2} },

                { BorderType::N,    { 0, 2 } },
                { BorderType::E,    { 1, 1 } },
                { BorderType::S,    { 0, 1 } },
                { BorderType::W,    { 1, 2 } },

                { BorderType::NS,   { 0, 0 } },
                { BorderType::EW,   { 1, 0 } },
                { BorderType::NE,   { 2, 1 } },
                { BorderType::ES,   { 2, 0 } },
                { BorderType::SW,   { 3, 0 } },
                { BorderType::NW,   { 3, 1 } },

                { BorderType::NES,  { 4, 2 } },
                { BorderType::NEW,  { 4, 1 } },
                { BorderType::NSW,  { 5, 2 } },
                { BorderType::ESW,  { 4, 0 } },

                { BorderType::NESW, { 6, 2 } },
            };

    #if GSET_TILESET_TEXARRAY 
        auto const atlas_columns = default_texture_tileset.atlas_width() / static_cast<int>(pptxu);
        
        for (auto const p : relative_positions)
        {
            auto const rel_pos = p.second;
            m_layers.insert({ p.first, (m + rel_pos.y) * atlas_columns + (n + rel_pos.x) });
        }

    #else
        //GIMP coordinates
        auto const top    = m * pptxu;
        auto const bottom = (m + 1) * pptxu;
        auto const left   = n * pptxu;
        auto const right  = (n + 1) * pptxu;

        /* OLD RELATIVE POSITIONS INCLUDING THE 3D BUILDINGS EXPERIMENT
        std::map<BorderType, Vector2f> subimage_coords = {

                    // Outdoor borders
                    //no branch
                    { BorderType::solo_outE,  {left + pptxu * 2, top + pptxu * 7} },
                    { BorderType::solo_outW,  {left + pptxu    , top + pptxu * 7} },
                    //one branch
                    { BorderType::N_outE,     {left + pptxu	, top + pptxu * 6} },
                    { BorderType::N_outW,     {left			, top + pptxu * 6} },
                    { BorderType::S_outE,     {left + pptxu , top + pptxu * 5} },
                    { BorderType::S_outW,     {left			, top + pptxu * 5} },
                    //two branches
                    { BorderType::NS_outE,    {left + pptxu    , top + pptxu * 4} },
                    { BorderType::NS_outW,    {left            , top + pptxu * 4} },
                    { BorderType::NE_outNE,   {left + pptxu * 5, top + pptxu * 5} },
                    { BorderType::NE_outSW,   {left + pptxu * 3, top + pptxu * 5} },
                    { BorderType::ES_outSE,   {left + pptxu * 5, top + pptxu * 4} },
                    { BorderType::ES_outNW,   {left + pptxu * 3, top + pptxu * 4} },
                    { BorderType::SW_outNE,   {left + pptxu * 4, top + pptxu * 4} },
                    { BorderType::SW_outSW,   {left + pptxu * 6, top + pptxu * 5} },
                    { BorderType::NW_outSE,   {left + pptxu * 4, top + pptxu * 5} },
                    { BorderType::NW_outNW,   {left + pptxu * 6, top + pptxu * 5} },
                    //three branches
                    { BorderType::NES_outNE,	   {left + pptxu * 7, top + pptxu * 6} },
                    { BorderType::NES_outNE_outW,  {left + pptxu * 5, top + pptxu * 9} },
                    { BorderType::NES_outSE,	   {left + pptxu * 7, top + pptxu * 5} },
                    { BorderType::NES_outSE_outW,  {left + pptxu * 2, top + pptxu * 9} },
                    { BorderType::NES_outW,		   {left + pptxu * 7, top + pptxu * 4} },
                    { BorderType::NEW_outNE,	   {left + pptxu * 9, top + pptxu * 5} },
                    { BorderType::NEW_outNW,	   {left + pptxu * 9, top + pptxu * 7} },
                    { BorderType::NSW_outE,		   {left + pptxu * 8, top + pptxu * 4} },
                    { BorderType::NSW_outE_outSW,  {left + pptxu * 4, top + pptxu * 9} },
                    { BorderType::NSW_outE_outNW,  {left + pptxu * 3, top + pptxu * 9} },
                    { BorderType::NSW_outSW,	   {left + pptxu * 8, top + pptxu * 5} },
                    { BorderType::NSW_outNW,	   {left + pptxu * 8, top + pptxu * 6} },
                    { BorderType::ESW_outSE,	   {left + pptxu * 9, top + pptxu * 4} },
                    { BorderType::ESW_outSW,	   {left + pptxu * 9, top + pptxu * 6} },


                    //four branches
                    { BorderType::NESW_outNE,		{left + pptxu * 12, top + pptxu * 4} },
                    { BorderType::NESW_outNE_outSW,	{left + pptxu * 13, top + pptxu * 6} },
                    { BorderType::NESW_outSE,		{left + pptxu * 12, top + pptxu * 5} },
                    { BorderType::NESW_outSE_outNW,	{left + pptxu * 12, top + pptxu * 6} },
                    { BorderType::NESW_outSW,		{left + pptxu * 13, top + pptxu * 5} },
                    { BorderType::NESW_outNW,		{left + pptxu * 13, top + pptxu * 4} },



                    // Outdoors multi-floor
                    { BorderType::solo_outS_multi,		 {left            , top + pptxu * 8} },	
                    { BorderType::E_outS_multi,			 {left + pptxu * 2, top + pptxu * 5} },
                    { BorderType::W_outS_multi,			 {left + pptxu * 2, top + pptxu * 6} },

                    { BorderType::EW_outS_multi,		 {left + pptxu * 2, top + pptxu * 4} },
                    { BorderType::NE_outSW_multi,		 {left + pptxu * 3, top + pptxu * 6} },
                    { BorderType::NW_outSE_multi,		 {left + pptxu * 4, top + pptxu * 6} },
                    { BorderType::ES_outSE_multi,		 {left + pptxu * 5, top + pptxu * 6} },
                    { BorderType::SW_outSW_multi,		 {left + pptxu * 6, top + pptxu * 6} },

                    { BorderType::NES_outSE_multi,		 {left + pptxu * 7, top + pptxu * 7} },
                    { BorderType::NES_outSE_outW_multi,  {left + pptxu * 2, top + pptxu * 10} },
                    { BorderType::NEW_outNE_outS_multi,  {left            , top + pptxu * 10} },
                    { BorderType::NEW_outS_multi,		 {left + pptxu * 9, top + pptxu * 10} },
                    { BorderType::NEW_outS_outNW_multi,  {left + pptxu * 7, top + pptxu * 10} },
                    { BorderType::NSW_outE_outSW_multi,  {left + pptxu * 4, top + pptxu * 10} },
                    { BorderType::NSW_outSW_multi,		 {left + pptxu * 8, top + pptxu * 7} },
                    { BorderType::ESW_outSE_multi,		 {left + pptxu * 9, top + pptxu * 8} },
                    { BorderType::ESW_outSW_multi,		 {left + pptxu * 9, top + pptxu * 9} },
                    
                    { BorderType::NESW_outNE_outSW_multi, {left + pptxu * 13, top + pptxu * 8} },
                    { BorderType::NESW_outSE_multi,		  {left + pptxu * 12, top + pptxu * 7} },
                    { BorderType::NESW_outSE_outNW_multi, {left + pptxu * 12, top + pptxu * 8} },
                    { BorderType::NESW_outSW_multi,		  {left + pptxu * 13, top + pptxu * 7} },
                };
        */

        for (auto const p : relative_positions)
        {
            auto const rel_pos = p.second;
            m_subimages.insert({ p.first , {left + (rel_pos.x * pptxu), top + (rel_pos.y * pptxu), pptxu, pptxu, default_texture_tileset} });
        }
    #endif

            
    #if DEBUGLOG
        Logger lgr(g_log);
        lgr << "Loaded subimages for this border type: " << m_subimages << std::endl;
    #endif
}

void TileVertices::reset(int const map_length, int const map_width, int const map_height)
{
    assert_uninitializedOrSynchronized();

    auto const new_tileCount = static_cast<VertCont::size_type>(map_length) * static_cast<VertCont::size_type>(map_width) * static_cast<VertCont::size_type>(map_height); //cast needed to avoid overflows
    if (new_tileCount % GraphicsSettings::chunkSize_inTile != 0u) 
    { 
        auto oss = std::ostringstream{}; oss << "new_tileCount must be a multiple of " << GraphicsSettings::chunkSize_inTile;
        throw std::runtime_error(oss.str());
    }
    
    m_map_length = map_length;
    m_map_width = map_width;
    m_map_height = map_height;

    m_vertices.clear();
    m_vertices.reserve(new_tileCount * triangles_per_tile * vertices_per_triangle);
    
    init_polygons();

    cache_random0to9NumberSequence();

    m_state = TileVerticesState::reset;
}

void TileVertices::init_polygons()
{
    auto const t_height = GraphicsSettings::floors_distance();

    for (int z = 0; z < m_map_height; ++z)
    {
        for (int y = 0; y < m_map_width; ++y)
        {
            for (int x = 0; x < m_map_length; ++x)
            {
                // World space coordinates.
                auto wz = z * t_height;

                auto wy_sliding = GSet::tiles_to_units(z) * GSet::wySliding_ratio();

                auto wleft   = GSet::tiles_to_units(y),
                     wright  = GSet::tiles_to_units(y + 1),
                     wtop    = -(GSet::tiles_to_units(x)) + wy_sliding,
                     wbottom = -(GSet::tiles_to_units(x + 1)) + wy_sliding;


                //TODO: 07: Unify the way to compute and assign the entity_id, because it's also used from FreeTriangles and from dynamic sprites
                auto temp_id = compute_entityId(x, y, z);

                #if DYNAMIC_ASSERTS
                if (temp_id <= 0)
                    throw std::runtime_error("A negative or null entity_id is not allowed.");
                #endif

                auto entity_id = static_cast<GLuint>(temp_id);


                // Top-left triangle
                //top-left vertex
                m_vertices.push_back({ {wleft,  wtop,    wz}, {0.f, 0.f}, 0u, entity_id });
                //top-right vertex
                m_vertices.push_back({ {wright, wtop,    wz}, {0.f, 0.f}, 0u, entity_id });
                //bottom-left vertex
                m_vertices.push_back({ {wleft,  wbottom, wz}, {0.f, 0.f}, 0u, entity_id });


                // Bottom-right triangle
                //top-right vertex
                m_vertices.push_back({ {wright, wtop,    wz}, {0.f, 0.f}, 0u, entity_id });
                //bottom-right vertex
                m_vertices.push_back({ {wright, wbottom, wz}, {0.f, 0.f}, 0u, entity_id });
                //bottom-left vertex
                m_vertices.push_back({ {wleft,  wbottom, wz}, {0.f, 0.f}, 0u, entity_id });
            }
        }
    }
}
//TODO: 99: Perchè accetta tile_type, border_type e border_style? Non potrebbe accettare solo qualcosa tipo TileGraphics e a TileGraphicsManager il compito di decidere?
void TileVertices::set_tileGraphics(int const x, int const y, int const z, 
                                    bool const is_border, TileType const tile_type, BorderType const border_type, BorderStyle const border_style)
{
    assert_initialization();

    if (is_border)
    {
        if (border_type == BorderType::none || border_style == BorderStyle::none)
        {
            throw std::runtime_error("border_type or border_style are not properly set.");
        }

        auto style_it = border_sprites.find(border_style);
        if (style_it == border_sprites.cend())
        {
            throw std::runtime_error("No sprites found for this BorderStyle.");
        }
                
        #if GSET_TILESET_TEXARRAY
            auto tex_layer = style_it->second.get_layer(border_type);

            #if FREE_ASSETS
                    tex_layer = 1; //transparent tile				
            #endif

            set_tileTexture(x, y, z, 0.f, 0.f, 1.f, 1.f, tex_layer);			
        #else
            auto tex_subimage = style_it->second.get_subimage(border_type);
            set_tileTexture(x, y, z, tex_subimage.left, tex_subimage.bottom, tex_subimage.right, tex_subimage.top, 0u);
        #endif
    }
    else
    {
        if (tile_type == TileType::none)
        {
            throw std::runtime_error("tile_type is not properly set.");
        }

        auto it = flat_sprites.find(tile_type);
        if (it == flat_sprites.cend())
        {
            throw std::runtime_error("No sprites found for this TileType.");
        }

                
        #if GSET_TILESET_TEXARRAY
            auto tex_layer = it->second.get_layer(get_random_0to9(x, y));
            
            #if FREE_ASSETS
                if(tile_type == TileType::ground || tile_type == TileType::underground)
                    tex_layer = 0;
                else
                    tex_layer = 1; //transparent tile
            #endif

            set_tileTexture(x, y, z, 0.f, 0.f, 1.f, 1.f, tex_layer);
        #else
            auto tex_subimage = it->second.get_subimage(get_random_0to9(x, y));
        
            #if FREE_ASSETS
                    tex_subimage = TextureSubimage(216.f, 0.f, 216.f, 216.f, default_texture_dynamics); //transparent tile
                if(tile_type == TileType::ground || tile_type == TileType::underground)
                    tex_subimage = TextureSubimage(0.f, 0.f, 216.f, 216.f, default_texture_dynamics);
                else
                    tex_subimage = TextureSubimage(216.f, 0.f, 216.f, 216.f, default_texture_dynamics); //transparent tile
            #endif

            set_tileTexture(x, y, z, tex_subimage.left, tex_subimage.bottom, tex_subimage.right, tex_subimage.top, 0u);			
        #endif


        //////////////////////////////
        // TEXTURE DEFINITION TESTS:
        //--------------------------
        
        //TODO: From the results of the following tests, I can conclude that for achieving a "pixel perfect" representation of
        //		the texture on the screen I should pick a different point inside the pixel. Such a point depends on the level of
        //		zoom the texture should have with respect to screen resolution. I may change that point in function of the level of zoom
        //		and in function of the screen resolution. But since I'll make different textures LowDef vs HD vs UltraHD, for now I won't
        //		develop such a system, because the problem is quite unnoticeable.

        //// Defined from the first quarter of the left pixel to the third quarter of the right pixel 
        //// and, respectively, from the first quarter of the bottom pixel to the third quarter of the top pixel.
        //// QUARTER PIXEL .|...  -------------->  ...|. QUARTER PIXEL
        //// When running in UHD mode, the result is pixel perfect both for "zoom = 1.0" and for "zoom = 2.0".
        //set_tileTexture(x, y, z, 0.06109619140625f, 0.93902587890625f, 0.07318115234375f, 0.95111083984375f);  
        

        //// Defined from the half of the left pixel to the half of the right pixel 
        //// and, respectively, from the half of the bottom pixel to the half of the top pixel.
        ////	HALF PIXEL ..|..  -------------->  ..|.. HALF PIXEL
        ////
        ////	When running in UHD mode, the result is pixel perfect for "zoom = 2.0" but for "zoom = 1.0", 
        ////	in fact in the latter case only the second half of the first pixel is shown and only the first half 
        ////	of the last pixel, then the 48 pixel in between are stretched to fit the 98 pixel left on the screen.
        //set_tileTexture(x, y, z, 0.0611572265625f, 0.9390869140625f, 0.0731201171875f, 0.9510498046875f); 


        //// Defined from the beginning of the left pixel to the end of the right pixel 
        //// and, respectively, from beginning the bottom pixel to the end of the top pixel.
        ////	BEGINNING |....  -------------->  ....| END
        ////
        ////	The result it's not good for certain level of zoom. In fact may happen that a pixel outside the subimage is
        ////	picked in the place of the boundary pixel of the subimage.
        //set_tileTexture(x, y, z, 0.06103515625f, 0.93896484375f, 0.0732421875f, 0.951171875f);  
        //      

        //--------------------------
        //--------------------------
        //////////////////////////////
    }


    m_changed_chunks.insert(compute_index(x, y, z) / chunk_size);
}

void TileVertices::set_tileTexture(int const x, int const y, int const z, 
                                   float const tex_left, float const tex_bottom, float const tex_right, float const tex_top, GLuint const texarray_layer)
{
    auto index = compute_index(x, y, z);

    if (index >= m_vertices.size())
        throw std::runtime_error("TileVertices index overflow...");

    auto starting_vertex = &m_vertices[index];


    //Top-left triangle
    //top-left vertex
    auto & v0 = starting_vertex[0];
    v0.tex_coords[0] = tex_left;
    v0.tex_coords[1] = tex_top;
    v0.layer = texarray_layer;
    //top-right vertex
    auto & v1 = starting_vertex[1];
    v1.tex_coords[0] = tex_right;
    v1.tex_coords[1] = tex_top;
    v1.layer = texarray_layer;
    //bottom-left vertex
    auto & v2 = starting_vertex[2];
    v2.tex_coords[0] = tex_left;
    v2.tex_coords[1] = tex_bottom;
    v2.layer = texarray_layer;

            
    // Bottom-right triangle
    //top-right vertex
    auto & v3 = starting_vertex[3];
    v3.tex_coords[0] = tex_right;
    v3.tex_coords[1] = tex_top;
    v3.layer = texarray_layer;
    //bottom-right vertex
    auto & v4 = starting_vertex[4];
    v4.tex_coords[0] = tex_right;
    v4.tex_coords[1] = tex_bottom;
    v4.layer = texarray_layer;
    //bottom-left vertex
    auto & v5 = starting_vertex[5];
    v5.tex_coords[0] = tex_left;
    v5.tex_coords[1] = tex_bottom;
    v5.layer = texarray_layer;
}


auto TileVertices::get_changes() const -> std::vector<std::pair<GLintptr, TilesetVertexData const*>>
{
    assert_synchronized();

    std::vector<std::pair<GLintptr, TilesetVertexData const*>> changes;

    for (auto chunk : m_changed_chunks)
    {
        auto const offset = static_cast<GLintptr>(chunk * chunk_byteSize());
        auto const ptr = &m_vertices[chunk * chunk_size];

        changes.push_back({offset, ptr});
    }

    return changes;
}


void TileVertices::debug_print(int /*sprite_start*/, int /*sprite_end*/) const
{
    assert_initialization();

    Logger lgr(g_log);
/*
    auto f = get_ptr();

    lgr << "Vertices size: " << m_attributes.size() << std::endl;

    for (int k = sprite_start; k < sprite_end; ++k)
    {
        int i = k * 30;

        lgr << Logger::nltb << "Tile (" << (- f[i + 1]) << ", " << f[i + 0] << ", " << f[i + 2] << ")"
            << Logger::addt
            << Logger::nltb << "Top-left triangle"
            << Logger::addt
            << Logger::nltb << "top-left vertex    :"	<< f[i + 0] << "   " << f[i + 1] << "   " << f[i + 2] << "   " << f[i + 3] << "   " << f[i + 4]
            << Logger::nltb << "top-right vertex   :"	<< f[i + 5] << "   " << f[i + 6] << "   " << f[i + 7] << "   " << f[i + 8] << "   " << f[i + 9]
            << Logger::nltb << "bottom-left vertex :"	<< f[i + 10] << "   " << f[i + 11] << "   " << f[i + 12] << "   " << f[i + 13] << "   " << f[i + 14]
            << Logger::remt
            << Logger::nltb << "Bottom-right triangle"
            << Logger::addt
            << Logger::nltb << "top-right vertex   :"	<< f[i + 15] << "   " << f[i + 16] << "   " << f[i + 17] << "   " << f[i + 18] << "   " << f[i + 19]
            << Logger::nltb << "bottom-right vertex:"	<< f[i + 20] << "   " << f[i + 21] << "   " << f[i + 22] << "   " << f[i + 23] << "   " << f[i + 24]
            << Logger::nltb << "bottom-left vertex :"	<< f[i + 25] << "   " << f[i + 26] << "   " << f[i + 27] << "   " << f[i + 28] << "   " << f[i + 29]
            << Logger::remt
            << Logger::remt
            << Logger::nltb
            << Logger::nltb
            ;

                
        lgr << Logger::nltb << "Tile (" << (- f[i + 1]) << ", " << f[i + 0] << ", " << f[i + 2] << ")"
            << Logger::addt
            << Logger::nltb << "Top-left triangle"
            << Logger::addt
            << Logger::nltb << "top-left vertex    :"	<< tx(f[i + 1])  << "   " << ty(f[i + 0]) << "   " << f[i + 2] << "   " << tu(f[i + 4]) << "   " << tv(f[i + 3])
            << Logger::nltb << "top-right vertex   :"	<< tx(f[i + 6])  << "   " << ty(f[i + 5]) << "   " << f[i + 7] << "   " << tu(f[i + 9]) << "   " << tv(f[i + 8])
            << Logger::nltb << "bottom-left vertex :"	<< tx(f[i + 11]) << "   " << ty(f[i + 10]) << "   "<< f[i + 12] << "   "<< tu(f[i + 14]) << "   " << tv(f[i + 13])
            << Logger::remt
            << Logger::nltb << "Bottom-right triangle"
            << Logger::addt
            << Logger::nltb << "top-right vertex   :"	<< f[i + 15] << "   " << f[i + 16] << "   " << f[i + 17] << "   " << f[i + 18] << "   " << f[i + 19]
            << Logger::nltb << "bottom-right vertex:"	<< f[i + 20] << "   " << f[i + 21] << "   " << f[i + 22] << "   " << f[i + 23] << "   " << f[i + 24]
            << Logger::nltb << "bottom-left vertex :"	<< f[i + 25] << "   " << f[i + 26] << "   " << f[i + 27] << "   " << f[i + 28] << "   " << f[i + 29]
            << Logger::remt
            << Logger::remt
            << Logger::nltb
            << Logger::nltb
            ;
    }*/

    lgr << std::endl;

    int stop;
    std::cin >> stop;
}

void TileVertices::cache_random0to9NumberSequence()
{
    std::mt19937 rand(82);
    //TODO: Create a good random sequence of number capable of representing a matrix
    for (auto x = 0; x < m_map_length; ++x)
    {
        for (auto y = 0; y < m_map_width; ++y)
        {
            auto r_num = 0;

            auto const current = Vector2i{ x, y };

            // Get only the neighbors that have already been pushed in random_numberSequence
            auto const nghbN  = current + Versor2i::N; 
            auto const nghbNE = current + Versor2i::NE;
            auto const nghbW  = current + Versor2i::W;
            auto const nghbNW = current + Versor2i::NW;
            

            // Skip random numbers until finding a number not used by the neighboors
            do {
                r_num = rand() % 10;
                        //short-circuit to avoid checking unexistent neighbors
            } while(	( contains(nghbN)  && r_num == random_0to9_numberSequence[static_cast<RandCont::size_type>(nghbN.x)  * m_map_width + nghbN.y ] ) ||
                        ( contains(nghbNE) && r_num == random_0to9_numberSequence[static_cast<RandCont::size_type>(nghbNE.x) * m_map_width + nghbNE.y] ) ||
                        ( contains(nghbW)  && r_num == random_0to9_numberSequence[static_cast<RandCont::size_type>(nghbW.x)  * m_map_width + nghbW.y ] ) ||
                        ( contains(nghbNW) && r_num == random_0to9_numberSequence[static_cast<RandCont::size_type>(nghbNW.x) * m_map_width + nghbNW.y] )	 );

            random_0to9_numberSequence.push_back(r_num);
        }
    }
}



////Useful to compare the actual memory usage with the predicted one.
//void allocatedMemory_forecast()
//{
//    int tiles_count = Map::LENGTH * Map::WIDTH * Map::HEIGHT;
//
//    g_log << "sizeof(char)"<< sizeof(char) << std::endl;
//    g_log << "sizeof(Tile)"<< sizeof(Tile) << std::endl;
//    g_log << "sizeof(Tile*)" << sizeof(Tile*) << std::endl;
//    g_log << "Allocated memory:" << (sizeof(Tile) + sizeof(Tile*)*27 )*tiles_count << std::endl;
//
//    std::allocator<Tile> alloc;
//    Tile* tiles = alloc.allocate(tiles_count);
//    for(auto p=tiles; p!=tiles+tiles_count; ++p)
//        alloc.construct(p, Vector3i(0,0,0), TileType::ground);
//    g_log << "Tile neighbor capacity: " << tiles->neighbors.capacity();
//
//    int pause;
//    std::cin>>pause;
//
//    for(auto p=tiles; p!=tiles+tiles_count; ++p)
//        alloc.destroy(p);
//    alloc.deallocate(tiles, tiles_count);
//}



} //namespace tgm