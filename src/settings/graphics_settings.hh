#ifndef GM_GRAPHICS_SETTINGS_HH
#define GM_GRAPHICS_SETTINGS_HH


#include <cmath>
#include <iostream>

#include "system/vector3.hh"
#include "window/glfw_wrapper.hh"

#include "debug/logger/log_streams.hh"


////
//  COMPILE-TIME SETTINGS
//  The following settings are meant to be used just for debug and give the ability to test different techniques of the engine. 
////


////
//  Enable the pipeline of off-screen buffers that allows to compute and render the black edges of roofs.
//  Default: true. Turning it off improves the performance.
////
#define GSET_EDGE_DETECTION_FILTER true


////
//  Enable alpha-to-coverage sampling in order to apply the anti-aliasing also inside the sprites. 
//  Useful to improve graphics in zoom levels different than 1 (remember that for zoom=1 the sprite rendering is pixel-perfect and there's no need for anti-aliasing).
//  Note: When no postprocessing filter is active and the scene is drawn on the default FBO, the GPU driver may force an arbitrary MSAA value and override every GLFW hint.
//  Default: true. Turning it off improves the performance.
////
#define GSET_ALPHA_TO_COVERAGE true


////
//  If true the tileset texture atlas is converted to a Texture2DArray. If false the texture atlas is used "as is".
//  It improves graphics for high zoom levels, avoiding sprite bleeding for low mipmap levels.
//  Default: true. It doesn't affect the performance.
////
#define GSET_TILESET_TEXARRAY true


////
//  If true the characters' (and in general mobiles') movement is rounded such in a way that each frame they cover a distance that's a multiple of 1 pixel.
//  This improves the graphics, avoiding that a sprite texel is sampled half in a pixel and half in another pixel. And also avoiding the disturbing flickering 
//  that this would produce when the sampling isn't homogeneous across frames.
//  Default: true. It doesn't affect the performance.
////
#define GSET_ROUND_TO_NEAREST_PIXEL true


////
//  When enabled it avoids that hidden polygons are drawn each frame, computing their visibility in advance.
//  It's an experimental feature, not really optimized, and only works with tiles. And it hasn't been maintained or tested for a while.
//  Default: false. Turning it on decreases the performance a bit.
////
#define GSET_OCCLUSION_CULLING false


////
//  It allows to quickly switch from a texture atlas to another. It is used to test different texture atlases with differen tile dimensions
//  and check that tile-dimension is really a variable and doesn't hardly affect any engine feature.
//  It also contains useful examples of how to change the tile dimension.
//  Default: false.
////
#define GSET_ALTERNATIVE_ASSETS false


////
//  It allows to view the polygons that make up the world, without rendering the surfaces but just the edges of them.
//  Default: false.
////
#define GSET_WIREFRAME_MODE false


////
//  It renders a post-processing effect that shows, for each pixel, how many non-discarded fragments compete for that pixel:
//      - Black: no fragments
//      - Green: 1 fragment
//      - Dark orange -> Yellow: 2 -> 10 fragments
//      - Dark red -> Bright red: 11 -> 50 fragments
//  It's useful to optimize the performance and test the occlusion culling.
//  Default: false
//// 
#define GSET_OVERDRAW_MODE false


////
//  It opens additional windows, besides the main game window, showing the off-screen framebuffers related to the edge detection.
//  It's only effective when the game is run in windowed mode and when the edge detection filter is enabled.
//  Default: false.
////
#define GSET_SHOW_EDGE_DETECTION_FBOS false


////
//  It opens an additional window, besides the main game window, showing the off-screen framebuffers related to the occlusion culling.
//  It's only effective when the game is run in windowed mode and when the edge detection filter is enabled.
//  Default: false.
////
#define GSET_SHOW_OCCLUSION_CULLING_FBO false



////
//  IMPLEMENTATION MACROS
//  Do not touch.
////

#if GSET_EDGE_DETECTION_FILTER && GSET_OVERDRAW_MODE
    #error GSET_EDGE_DETECTION_FILTER and GSET_OVERDRAW_MODE cannot be both active
#endif


// True when there's at least one postprocessing effect active (postprocessing implies that the scene is rendered to an off-screen framebuffer).
#define GSET_POSTPROCESSING (GSET_EDGE_DETECTION_FILTER || GSET_OVERDRAW_MODE)

#if GSET_WIREFRAME_MODE && GSET_POSTPROCESSING
    #error GSET_WIREFRAME_MODE and GSET_POSTPROCESSING effects cannot be both active
#endif

#if GSET_ALPHA_TO_COVERAGE && GSET_OVERDRAW_MODE
    #error GSET_ALPHA_TO_COVERAGE and GSET_OVERDRAW_MODE cannot be both active.
