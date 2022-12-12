#include "glfw_wrapper.hh"


#include "debug/asserts.hh"
#include "debug/logger/log_streams.hh"


namespace tgm
{



static void glfw_error_callback(int error, char const* description)
{
    g_log << "GLFW error #" << error << ": " << description;
}



GLFW::GLFW()
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    else
    {
        m_is_init = true;

        print_monitor_info();
    }
}

GLFW::~GLFW()
{
    if (m_is_init)
    {
        glfwTerminate();
    }
}

void GLFW::print_monitor_info()
{
    auto logger = Logger{ g_log };

    auto curr_monitor = glfwGetPrimaryMonitor();
    if (curr_monitor)
    {
        logger << "Current monitor: "; print_monitor(logger, curr_monitor);

        auto curr_mode = glfwGetVideoMode(curr_monitor);
        if (curr_mode)
        {
            logger << Logger::nltb << "Current video mode: "; print_mode(logger, *curr_mode);
        }
        else
        {
            logger << Logger::nltb << "No current video mode found";
        }
    }
    else
    {
        logger << Logger::nltb << "No current monitor found";
    }
        
    auto monitor_num = 0;
    auto monitors = glfwGetMonitors(&monitor_num);
    logger << Logger::nltb << "Monitor Num: " << monitor_num 
           << Logger::addt;
    for (auto i = 0; i < monitor_num; ++i)
    {
        auto monitor = monitors[i];
        logger << Logger::nltb << "Monitor #" << i << ": "; print_monitor(logger, monitor);

        auto modes_num = 0;
        auto modes = glfwGetVideoModes(monitor, &modes_num);
            
        logger << Logger::addt;

        for (auto j = 0; j < modes_num; ++j)
        {
            auto mode = modes[j];
            logger << Logger::nltb << "Mode #" << j << ": "; print_mode(logger, mode);
        }

        logger << Logger::remt;
    }
        
    logger << Logger::remt;
}

auto GLFW::print_monitor(Logger & logger, GLFWmonitor * monitor) -> Logger &
{
    assert(monitor);

    auto posx = 0;
    auto posy = 0;
    auto width = 0;
    auto height = 0;
    glfwGetMonitorWorkarea(monitor, &posx, &posy, &width, &height);

    logger << ": " << width << "x" << height << " --- " << "(" << posx << ", " << posy << ")" << glfwGetMonitorName(monitor);

    return logger;
}

auto GLFW::print_mode(Logger & logger, GLFWvidmode const& mode) -> Logger &
{
    logger << mode.width << "x" << mode.height << " --- " << "(" << mode.redBits << ", " << mode.blueBits << ", " << mode.greenBits << ") --- refresh rate: " << mode.refreshRate;

    return logger;
}


auto GLFW::video_mode() -> VideoMode
{
    check(m_is_init);
    
    auto monitor = glfwGetPrimaryMonitor();
    check(monitor);

    auto mode = glfwGetVideoMode(monitor);
    check(mode);

    return VideoMode{ mode->width, mode->height, mode->redBits, mode->greenBits, mode->blueBits, mode->refreshRate };
}



} //namespace tgm