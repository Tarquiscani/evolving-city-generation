#ifndef GM_VISUAL_DEBUG_GRAPHICS_MANGER_HH
#define GM_VISUAL_DEBUG_GRAPHICS_MANGER_HH


#include "settings/debug/debug_settings.hh"
#if VISUALDEBUG


#include <vector>

#include <glad/glad.h>

#include "debug_vertices.hh"
#include "graphics/shader.hh"
#include "system/parallelepiped.hh"

#include "debug/logger/debug_printers.hh"


namespace tgm
{



////
//	Note: this class assumes that VisualDebug has a window with its own non-shared OpenGLcontext.
////
class VisualDebugGraphicsManager
{
    public:
        VisualDebugGraphicsManager(DebugVertices & vertices);
        VisualDebugGraphicsManager(VisualDebugGraphicsManager const&) = delete;
        VisualDebugGraphicsManager & operator=(VisualDebugGraphicsManager const&) = delete;

        void init(Vector2i const fbo_size, Vector2i const window_size);
        void draw();
        void resize_fbo(Vector2i const new_fbo_size);
        void resize_window(Vector2i const new_window_size);
        void shutdown() { destroy_objects(); }

        ////
        //	Convert "GLFW screen reference system" coordinates to "VisualDebug frame reference system (in pixels)" coordinates
        ////
        auto glfwScreenPixel_to_framePos(Vector2f const glfw_cursorPos, FloatParallelepiped const& frame) -> Vector2f;

    private:
        Vector2i m_defaultFbo_size;
        Vector2i m_window_size;

        Shader m_shader;

        GLuint m_tile_VBO = 0;
        GLuint m_tile_VAO = 0;

        DebugVertices & m_vertices;

        void generate_objects();
        void destroy_objects();

        static auto compute_viewMatrix() -> glm::mat4;
        static auto compute_projectionMatrix(FloatParallelepiped const& frame) -> glm::mat4;
};



} //namespace tgm


#endif //VISUALDEBUG


#endif //GM_VISUAL_DEBUG_GRAPHICS_MANGER_HH