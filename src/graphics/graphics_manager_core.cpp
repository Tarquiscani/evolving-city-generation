#include "graphics_manager_core.hh"


#include <iostream>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>

#include "graphics/opengl/draw_arrays_indirect_command.hh"

#include "debug/logger/debug_printers.hh"
#include "debug/logger/log_streams.hh"


namespace tgm
{



namespace GraphicsManagerCore
{



static void print_openGLVersion_infos()
{
    auto lgr = Logger{ g_log };

    auto context_major = GLint{ 0 };
    auto context_minor = GLint{ 0 };

    glGetIntegerv(GL_MAJOR_VERSION, &context_major);
    glGetIntegerv(GL_MINOR_VERSION, &context_minor);

    lgr << "OpenGL version used for the current context: " << context_major << "." << context_minor;


    lgr << Logger::nltb << "Supported GLSL versions:"
        << Logger::addt;

    auto supported_glsl_version_num = GLint{ 0 };
    glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &supported_glsl_version_num);

    for (auto i = 0; i < supported_glsl_version_num; ++i)
    {
        auto string_version = std::string{ reinterpret_cast<char const*>(glGetStringi(GL_SHADING_LANGUAGE_VERSION, i)) };

        lgr << Logger::nltb << string_version;
    }
}


static void debugMessage_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, GLchar const* message, void const*)
{
    std::string source_str;
    std::string type_str;
    std::string severity_str;

    switch (source) 
    {
        case GL_DEBUG_SOURCE_API:
            source_str = "API";
            break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            source_str = "WINDOW SYSTEM";
            break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            source_str = "SHADER COMPILER";
            break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
            source_str = "THIRD PARTY";
            break;

        case GL_DEBUG_SOURCE_APPLICATION:
            source_str = "APPLICATION";
            break;

        case GL_DEBUG_SOURCE_OTHER:
            source_str = "UNKNOWN";
            break;

        default:
            source_str = "UNKNOWN";
            break;
    }

    switch (type) 
    {
        case GL_DEBUG_TYPE_ERROR:
            type_str = "ERROR";
            break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            type_str = "DEPRECATED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            type_str = "UDEFINED BEHAVIOR";
            break;

        case GL_DEBUG_TYPE_PORTABILITY:
            type_str = "PORTABILITY";
            break;

        case GL_DEBUG_TYPE_PERFORMANCE:
            type_str = "PERFORMANCE";
            break;

        case GL_DEBUG_TYPE_OTHER:
            type_str = "OTHER";
            break;

        case GL_DEBUG_TYPE_MARKER:
            type_str = "MARKER";
            break;

        default:
            type_str = "UNKNOWN";
            break;
    }

    switch (severity) 
    {
        case GL_DEBUG_SEVERITY_HIGH:
            severity_str = "HIGH";
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            severity_str = "MEDIUM";
            break;

        case GL_DEBUG_SEVERITY_LOW:
            severity_str = "LOW";
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severity_str = "NOTIFICATION";
            break;

        default:
            severity_str = "UNKNOWN";
            break;
    }


    std::ostringstream oss;
    Logger lgr{ oss };
    lgr << Logger::nltb << "GL DEBUG CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "**GL ERROR**" : "") 
        << Logger::addt
        << Logger::nltb << "Source:   "	<< source_str
        << Logger::nltb << "Type:     "	<< type_str
        << Logger::nltb << "Id:       " << id
        << Logger::nltb << "Severity: " << severity_str
        << Logger::nltb << "Message:  "	<< message << '\n' << std::endl;

    g_log << oss.str();

    if (type == GL_DEBUG_TYPE_ERROR)
    {
        throw std::runtime_error(oss.str());
    }
}

void init(std::string const context_name)
{
    //g_log << "\nInitializing '" << context_name << "' context.\n" << std::endl;

    // Load all OpenGL function pointers
    if (!gladLoadGL())
    {
        throw std::runtime_error("Failed to initialize GLAD.");
    }


    // Set a debug output callback
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);						// make possible to access the stacktrace from debugMessage_callback
    glDebugMessageCallback(debugMessage_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
}