#endif


#define GSET_SHOW_EDGE_DETECTION_FBOS_IMPL (GSET_EDGE_DETECTION_FILTER && GSET_SHOW_EDGE_DETECTION_FBOS)

#define GSET_SHOW_OCCLUSION_CULLING_FBO_IMPL (GSET_OCCLUSION_CULLING && GSET_SHOW_OCCLUSION_CULLING_FBO)



namespace tgm
{



////
//	Actual VideoMode in which the game run. 
////
class GameVideoMode : public VideoMode
{
    public:
        ////
        //	@fullscreen: Indicate whether the application have to be run in fullscreen mode or in window mode.
        //	@width, @height: Ignored in window mode. In fullscreen mode represent the chosen resolution.
        ////
        GameVideoMode(bool const fullscreen, int const width = 0, int const height = 0);

        bool fullscreen() const noexcept { return m_fullscreen; }

    private:
        bool m_fullscreen = false;
};


enum class TextureDefinition
{
    LowDefinition,
    HighDefinition,
    UltraHighDefinition,
};

class GraphicsSettings
{
    private:
        static auto init_game_video_mode() -> GameVideoMode;

    public:
        static inline auto game_video_mode = init_game_video_mode();


        static unsigned constexpr depthBuffer_bits = 24u;
        static int constexpr samples = 4;


        ////
        //	RESOLUTION
        ////

        ////
        //	Ratio useful to compute the GUI size and other PPI-dependant sizes.
        ////
        static inline auto ppi_adjustment() noexcept -> int { return game_video_mode.width() / 640; }

        ////
        //	Ratio useful to compute ImGui font and spacing sizes.
        ////
        static auto imgui_scale() -> float;

        ////
        //	Factor used to compute the edge thickness.
        ////
        static inline auto edgeThickness_factor() noexcept -> float { return m_edgeThickness_factor; }




        ////
        //	RENDERING
        ////


        ////
        //  Conversion factor of the floor z-coordinate from map reference system to world space reference system.
        //  Note: The height of the world space is a lot larger because otherwise when zooming out in perspective 
        //		  mode there would be an overlapping between sprites of different floors.
        ////
        static constexpr auto floorsSpacing_ratio = 10.f;

        ////
        //  @return: Distance between two floors (in units -- OpenGL world space reference system)
        ////
        static inline auto floors_distance() noexcept -> float { return tiles_to_units(floorsSpacing_ratio); };

        ////
        //  How much a sprite must be raised from the ground when passing from "map reference system" to "world space reference system" (world space r.s. - in units)
        ////
        static constexpr auto matsLayer_raising = 4.f / 6.f;
        static constexpr auto volumesLayer_raising = 5.f / 6.f;

        ////
        //	Maximum height of a roof (in tiles -- map reference system). 
        //  Note: It's used to move the camera sufficiently upwards so that it can frame the roof.
        ////
        static constexpr auto roof_maxHeight = 4;
        
        ////
        //	@return: How much the roof must be shifted along the map x-axis to perfectly fit over the borders (in tiles - map RS).
        //			 Note: The sliding is used to simulate height.
        ////
        static inline auto roof_wyOffset() noexcept -> float { return 3.f / 5.f; }

        ////
        //  @return: How much the the map x-coordinate has to be slided according to its z-coordinate when passing from map RS to world space RS. 
        //			 Note: The sliding is needed to simulate height.
        ////
        static inline auto wySliding_ratio() noexcept -> float { return 1.f; }

        ////
        //	@return: How much a vertex of the roof must be slided along the map x-axis according to its relative z-coordinate (the relative height 
        //			 is measured from the base of the roof).
        //			 Note: The sliding is used to simulate height.
        ////
        static inline auto roofVertex_wySliding_ratio() noexcept -> float { return wySliding_ratio() * 3.f / 4.f; }
        

        


        ////
        //	TEXTURES
        ////
        
        #if GSET_ALTERNATIVE_ASSETS
            static constexpr auto lowDefinition_pptxu = 216.f;
            static constexpr auto highDefinition_pptxu = 216.f;
            static constexpr auto ultraHighDefinition_pptxu = 216.f;
        #else
            static constexpr auto lowDefinition_pptxu = 32.f;
            static constexpr auto highDefinition_pptxu = 64.f;
            static constexpr auto ultraHighDefinition_pptxu = 128.f;
        #endif

        static inline auto texture_definition() noexcept -> TextureDefinition { return m_tex_definition; }

        ////
        //  @return: Number of pixels in a unit of the texture atlas.
        ////
        static inline auto pptxu() noexcept -> float { return m_pixels_per_textureUnit; };
        



