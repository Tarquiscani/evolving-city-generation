#include "glfw_wrapper.hh"

namespace tgm
{



static void error_callback(int error, const char* description)
{
	std::cout << "GLFW error #" << error << ": " << description;
}

void init_glfw()
{
	static bool initialized = false;

	if (!initialized)
	{
		initialized = true;

		glfwSetErrorCallback(error_callback);

		if (!glfwInit())
		{
			throw std::runtime_error("Failed to initialize GLFW");
		}
	}
}



} //namespace tgm