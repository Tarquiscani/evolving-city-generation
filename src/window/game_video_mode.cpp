#include "game_video_mode.hh"


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
            g_log << "GameVideoMode fullscreen resolution (" << width << "x" << height << ") is different from the original one." << std::endl;
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



} // namespace tgm