        ////
        //	UNITS OF MEASURE
        ////
            
        ////
        //	Units (of measurement) per tile.
        ////
        static constexpr auto upt = 1.f;
        

        static inline auto texels_to_units(float const texels) -> float { return texels / tpt * upt; }

        static inline auto units_to_tiles(float const units) -> int { return static_cast<int>(::floorf(units / upt)); } //TODO: It should be "std::floorf", but g++ doesn't allow it.
        static inline auto tiles_to_units(int const tiles) -> float { return tiles * upt; }
        static inline auto tiles_to_units(float const tiles) -> float { return tiles * upt; }

        static inline auto units_to_screenPixels(float const units, float const pixel_dim) -> float { return units / pixel_dim; }
        static inline auto screenPixels_to_units(float const spix, float const pixel_dim) -> float { return spix * pixel_dim; }
        



        ////
        //	MOVEMENT
        ////
        
        ////
        //	Movement unit (in units). 
        //  Arbitrary value used by the brute trail algorithm to split the movement in discrete steps.
        //	It's also used as an epsilon in some position comparisons.
        ////
        static constexpr auto mu = upt / 100.f;




        
        ////
        //	OTHERS
        ////
        static auto constexpr chunkSize_inTile = 1500; 


        static inline Vector3f TEST_playerSpritePosition{ 0.f, 0.f, 0.f };
        static inline Vector3f TEST_cameraTargetPosition{ 0.f, 0.f, 0.f };

    private:
        

        #if GSET_ALTERNATIVE_ASSETS
            static constexpr auto m_lowDefinition_tpt = 216.f;
            static constexpr auto m_highDefinition_tpt = 216.f;
            static constexpr auto m_ultraHighDefinition_tpt = 216.f;
        #else
            static constexpr auto m_lowDefinition_tpt = 32.f;
            static constexpr auto m_highDefinition_tpt = 64.f;
            static constexpr auto m_ultraHighDefinition_tpt = 128.f;
        #endif


        
        static inline bool is_lowDef_resolution()  { return game_video_mode.width() > 700  && game_video_mode.width() <= 1400; }
        static inline bool is_fullHD_resolution()  { return game_video_mode.width() > 1400 && game_video_mode.width() <= 1920; }
        static inline bool is_ultraHD_resolution() { return game_video_mode.width() > 1920 && game_video_mode.width() <= 3840; }


        ////
        //	Select the appropriate texture definition.
        ////
        static auto init_texDefinition() -> TextureDefinition;
        static inline TextureDefinition const m_tex_definition = init_texDefinition();


        ////
        //	Select the dimension of a single tile, in texels.
        ////
        static inline auto select_texelsPerTile() -> float
        {
            switch (m_tex_definition)
            {
                case TextureDefinition::LowDefinition:
                    return m_lowDefinition_tpt;
                    break;

                case TextureDefinition::HighDefinition:
                    return m_highDefinition_tpt;
                    break;

                case TextureDefinition::UltraHighDefinition:
                    return m_ultraHighDefinition_tpt;
                    break;

                default:
                    throw std::runtime_error("Unexpected TextureDefinition.");
                    break;
            }
        }

        // Dimension of a tile (in texels)
        static inline float const tpt = select_texelsPerTile();


        ////
        //	Select the appropriate texture definition.
        ////
        static inline auto init_pixelsPerTextureUnit() -> float
        {
            switch (m_tex_definition)
            {
                case TextureDefinition::LowDefinition:
                    return lowDefinition_pptxu; 
                    break;

                case TextureDefinition::HighDefinition:
                    return highDefinition_pptxu;
                    break;

                case TextureDefinition::UltraHighDefinition:
                    return ultraHighDefinition_pptxu;
                    break;

                default:
                    throw std::runtime_error("Unexpected texture definition.");
                    break;
            }
        }

        static inline float const m_pixels_per_textureUnit = init_pixelsPerTextureUnit();


        
        static inline auto init_edgeThicknessFactor() -> float
        {
            switch (m_tex_definition)
            {
                case TextureDefinition::LowDefinition:
                    return 0.f; 
                    break;

                case TextureDefinition::HighDefinition:
                    return 1.f;
                    break;

                case TextureDefinition::UltraHighDefinition:
                    return 4.f;
                    break;

                default:
                    throw std::runtime_error("Unexpected texture definition.");
                    break;
            }
        }

        static inline float const m_edgeThickness_factor = init_edgeThicknessFactor();
};

using GSet = GraphicsSettings;



} //namespace tgm


#endif //GM_GRAPHICS_SETTINGS_HH