void print_contextInfos(std::string const context_name)
{
    g_log << "\nInformations about '" << context_name << "' context:\n" << std::endl;


    print_openGLVersion_infos();

    
    //--- Textures limits

    GLint max_texturesSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texturesSize);
    g_log << "Max texture size: " << max_texturesSize << std::endl;

    GLint maxTextures_in_fragmentShader = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures_in_fragmentShader);
    g_log << "Max number of textures that can be accessed in fragment shader: " << maxTextures_in_fragmentShader << std::endl;

    GLint max_boundableTextures = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_boundableTextures);
    g_log << "Max number of textures that can be bound: " << max_boundableTextures << std::endl;

    GLint max_arrayTexture_layers = 0;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_arrayTexture_layers);
    g_log << "Max layers in a texture array: " << max_arrayTexture_layers << std::endl;
        

    //--- Uniforms limits

    GLint max_uniform_block_size = 0;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_block_size);
    g_log << "Max uniform block size: " << max_uniform_block_size << std::endl;
        

    //--- Atomic counter limits
        
    GLint max_combined_atomicCounters = 0;
    glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS, &max_combined_atomicCounters);
    g_log << "Max combined atomic counters: " << max_combined_atomicCounters << std::endl;

    GLint max_fragment_atomicCounters = 0;
    glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS, &max_fragment_atomicCounters);
    g_log << "\tMax fragment atomic counters: " << max_fragment_atomicCounters << std::endl;

        
    //--- SSBO limits

    GLint max_SSBO_bindings = 0;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &max_SSBO_bindings);
    g_log << "Max SSBO bindings: " << max_SSBO_bindings << std::endl;

    GLint max_combined_SSBO_count = 0;
    glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &max_combined_SSBO_count);
    g_log << "Max combined SSBO blocks: " << max_combined_SSBO_count << std::endl;

    GLint max_fragment_SSBO_count = 0;
    glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &max_fragment_SSBO_count);
    g_log << "\tMax fragment SSBO blocks: " << max_fragment_SSBO_count << std::endl;

    GLint max_SSBO_size = 0;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &max_SSBO_size);
    g_log << "Max SSBO size: " << max_SSBO_size << std::endl;
}

