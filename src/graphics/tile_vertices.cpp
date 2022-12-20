#include "tile_vertices.hh"


#include <random>

#include "map/direction.h"
#include "utilities.hh"

#include "debug/asserts.hh"


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

BorderBackgroundSubimages::BorderBackgroundSubimages(Vector2i const offset)
{    
    // Relative position of the other border background subimages with respect to the starting one (GIMP texture unit reference system).
    auto const relative_positions = std::map<BorderBackgroundType, Vector2i>{
                { BorderBackgroundType::NoSides,                {  0, 0} },
                { BorderBackgroundType::SideCutTopShortLeft,    {  1, 0} },
                { BorderBackgroundType::SideCutTopShortRight,   {  2, 0} },
                { BorderBackgroundType::SideCutTopShortFull,    {  3, 0} },
                { BorderBackgroundType::SideCutTopLongLeft,     {  4, 0} },
                { BorderBackgroundType::SideCutTopLongRight,    {  5, 0} },
                { BorderBackgroundType::SideCutTopLongFull,     {  6, 0} },
                { BorderBackgroundType::SideFull,               {  7, 0} },
                { BorderBackgroundType::SideCutBottomRight,     {  8, 0} },
                { BorderBackgroundType::SideCutBottomLeft,      {  9, 0} },
                { BorderBackgroundType::SideCutBottomFull,      { 10, 0} }
            };

    add_subimages(offset, relative_positions);
            
    #if DEBUGLOG
        auto lgr = Logger{ g_log };
        lgr << "Loaded subimages for this background style: " << m_subimages << std::endl;
    #endif
}

