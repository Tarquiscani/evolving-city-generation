#include "graphics_settings.hh"


#include <algorithm>

#include "settings/debug/debug_settings.hh"


namespace tgm
{



GameVideoMode::GameVideoMode(bool const fullscreen, int const width, int const height) :
	m_fullscreen(fullscreen)
{
	auto const original_mode = g_glfw.video_mode();


	if (width > original_mode.width() || height > original_mode.height())
	{
		throw std::runtime_error("The resolution can't be higher than the original one.");
	}


	if (fullscreen)
	{
		if ((width > 0 && width < original_mode.width()) || (height > 0 && height < original_mode.height()))
		{
			m_width = width;
			m_height = height;
			std::cout << "GameVideoMode fullscreen resolution (" << width << "x" << height << ") is different from the original one." << std::endl;
		}
		else //if the specified resolution isn't supported
		{
			m_width = original_mode.width();
			m_height = original_mode.height();
		}
	}
	else
	{
		// In the window-mode the resolution is that of the native screen.
		m_width = original_mode.width();
		m_height = original_mode.height();
	}
	

	m_red_bits = original_mode.red_bits();
	m_green_bits = original_mode.green_bits();
	m_blue_bits = original_mode.blue_bits();
	m_refresh_rate = original_mode.refresh_rate();
}

auto GraphicsSettings::init_game_video_mode() -> GameVideoMode
{
    //return GameVideoMode{ false, 500, 500 };
	//return GameVideoMode{ true, 800, 600 };
	//return GameVideoMode{ true, 1600, 1200 };
    //return GameVideoMode{ true, 1366, 768 };
	//return GameVideoMode{ true, 1920, 1080 };
	//return GameVideoMode{ true, 3840, 2160 };
	return GameVideoMode{ true };
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
