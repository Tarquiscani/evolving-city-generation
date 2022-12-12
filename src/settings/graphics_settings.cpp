#include "graphics_settings.hh"


#include <algorithm>

#include "settings/debug/debug_settings.hh"


namespace tgm
{



auto GraphicsSettings::init_game_video_mode() -> GameVideoMode
{
    return GameVideoMode{ false, 500, 500 };
    //return GameVideoMode{ true, 800, 600 };
    //return GameVideoMode{ true, 1600, 1200 };
    //return GameVideoMode{ true, 1366, 768 };
    //return GameVideoMode{ true, 1920, 1080 };
    //return GameVideoMode{ true, 3840, 2160 };
    //return GameVideoMode{ true };
}


////
//	Select the appropriate texture definition.
////
auto GraphicsSettings::init_texDefinition() -> TextureDefinition
{
    if (is_lowDef_resolution())
    {
        g_log << "Low definition textures" << std::endl;
        return TextureDefinition::LowDefinition;
    }
    else if (is_fullHD_resolution())
    {
        g_log << "HD textures" << std::endl;
        return TextureDefinition::HighDefinition;
    }
    else if (is_ultraHD_resolution())
    {
        g_log << "Ultra HD textures" << std::endl;
        return TextureDefinition::UltraHighDefinition;
    }
    else
        throw std::runtime_error("Unexpected screen resolution.");
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