BorderSectionSubimages::BorderSectionSubimages(Vector2i const offset)
{
    // Relative position of the other border background subimages with respect to the starting one (GIMP texture unit reference system).
    auto const relative_positions = std::map<BorderSectionType, Vector2i>{
                { BorderSectionType::NoSection,                                 {  0,  0 } },
                { BorderSectionType::NoSectionShadowLeft,                       {  0,  1 } },
                { BorderSectionType::NoSectionShadowRight,                      {  0,  2 } },
                { BorderSectionType::NoSectionShadowFront,                      {  0,  3 } },
                { BorderSectionType::NoSectionShadowLeftShadowRight,            {  0,  4 } },
                { BorderSectionType::NoSectionShadowLeftShadowFront,            {  0,  5 } },
                { BorderSectionType::NoSectionShadowRightShadowFront,           {  0,  6 } },
                { BorderSectionType::NoSectionShadowLeftShadowRightShadowFront, {  0,  7 } },

                { BorderSectionType::SectionBehind,                             {  1,  0 } },
                { BorderSectionType::SectionBehindShadowFront,                  {  1,  1 } },
                { BorderSectionType::SectionBehindShadowBehind,                 {  1,  2 } },
                { BorderSectionType::SectionBehindShadowFrontShadowBehind,      {  1,  3 } },

                { BorderSectionType::SectionFront,                              {  2,  0 } },
                { BorderSectionType::SectionFrontShadowLeft,                    {  2,  1 } },
                { BorderSectionType::SectionFrontShadowRight,                   {  2,  2 } },
                { BorderSectionType::SectionFrontShadowLeftShadowRight,         {  2,  3 } },

                { BorderSectionType::SectionBehindFront,                        {  3,  0 } },
                { BorderSectionType::SectionBehindFrontShadowBehind,            {  3,  1 } },

                { BorderSectionType::SectionBehindLeft,                         {  4,  0 } },
                { BorderSectionType::SectionBehindLeftShadowFront,              {  4,  1 } },

                { BorderSectionType::SectionBehindRight,                        {  5,  0 } },
                { BorderSectionType::SectionBehindRightShadowFront,             {  5,  1 } },

                { BorderSectionType::SectionFrontBehindLeft,                    {  6,  0 } },
                { BorderSectionType::SectionFrontLeft,                          {  6,  1 } },

                { BorderSectionType::SectionFrontBehindRight,                   {  7,  0 } },
                { BorderSectionType::SectionFrontRight,                         {  7,  1 } },

                { BorderSectionType::SectionBehindRightLeft,                    {  8,  0 } },
                { BorderSectionType::SectionBehindRightLeftShadowFront,         {  8,  1 } },
                { BorderSectionType::SectionRightLeft,                          {  8,  2 } },
                { BorderSectionType::SectionRightLeftShadowFront,               {  8,  3 } },
                { BorderSectionType::SectionRight,                              {  8,  4 } },
                { BorderSectionType::SectionRightShadowFront,                   {  8,  5 } },
                { BorderSectionType::SectionLeft,                               {  8,  6 } },
                { BorderSectionType::SectionLeftShadowFront,                    {  8,  7 } },
                
                { BorderSectionType::SectionFrontBehindRightLeft,               {  9,  0 } },
                { BorderSectionType::SectionFrontRightLeft,                     {  9,  1 } },
            };

    add_subimages(offset, relative_positions);
            
    #if DEBUGLOG
        auto lgr = Logger{ g_log };
        lgr << "Loaded subimages for this background style: " << m_subimages << std::endl;
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

    cache_random_0_to_9_sequence();

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
                m_vertices.push_back({ {wleft,  wtop,    wz}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, 0u, 0u, 0u, entity_id });
                //top-right vertex
                m_vertices.push_back({ {wright, wtop,    wz}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, 0u, 0u, 0u, entity_id });
                //bottom-left vertex
                m_vertices.push_back({ {wleft,  wbottom, wz}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, 0u, 0u, 0u, entity_id });


                // Bottom-right triangle
                //top-right vertex
                m_vertices.push_back({ {wright, wtop,    wz}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, 0u, 0u, 0u, entity_id });
                //bottom-right vertex
                m_vertices.push_back({ {wright, wbottom, wz}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, 0u, 0u, 0u, entity_id });
                //bottom-left vertex
                m_vertices.push_back({ {wleft,  wbottom, wz}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, 0u, 0u, 0u, entity_id });
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
        // Check that border_type and border_style are properly set
        check(border_type.background != BorderBackgroundType::None && border_type.section != BorderSectionType::None
              /*&& border_type.corner_shadow != BorderCornerShadowType::None*/ && border_style != BorderStyle::none);

        auto background_subimages_it = border_background_subimages.find(border_style);
        check(background_subimages_it != border_background_subimages.cend())           // No sprites found for this BorderStyle.

        auto section_subimages_it = border_section_subimages.find(border_style);
        check(section_subimages_it != border_section_subimages.cend())           // No sprites found for this BorderStyle.
                
        #if GSET_TILESET_TEXARRAY
            auto const background_layer = background_subimages_it->second.get_layer(border_type.background);
            auto const section_layer = section_subimages_it->second.get_layer(border_type.section);

            #if GSET_ALTERNATIVE_ASSETS
                    background_layer = 1; //transparent tile			
                    section_layer = 1;
            #endif

            set_tile_texture(x, y, z, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, background_layer, section_layer, 0u);			
        #else
            auto const background_subimage = background_subimages_it->second.get_subimage(border_type.background);
            auto const section_subimage = section_subimages_it->second.get_subimage(border_type.section);
            set_tile_texture(x, y, z, 
                             background_subimage.left, background_subimage.bottom, background_subimage.right, background_subimage.top, 
                             section_subimage.left, section_subimage.bottom, section_subimage.right, section_subimage.top, 
                             0.f, 0.f, 0.f, 0.f, 
                             0u, 0u, 0u);
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
            auto background_layer = it->second.get_layer(get_random_0to9(x, y));
            
            #if GSET_ALTERNATIVE_ASSETS
                if(tile_type == TileType::ground || tile_type == TileType::underground)
                {
                    background_layer = 0;
                }
                else
                {
                    background_layer = 1; //transparent tile
                }
            #endif

            set_tile_texture(x, y, z, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, background_layer, 0u, 0u);
        #else
            auto background_subimage = it->second.get_subimage(get_random_0to9(x, y));
        
            #if GSET_ALTERNATIVE_ASSETS
                    background_subimage = TextureSubimage(216.f, 0.f, 216.f, 216.f, default_texture_dynamics); //transparent tile
                if(tile_type == TileType::ground || tile_type == TileType::underground)
                    background_subimage = TextureSubimage(0.f, 0.f, 216.f, 216.f, default_texture_dynamics);
                else
                    background_subimage = TextureSubimage(216.f, 0.f, 216.f, 216.f, default_texture_dynamics); //transparent tile
            #endif

            set_tile_texture(x, y, z, 
                             background_subimage.left, background_subimage.bottom, background_subimage.right, background_subimage.top, 
                             0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
                             0u, 0u, 0u);			
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
        //set_tile_texture(x, y, z, 0.06109619140625f, 0.93902587890625f, 0.07318115234375f, 0.95111083984375f);  
        

        //// Defined from the half of the left pixel to the half of the right pixel 
        //// and, respectively, from the half of the bottom pixel to the half of the top pixel.
        ////	HALF PIXEL ..|..  -------------->  ..|.. HALF PIXEL
        ////
        ////	When running in UHD mode, the result is pixel perfect for "zoom = 2.0" but for "zoom = 1.0", 
        ////	in fact in the latter case only the second half of the first pixel is shown and only the first half 
        ////	of the last pixel, then the 48 pixel in between are stretched to fit the 98 pixel left on the screen.
        //set_tile_texture(x, y, z, 0.0611572265625f, 0.9390869140625f, 0.0731201171875f, 0.9510498046875f); 


        //// Defined from the beginning of the left pixel to the end of the right pixel 
        //// and, respectively, from beginning the bottom pixel to the end of the top pixel.
        ////	BEGINNING |....  -------------->  ....| END
        ////
        ////	The result it's not good for certain level of zoom. In fact may happen that a pixel outside the subimage is
        ////	picked in the place of the boundary pixel of the subimage.
        //set_tile_texture(x, y, z, 0.06103515625f, 0.93896484375f, 0.0732421875f, 0.951171875f);  
        //      

        //--------------------------
        //--------------------------
        //////////////////////////////
    }


    m_changed_chunks.insert(compute_index(x, y, z) / chunk_size);
}

