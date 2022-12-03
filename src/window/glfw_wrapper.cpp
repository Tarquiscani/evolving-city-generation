#include "glfw_wrapper.hh"


#include <cassert>


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
	}
}

GLFW::~GLFW()
{
	if (m_is_init)
	{
		glfwTerminate();
	}
}


auto GLFW::video_mode() -> GLFWvidmode const*
{
	assert(m_is_init);
	
	return glfwGetVideoMode(glfwGetPrimaryMonitor());
}



} //namespace tgm