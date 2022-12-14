#include "graphics_settings.hh"


#include <algorithm>

#include "settings/debug/debug_settings.hh"


namespace tgm
{



auto GraphicsSettings::init_game_video_mode() -> GameVideoMode
{
#if CMAKE_FULLSCREEN
    return GameVideoMode{ true };
#else // CMAKE_WINDOWED
    return GameVideoMode{ false, 500, 500 };
#endif
    //return GameVideoMode{ true, 800, 600 };
    //return GameVideoMode{ true, 1600, 1200 };
    //return GameVideoMode{ true, 1366, 768 };
    //return GameVideoMode{ true, 1920, 1080 };
    //return GameVideoMode{ true, 3840, 2160 };
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

auto GraphicsSettings::edge_thickness(float const current_zoom_level) -> int 
{ 
    auto thickness = 0.f;

    switch (m_tex_definition)
    {
        case TextureDefinition::LowDefinition:
            // It's a function that fits the following values:
            // 0.1 -> 6
            // 0.5 -> 4
            // 1   -> 2
            // 3   -> 1
            thickness = 2.400139f * std::pow(current_zoom_level, -0.4104228f);
            break;

        case TextureDefinition::HighDefinition:
            // It's a piecewise-defined function that fits the following values:
            // 0.1 -> 12
            // 0.5 -> 8
            // 1   -> 2
            // 4   -> 1
            thickness = current_zoom_level <= 1 ?
                            -11.14754f  * current_zoom_level + 13.27869f :
                            -0.1666667f * current_zoom_level + 2.166667f;
            break;

        case TextureDefinition::UltraHighDefinition:
            thickness = (current_zoom_level + 6.f) / current_zoom_level;
            break;

        default:
            throw std::runtime_error("Unexpected texture definition.");
            break;
    }

    return std::roundf(std::clamp(thickness, 1.f, 12.f));
}



} //namespace tgm
