#include "graphics_settings.hh"


#include <algorithm>


namespace tgm
{



VideoMode::VideoMode(bool const fullscreen, int const width, int const height) :
	m_fullscreen(fullscreen)
{
	auto const current_mode = g_glfw.video_mode();
	auto const original_width = current_mode->width;
	auto const original_height = current_mode->height;

	std::cout << "Original VideoMode resolution: " << original_width << "x" << original_height << std::endl;


	if (width > original_width || height > original_height)
	{
		throw std::runtime_error("The resolution can't be higher than the original one.");
	}


	if (fullscreen)
	{
		if ((width > 0u && width < original_width) || (height > 0u && height < original_height))
		{
			m_width = width;
			m_height = height;
			std::cout << "Custom VideoMode resolution: " << width << "x" << height << std::endl;
		}
		else //if the specified resolution isn't supported
		{
			m_width = original_width;
			m_height = original_height;
		}
	}
	else
	{
		// In the window-mode the resolution is that of the native screen.
		m_width = original_width;
		m_height = original_height;
	}
}


auto GraphicsSettings::init_videoMode() -> VideoMode
{
    return VideoMode{ false, 500, 500 };
	//return VideoMode{ true, 800, 600 };
	//return VideoMode{ true, 1600, 1200 };
    //return VideoMode{ true, 1366, 768 };
	//return VideoMode{ true, 1920, 1080 };
	//return VideoMode{ true, 3840, 2160 };
	//return VideoMode{ true };
}

static auto compute_imgui_scale(float const ppi_adjustment) -> float
{
	if (ppi_adjustment <= 3.1f)	// In case the resolution is 1080p the best scale is 1.f
	{
		return 1.f;
	}
	else
	{
		return std::max(1.f, ppi_adjustment / 2.f); //a scale value smaller than 1.f produces an ugly output
	}
}

//TODO: Make imgui_scale be a c++17 static inline variable.
auto GraphicsSettings::imgui_scale() -> float
{

	static float* is_p = new float(compute_imgui_scale(ppi_adjustment())); //a scale value smaller than 1.f produces an ugly output

	return *is_p;
}



} //namespace tgm