auto glfwScreenRS_to_openGlFramebufferRS(Vector2f const glfw_cursorPos, Vector2i const screen_size, Vector2i const framebuffer_size) -> Vector2f
{
    //From "GLFW screen window reference system" to "GLFW framebuffer window reference system"
    auto pixel_pos = Vector2f{
        glfw_cursorPos.x / screen_size.x * framebuffer_size.x,
        glfw_cursorPos.y / screen_size.y * framebuffer_size.y,
    };

    //OpenGL has a different screen reference system. Need to change origin and y-axis direction
    return { pixel_pos.x, framebuffer_size.y - pixel_pos.y - 1 }; // "-1" because the GLFW pixel is identified by its top-left corner, whereas the OpenGL 
                                                                  // pixel is identified by its bottom-left corner.
}

    
auto fragmentRS_to_openGlWorldSpaceRS(Vector2f const fbo_pos, float const z_depth, glm::mat4 const view, glm::mat4 projection, Vector2i const framebuffer_size) -> Vector3f
{
    auto const frag_pos = glm::vec3{ fbo_pos.x + 0.5f, fbo_pos.y + 0.5f, z_depth };			// center of the fragment
    auto const viewport = glm::vec4{ 0, 0, framebuffer_size.x, framebuffer_size.y };		// window rectangle 
    auto const world = glm::unProject(frag_pos, view, projection, viewport);				// (in pixels -- OpenGL world space reference system)

    return { world.x, world.y, world.z };
}


    
void debug_printTexture(GLuint const texture_id, unsigned const tex_width, unsigned const tex_height, TextureFormat const format, 
                        GLint const beg_x, GLint const beg_y, GLsizei const rect_width, GLsizei const rect_height)
{
    if (format != TextureFormat::DepthStencil)
    {
        std::vector<GLubyte> texels;

        auto channels = 0u;
        auto gl_format = static_cast<GLenum>(0u);
        auto gl_type = static_cast<GLenum>(0u);

        switch (format)
        {
            case TextureFormat::Red:
                channels = 1;
                gl_format = GL_RED;
                gl_type = GL_UNSIGNED_BYTE;
                break;

            case TextureFormat::RGB:
                channels = 3;
                gl_format = GL_RGB;
                gl_type = GL_UNSIGNED_BYTE;
                break;

            case TextureFormat::RGBA:
                channels = 4;
                gl_format = GL_RGBA;
                gl_type = GL_UNSIGNED_BYTE;
                break;

            default:
                throw std::runtime_error("Unexpected texture format.");
                break;
        }

        auto buffer_size = channels * tex_width * tex_height;
        texels.insert(texels.end(), buffer_size, 0u);


        g_log << "Checking " << channels << "-channel texture:" << std::endl;
            
        glPixelStorei(GL_PACK_ALIGNMENT, 1); // Removes the padding at the end of a texture row, so that the data read from glGetTexImage() are tightly packed.

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
            glGetTexImage(GL_TEXTURE_2D, 0, gl_format, gl_type, (void*) texels.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    

        auto const end_x = beg_x + rect_width;
        auto const end_y = beg_y + rect_height;

        switch (format)
        {
            case TextureFormat::Red:
            {
                for (int x = beg_x; x < end_x; ++x)
                {
                    for (int y = beg_y; y < end_y; ++y)
                    {
                        auto const cast_y = static_cast<decltype(texels)::size_type>(y);

                        g_log << static_cast<unsigned int>(texels[(cast_y * tex_width + x)]) << "    ";
                    }

                    g_log << "\n\n";
                }

                break;
            }

            case TextureFormat::RGB:
            {
                for (int x = beg_x; x < end_x; ++x)
                {
                    for (int y = beg_y; y < end_y; ++y)
                    {
                        auto const cast_y = static_cast<decltype(texels)::size_type>(y);

                        g_log << "(" << static_cast<unsigned int>(texels[(cast_y * tex_width + x) * channels])     << ", "
                                     << static_cast<unsigned int>(texels[(cast_y * tex_width + x) * channels + 1]) << ", "
                                     << static_cast<unsigned int>(texels[(cast_y * tex_width + x) * channels + 2]) << ")   ";
                    }

                    g_log << "\n\n";
                }

                break;
            }

            case TextureFormat::RGBA:
            {
                for (int x = beg_x; x < end_x; ++x)
                {
                    for (int y = beg_y; y < end_y; ++y)
                    {
                        auto const cast_y = static_cast<decltype(texels)::size_type>(y);

                        g_log << "(" << static_cast<unsigned int>(texels[(cast_y * tex_width + x) * channels])     << ", "
                                     << static_cast<unsigned int>(texels[(cast_y * tex_width + x) * channels + 1]) << ", "
                                     << static_cast<unsigned int>(texels[(cast_y * tex_width + x) * channels + 2]) << ", "
                                     << static_cast<unsigned int>(texels[(cast_y * tex_width + x) * channels + 3]) << ")   ";
                    }

                    g_log << "\n\n";
                }

                break;
            }

            default:
                throw std::runtime_error("Unexpected texture format.");
                break;
        }
    }
    else if(format == TextureFormat::DepthStencil)
    {
        std::vector<GLuint> depthsAndStencils;

        auto gl_format = GL_DEPTH_STENCIL;
        auto gl_type = GL_UNSIGNED_INT_24_8;

        auto buffer_size = tex_width * tex_height;
        depthsAndStencils.insert(depthsAndStencils.end(), buffer_size, 0u);


        g_log << "Checking depth-stencil texture:" << std::endl;

        glPixelStorei(GL_PACK_ALIGNMENT, 1); // Removes the padding at the end of a texture row, so that the data read from glGetTexImage() are tightly packed.

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
            glGetTexImage(GL_TEXTURE_2D, 0, gl_format, gl_type, (void*) depthsAndStencils.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    

        auto const end_x = beg_x + rect_width;
        auto const end_y = beg_y + rect_height;

        for (int x = beg_x; x < end_x; ++x)
        {
            for (int y = beg_y; y < end_y; ++y)
            {
                auto const cast_y = static_cast<decltype(depthsAndStencils)::size_type>(y);
                auto const depthAndStencil = static_cast<unsigned int>(depthsAndStencils[(cast_y * tex_width + x)]);

                // Split depthAndStencil in the depth component and in the stencil index.
                g_log << ((depthAndStencil & 0xFFFFFF00) >> 8) << " - " << (depthAndStencil & 0x000000FF) << "    "; 
            }

            g_log << "\n\n";
        }
    }
    else
    {
        throw std::runtime_error("Unexpected texture format.");
    }

        

    g_log << std::endl;
}
    

void debug_printDrawArraysIndirectCommandBuffer(GLuint const cmd_id)
{
    DrawArraysIndirectCommand const command = { 0u, 0u, 0u, 0u };

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmd_id);
        glGetBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawArraysIndirectCommand), (void*)&command);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

    g_log << "DrawArraysIndirectCommand {\n"
              << "\tcount:         " << command.count << '\n'
              << "\tprim_count:    " << command.primCount << '\n'
              << "\tfirst:         " << command.first << '\n'
              << "\tbase_instance: " << command.baseInstance << '\n' 
              << "}\n" << std::endl;
}



} //namespace GraphicsManagerCore



} //namespace tgm