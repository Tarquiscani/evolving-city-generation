#ifndef GM_GLFW_WRAPPER_HH
#define GM_GLFW_WRAPPER_HH


#include <iostream>
#include <exception>

// They must be included in the following order. 'glfw3.h' can't be included separately.
#include <glad/glad.h>

//glfw options macro
//#define GLFW_DLL		// Definition required when using dll version of GLFW library
#define GLFW_INCLUDE_NONE

#include <glfw/glfw3.h>

namespace tgm
{


////
//	Initialize both GLFW and GLAD.
////
void init_glfw();



} //namespace tgm
using namespace tgm;



#endif //GM_GLFW_WRAPPER_HH