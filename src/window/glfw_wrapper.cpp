#include "glfw_wrapper.hh"


#include "settings/debug/debug_settings.hh"


namespace tgm
{



static void glfw_error_callback(int error, char const* description)
{
	std::cout << "GLFW error #" << error << ": " << description;
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
	auto curr_monitor = glfwGetPrimaryMonitor();
	if (curr_monitor)
	{
		std::cout << "Current monitor: "; print_monitor(std::cout, curr_monitor) << std::endl;

		auto curr_mode = glfwGetVideoMode(curr_monitor);
		if (curr_mode)
		{
			std::cout << "Current video mode: "; print_mode(std::cout, *curr_mode) << std::endl;
		}
		else
		{
			std::cout << "No current video mode found" << std::endl;
		}
	}
	else
	{
		std::cout << "No current monitor found" << std::endl;
	}
		
	auto monitor_num = 0;
	auto monitors = glfwGetMonitors(&monitor_num);
	std::cout << "Monitor Num: " << monitor_num << std::endl;
	for (auto i = 0; i < monitor_num; ++i)
	{
		auto monitor = monitors[i];
		std::cout << "Monitor #" << i << ": "; print_monitor(std::cout, monitor) << std::endl;

		auto modes_num = 0;
		auto modes = glfwGetVideoModes(monitor, &modes_num);
			
		for (auto j = 0; j < modes_num; ++j)
		{
			auto mode = modes[j];
			std::cout << "\t Mode #" << j << ": "; print_mode(std::cout, mode) << std::endl;
		}
	}
}

auto GLFW::print_monitor(std::ostream & os, GLFWmonitor * monitor) -> std::ostream &
{
	assert(monitor);

	auto posx = 0;
	auto posy = 0;
	auto width = 0;
	auto height = 0;
	glfwGetMonitorWorkarea(monitor, &posx, &posy, &width, &height);

	os << ": " << width << "x" << height << " --- " << "(" << posx << ", " << posy << ")" << glfwGetMonitorName(monitor);

    return os;
}

auto GLFW::print_mode(std::ostream & os, GLFWvidmode const& mode) -> std::ostream &
{
	os << mode.width << "x" << mode.height << " --- " << "(" << mode.redBits << ", " << mode.blueBits << ", " << mode.greenBits << ") --- refresh rate: " << mode.refreshRate << std::endl;

    return os;
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