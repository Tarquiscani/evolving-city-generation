#ifndef GM_GLFW_WRAPPER_HH
#define GM_GLFW_WRAPPER_HH


#include <exception>
#include <iostream>

// They must be included in the following order. 'glfw3.h' can't be included separately.
#include <glad/glad.h>

//glfw options macro
//#define GLFW_DLL		// Definition required when using dll version of GLFW library
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


namespace tgm
{



class GLFW
{
	public:
		////
		//	Initialize both GLFW and GLAD.
		////
		GLFW();
		GLFW(GLFW const&) = delete;
		GLFW& operator=(GLFW const&) = delete;

		~GLFW();

		auto video_mode() -> GLFWvidmode const*;

	private:
		bool m_is_init = false;
};


inline GLFW g_glfw{};



} //namespace tgm


#endif //GM_GLFW_WRAPPER_HH