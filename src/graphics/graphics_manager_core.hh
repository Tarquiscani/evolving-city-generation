#ifndef GM_GRAPHICS_MANAGER_CORE_HH
#define GM_GRAPHICS_MANAGER_CORE_HH



#include <string>
#include <glad/glad.h>
#define GLM_FORCE_SILENT_WARNINGS
#include <glm/glm.hpp>

#include <system/vector2.hh>
#include <system/vector3.hh>

namespace tgm
{



enum class TextureFormat
{
	Red,
	RGB,
	RGBA,
	DepthStencil
};


//TODO: NOW: Forse andrebbe rinominato GpuManagerCore
namespace GraphicsManagerCore
{


void init(std::string const context_name);

void print_contextInfos(std::string const context_name);

////
//	Convert from "GLFW screen reference system" coordinates to "OpenGL framebuffer reference system" coordinates.
//	@glfw_cursorPos: Relative position of the cursor in the window (GLFW screen reference system).
//	
//	@return: (OpenGL framebuffer reference system)
////
auto glfwScreenRS_to_openGlFramebufferRS(Vector2f const glfw_cursorPos, Vector2i const screen_size, Vector2i const framebuffer_size) -> Vector2f;

////
//	@fbo_pos: (OpenGL framebuffer reference system)
//
//	@return: (in pixels -- OpenGL world space reference system)
////
auto fragmentRS_to_openGlWorldSpaceRS(Vector2f const fbo_pos, float const z_depth, glm::mat4 const view, glm::mat4 projection, Vector2i const framebuffer_size) -> Vector3f;



////
//	Print on console a rectangle of texels inside texture. The texture has @channels channels and its underlying type is GL_UNSIGNED_BYTE.
//	@tex_width: Width of the texture (in texels -- OpenGL texture reference system).
//	@tex_height: Height of the texture (in texels -- OpenGL texture reference system).
//	@beg_x, @beg_y: Beginning of the rectangle of texels that have to be printed on the console.
//	@rect_width, @rect_height: Beginning of the rectangle of texels that have to be printed on the console.
////
void debug_printTexture(GLuint const texture_id, unsigned const tex_width, unsigned const tex_height, TextureFormat const format, 
						GLint const beg_x, GLint const beg_y, GLsizei const rect_width, GLsizei const rect_height);


void debug_printDrawArraysIndirectCommandBuffer(GLuint const cmd_id);


} //namespace GraphicsManagerCore



} //namespace tgm
using namespace tgm;



#endif //GM_GRAPHICS_MANAGER_CORE_HH