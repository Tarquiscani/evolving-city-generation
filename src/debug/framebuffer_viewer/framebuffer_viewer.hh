#ifndef GM_FRAMEBUFFER_VIEWER_HH
#define GM_FRAMEBUFFER_VIEWER_HH


#include <string>
#include <vector>

#include "glad/glad.h"

#include "graphics/shader.hh"
#include "window/window_manager.hh"


namespace tgm
{



////
//
//	Used to open an auxiliary window to display a texture.
//
////
class FramebufferViewer
{
    public:
        FramebufferViewer();
        ~FramebufferViewer();

        bool is_open() const noexcept { return m_open; }

        void open(std::string const title, GLuint const texture, GLsizei const width, GLsizei const height, std::string const shader_name);
        void update();
        void close();

    private:
        bool m_open = false;

        GLuint m_texture = 0;

        Window & m_window;

        Shader m_shader;

        static inline std::vector<float> const m_VAO_vertices = {
            //top-left triangle
            -1.f, -1.f,		0.f, 0.f,
            -1.f,  1.f,		0.f, 1.f,
             1.f,  1.f,		1.f, 1.f,

             //bottom-right triangle
             -1.f, -1.f,		0.f, 0.f,
              1.f,  1.f,		1.f, 1.f,
              1.f, -1.f, 	1.f, 0.f
        };

        GLuint m_VAO = 0;
        GLuint m_VBO = 0;

        void assert_open() const;
};



} // namespace tgm


#endif //GM_FRAMEBUFFER_VIEWER_HH