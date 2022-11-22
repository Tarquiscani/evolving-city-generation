#include "texture_2d.hh"


#include "stb_image/stb_image.h"

#include "settings/graphics_settings.hh"


namespace tgm
{



Texture2D::Texture2D(char const* lowDef_path, char const* highDef_path, char const* ultraHD_path)
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
	int nrChannels;

	// Flip loaded texture on the y-axis. Necessary because images usually have the origin in top-left corner,
	// while OpenGL expects the origin to be in bottom-left corner. 
	stbi_set_flip_vertically_on_load(true);

	m_data = stbi_load(path, &m_width, &m_height, &nrChannels, 0);

	if (m_data)
		std::cout << "Texture loaded:   name: " << path << "   width: " << m_width << "    height: " << m_height << "   channels: " << nrChannels << std::endl;
	else
		throw std::runtime_error("Failed to load texture");
}


void Texture2D::free()
{
	if (m_freed)
		throw std::runtime_error("Cannot free an already freed texture.");

	stbi_image_free(m_data);

	m_freed = true;
}



} //namespace tgm