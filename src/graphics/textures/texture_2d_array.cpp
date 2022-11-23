#include "texture_2d_array.hh"

#include <sstream>

#include "stb/stb_image.h"				// The implementation is already defined in another file
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h" //for debug only


namespace tgm
{



Texture2DArray::Texture2DArray(char const* lowDef_path, char const* highDef_path, char const* ultraHD_path)
{
	auto path = "";

	switch (GSet::texture_definition())
	{
		case TextureDefinition::LowDefinition:
			path = lowDef_path;
			break;

		case TextureDefinition::HighDefinition:
			path = highDef_path;
			break;

		case TextureDefinition::UltraHighDefinition:
			path = ultraHD_path;
			break;

		default:
			throw std::runtime_error("Unexpected texture definition.");
			break;
	}


	// load image, create texture
	auto channels = 0;

	// Flip loaded texture on the y-axis. Necessary because images usually have the origin in top-left corner,
	// while OpenGL expects the origin to be in bottom-left corner. 
	stbi_set_flip_vertically_on_load(true);

	auto const raw_data = stbi_load(path, &m_atlas_width, &m_atlas_height, &channels, 0);

	if (raw_data)
	{
		std::cout << "Texture atlas loaded and converted to a Texture2DArray:   name: " << path << "   width: " << m_atlas_width << "    height: " << m_atlas_height << "   channels: " << channels << std::endl;
	}
	else
	{
		throw std::runtime_error("Failed to load texture");
	}


	compute_array(raw_data, m_atlas_width, m_atlas_height, channels);

	
	stbi_image_free(raw_data);
}

void Texture2DArray::compute_array(unsigned char* raw_data, int const raw_width, int const raw_height, int const channels)
{
	auto const pptxu = static_cast<int>(GSet::pptxu());
	m_width = pptxu;
	m_height = pptxu;

    auto const atlas_columns = raw_width  / pptxu;
    auto const atlas_rows    = raw_height / pptxu;
	m_layer_count = atlas_columns * atlas_rows;

	m_data.reset(new unsigned char[static_cast<std::size_t>(channels) * pptxu * pptxu * m_layer_count]);

	auto const bottom_offset = raw_height - atlas_rows * pptxu; // The atlas is aligned to the top-left corner, whereas the OpenGL image is aligned to bottom-left corner.
																// So if the raw_height isn't a multiple of pptxu, then there is a small space between the bottom edge and
																// the first texture unit.
	auto current = 0; //current byte

	for (int m = atlas_rows - 1; m >= 0; --m) //the top raw of the atlas is the last one since the image is flipped
	{
		for (int n = 0; n < atlas_columns; ++n)
		{
			for (int j = 0; j < pptxu; ++j) //j: current line inside the texture unit
			{
				auto const beg = (bottom_offset + m * pptxu + j) * raw_width + n * pptxu;
				auto const end = beg + pptxu;

				for (int i = beg; i < end; ++i) //j: current pixel inside the texture unit line
				{
					for (int c = 0; c < channels; ++c) //c: current channel inside the pixel
					{
						m_data[current++] = raw_data[i * channels + c];
					}
				}
			}
		}
	}

	//debug_printLayers(channels, pptxu, atlas_rows, atlas_columns);
}

void Texture2DArray::free()
{
	if (m_freed)
		throw std::runtime_error("Cannot free an already freed texture.");

	m_data.reset();

	m_freed = true;
}


void Texture2DArray::debug_printLayers(int const channels, int const pptxu, int const atlas_rows, int const atlas_columns)
{
	std::string dir{"_DATA/exp/layers/"};

	stbi_flip_vertically_on_write(true);


	auto const bytes_per_layer = pptxu * pptxu * channels;
	
	for (int m = 0; m < atlas_rows; ++m)
	{
		for (int n = 0; n < atlas_columns; ++n)
		{
			std::ostringstream path; path << dir << "txu_" << m << "_" << n << ".png";

			auto const beg = (m * atlas_columns + n) * bytes_per_layer;
			auto const end = beg + bytes_per_layer;

			// Skip transparent layers by checking the alpha value of each pixel.
			bool skip = false;
			if (channels == 4)
			{
				skip = true;
				for (int i = beg + 3; i < end; i += 4)
				{
					if (m_data[i] != 0) 
					{ 
						skip = false; 
						break; 
					}
				}
			}

			if (!skip)
			{
				stbi_write_png(path.str().c_str(), pptxu, pptxu, channels, &m_data[beg], channels * pptxu);
			}
		}
	}
}



} //namespace tgm