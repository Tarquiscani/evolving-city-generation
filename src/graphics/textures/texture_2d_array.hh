#ifndef GM_TEXTURE_2D_ARRAY_HH
#define GM_TEXTURE_2D_ARRAY_HH


#include <exception>
#include <iostream>
#include <memory>

#include "settings/graphics_settings.hh"


namespace tgm
{



class Texture2DArray
{
	public:
		Texture2DArray(char const* lowDef_path, char const* highDef_path, char const* ultraHD_path);
		Texture2DArray(Texture2DArray const&) = delete;
		Texture2DArray & operator=(Texture2DArray const&) = delete;


		auto data() const -> unsigned char*
		{
			if (m_freed)
				throw std::runtime_error("Cannot get data from a freed texture.");

			return m_data.get();
		}


		void free();


		auto width() const noexcept -> int { return m_width; }
		auto height() const noexcept -> int { return m_height; }
		auto layer_count() const noexcept -> int { return m_layer_count; }

		auto atlas_width() const noexcept -> int { return m_atlas_width; }
		auto atlas_height() const noexcept -> int { return m_atlas_height; }


	private:
		bool m_freed = false;

		std::unique_ptr<unsigned char[]> m_data;
		int m_width = 0;
		int m_height = 0;
		int m_layer_count = 0;

		int m_atlas_width = 0;
		int m_atlas_height = 0;

		void compute_array(unsigned char* raw_data, int const raw_width, int const raw_height, int const channels);

		void debug_printLayers(int const channels, int const pptxu, int const atlas_rows, int const atlas_columns);
};


#if FREE_ASSETS
	#if GSET_TILESET_TEXARRAY
		inline Texture2DArray default_texture_tileset{"media/free_assets/free_tileset.png", 
													  "media/free_assets/free_tileset.png", 
													  "media/free_assets/free_tileset.png" };
	#endif
#else
	#if GSET_TILESET_TEXARRAY
		inline Texture2DArray default_texture_tileset{"media/textures/default_texture_tileset_ld.png", 
													  "media/textures/default_texture_tileset_hd.png", 
													  "media/textures/default_texture_tileset_uhd.png"};
	#endif
#endif



} //namespace tgm


#endif //GM_TEXTURE_2D_ARRAY_HH