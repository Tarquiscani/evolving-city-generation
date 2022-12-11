#ifndef GM_GRAPHICS_SETTINGS_HH
#define GM_GRAPHICS_SETTINGS_HH


#include <cmath>
#include <iostream>

#include "system/vector3.hh"
#include "window/glfw_wrapper.hh"

#include "debug/logger/log_streams.hh"


#define POLYGON_MODE false


#define EDGE_DETECTION_FILTER true

#define EDGE_DETECTION_FILTER_INTERMEDIATE_STEPS (EDGE_DETECTION_FILTER && false)

// It renders a post-processing effect that shows, for each pixel, how many non-discarded fragments compete for that pixel.
// (black: no fragments; green: 1 fragment; dark orange -> yellow: 2 -> 10 fragments; dark red -> bright red: 11 -> 50 fragments)
#define OVERDRAW_FILTER false


#define POSTPROCESSING (EDGE_DETECTION_FILTER || OVERDRAW_FILTER)

#if EDGE_DETECTION_FILTER && OVERDRAW_FILTER
	#error EDGE_DETECTION_FILTER and OVERDRAW_FILTER cannot be both active
#endif

#if POLYGON_MODE && POSTPROCESSING
	#error POLYGON_MODE and POSTPROCESSING effects cannot be both active
#endif


// It avoids that hidden entites are drawn each frame, computing their visibility in advance.
#define OCCLUSION_CULLING false

#define SHOW_VISIBLE_ENTITIES_FBO (OCCLUSION_CULLING && false)


// Enable alpha-to-coverage sampling in order to apply anti-aliasing also inside the sprites.
// Note: When no postprocessing filter is active and the scene is drawn on the default FBO, the GPU driver may force an arbitrary MSAA value and override every GLFW hint.
#define ALPHA_TO_COVERAGE false

#if ALPHA_TO_COVERAGE && OVERDRAW_FILTER
	#error ALPHA_TO_COVERAGE and OVERDRAW_FILTER cannot be both active.
#endif


// If true the tileset texture atlas is converted to a Texture2DArray. If false the texture atlas is used "as is".
#define GSET_TILESET_TEXARRAY true


// If true the mobile movement is rounded so that each frame they cover a distance that's a multiple of 1 pixel.
#define GSET_ROUND_TO_NEAREST_PIXEL true


// It changes the texture used to represent the scene switching to royalty free assets.
#define FREE_ASSETS false


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
		
		#if FREE_ASSETS
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
		

		#if FREE_ASSETS
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
		static inline auto init_texDefinition() -> TextureDefinition
		{
			if (is_lowDef_resolution())
			{
				std::cout << "Low definition textures" << std::endl;
				return TextureDefinition::LowDefinition;
			}
			else if (is_fullHD_resolution())
			{
				std::cout << "HD textures" << std::endl;
				return TextureDefinition::HighDefinition;
			}
			else if (is_ultraHD_resolution())
			{
				std::cout << "Ultra HD textures" << std::endl;
				return TextureDefinition::UltraHighDefinition;
			}
			else
				throw std::runtime_error("Unexpected screen resolution.");
		}

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