void TileVertices::set_tile_texture(int const x, int const y, int const z, 
                                    float const background_sprite_coord_left, float const background_sprite_coord_bottom, 
                                    float const background_sprite_coord_right, float const background_sprite_coord_top, 
                                    float const section_sprite_coord_left, float const section_sprite_coord_bottom, 
                                    float const section_sprite_coord_right, float const section_sprite_coord_top, 
                                    float const corner_shadow_sprite_coord_left, float const corner_shadow_sprite_coord_bottom, 
                                    float const corner_shadow_sprite_coord_right, float const corner_shadow_sprite_coord_top, 
                                    GLuint const background_sprite_layer, GLuint const section_sprite_layer, GLuint const corner_shadow_sprite_layer)
{
    auto index = compute_index(x, y, z);

    if (index >= m_vertices.size())
        throw std::runtime_error("TileVertices index overflow...");

    auto starting_vertex = &m_vertices[index];

    for (auto i = 0; i < 6; ++i)
    {
        auto & v = starting_vertex[i];
        v.background_sprite_layer = background_sprite_layer;
        v.section_sprite_layer = section_sprite_layer;
        v.corner_shadow_sprite_layer = corner_shadow_sprite_layer;
    }

    //Top-left triangle
    //top-left vertex
    auto & v0 = starting_vertex[0];
    v0.background_sprite_coords[0] = background_sprite_coord_left;
    v0.background_sprite_coords[1] = background_sprite_coord_top;
    v0.section_sprite_coords[0] = section_sprite_coord_left;
    v0.section_sprite_coords[1] = section_sprite_coord_top;
    v0.corner_shadow_sprite_coords[0] = corner_shadow_sprite_coord_left;
    v0.corner_shadow_sprite_coords[1] = corner_shadow_sprite_coord_top;
    //top-right vertex
    auto & v1 = starting_vertex[1];
    v1.background_sprite_coords[0] = background_sprite_coord_right;
    v1.background_sprite_coords[1] = background_sprite_coord_top;
    v1.section_sprite_coords[0] = section_sprite_coord_right;
    v1.section_sprite_coords[1] = section_sprite_coord_top;
    v1.corner_shadow_sprite_coords[0] = corner_shadow_sprite_coord_right;
    v1.corner_shadow_sprite_coords[1] = corner_shadow_sprite_coord_top;
    //bottom-left vertex
    auto & v2 = starting_vertex[2];
    v2.background_sprite_coords[0] = background_sprite_coord_left;
    v2.background_sprite_coords[1] = background_sprite_coord_bottom;
    v2.section_sprite_coords[0] = section_sprite_coord_left;
    v2.section_sprite_coords[1] = section_sprite_coord_bottom;
    v2.corner_shadow_sprite_coords[0] = corner_shadow_sprite_coord_left;
    v2.corner_shadow_sprite_coords[1] = corner_shadow_sprite_coord_bottom;

            
    // Bottom-right triangle
    //top-right vertex
    auto & v3 = starting_vertex[3];
    v3.background_sprite_coords[0] = background_sprite_coord_right;
    v3.background_sprite_coords[1] = background_sprite_coord_top;
    v3.section_sprite_coords[0] = section_sprite_coord_right;
    v3.section_sprite_coords[1] = section_sprite_coord_top;
    v3.corner_shadow_sprite_coords[0] = corner_shadow_sprite_coord_right;
    v3.corner_shadow_sprite_coords[1] = corner_shadow_sprite_coord_top;
    //bottom-right vertex
    auto & v4 = starting_vertex[4];
    v4.background_sprite_coords[0] = background_sprite_coord_right;
    v4.background_sprite_coords[1] = background_sprite_coord_bottom;
    v4.section_sprite_coords[0] = section_sprite_coord_right;
    v4.section_sprite_coords[1] = section_sprite_coord_bottom;
    v4.corner_shadow_sprite_coords[0] = corner_shadow_sprite_coord_right;
    v4.corner_shadow_sprite_coords[1] = corner_shadow_sprite_coord_bottom;
    //bottom-left vertex
    auto & v5 = starting_vertex[5];
    v5.background_sprite_coords[0] = background_sprite_coord_left;
    v5.background_sprite_coords[1] = background_sprite_coord_bottom;
    v5.section_sprite_coords[0] = section_sprite_coord_left;
    v5.section_sprite_coords[1] = section_sprite_coord_bottom;
    v5.corner_shadow_sprite_coords[0] = corner_shadow_sprite_coord_left;
    v5.corner_shadow_sprite_coords[1] = corner_shadow_sprite_coord_bottom;
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

void TileVertices::cache_random_0_to_9_sequence()
{
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
                r_num = Util::rand(10);
                        //short-circuit to avoid checking unexistent neighbors
            } while(	( contains(nghbN)  && r_num == random_0_to_9_sequence[static_cast<RandCont::size_type>(nghbN.x)  * m_map_width + nghbN.y ] ) ||
                        ( contains(nghbNE) && r_num == random_0_to_9_sequence[static_cast<RandCont::size_type>(nghbNE.x) * m_map_width + nghbNE.y] ) ||
                        ( contains(nghbW)  && r_num == random_0_to_9_sequence[static_cast<RandCont::size_type>(nghbW.x)  * m_map_width + nghbW.y ] ) ||
                        ( contains(nghbNW) && r_num == random_0_to_9_sequence[static_cast<RandCont::size_type>(nghbNW.x) * m_map_width + nghbNW.y] )	 );

            random_0_to_9_sequence.push_back(r_num);
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