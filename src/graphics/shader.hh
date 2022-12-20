#ifndef GM_SHADER_HH
#define GM_SHADER_HH


#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#define GLM_FORCE_SILENT_WARNINGS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "system/vector2.hh"
#include "system/vector3.hh"

namespace tgm
{


enum class ShaderType
{
    Vertex,
    Fragment,
};


auto operator<<(std::ostream & os, ShaderType const scp) -> std::ostream &;


class Shader
{
    public:

        void load(std::string const vshader_path, std::string const fshader_path)
        {
            load_from_multipleFiles({ vshader_path }, { fshader_path });
        }

        ////
        //	@vshader_paths, @fshader_paths: Paths of multiple files that chained together forms the complete code of the shader.
        //	@preprocessor_defs: Each element of the vector is a preprocessor definition. Each pair contains the name of the identifier and its value.
        //						Note: don't use boolean literals ("true"/"false") with GLSL preprocessor directives, they aren't supported. Use number literals instead.
        ////
        void load_from_multipleFiles(std::vector<std::string> const& vshader_paths, std::vector<std::string> const& fshader_paths, 
                                     std::vector<std::pair<std::string, std::string>> const& preprocessor_defs = std::vector<std::pair<std::string, std::string>>{});

        void set_fragDataLocation(GLuint const color_number, char const* name)
        {
            assert_loaded();

            glBindFragDataLocation(m_id, color_number, name);
        }

        ////
        //  Bind this shader.
        ////
        void bind() const
        {
            assert_loaded();

            glUseProgram(m_id);
        }


        void set_bool(char const* name, bool const value) const
        {
            glProgramUniform1i(m_id, glGetUniformLocation(m_id, name), static_cast<int>(value));
        }

        void set_int(char const* name, int const value) const
        {
            glProgramUniform1i(m_id, glGetUniformLocation(m_id, name), value);
        }

        void set_uint(char const* name, unsigned int const value) const
        {
            glProgramUniform1ui(m_id, glGetUniformLocation(m_id, name), value);
        }

        void set_float(char const* name, float const value) const
        {
            glProgramUniform1f(m_id, glGetUniformLocation(m_id, name), value);
        }

        void set_vec2(char const* name, Vector2f const v) const
        {
            glProgramUniform2f(m_id, glGetUniformLocation(m_id, name), v.x, v.y);
        }

        void set_vec3(char const* name, Vector3f const v) const
        {
            glProgramUniform3f(m_id, glGetUniformLocation(m_id, name), v.x, v.y, v.z);
        }

        void set_mat4(char const* name, glm::mat4 const& m) const
        {
            glProgramUniformMatrix4fv(m_id, glGetUniformLocation(m_id, name), 1, false, glm::value_ptr(m)); //conversion from glm-like matrix to OpenGL-like matrix pointer
        }

        void set_vec2array(char const* name, Vector2f const* vector_array, std::size_t const length) const
        {
            auto const contiguous = flatten(vector_array, length);

            glProgramUniform2fv(m_id, glGetUniformLocation(m_id, name), static_cast<GLsizei>(length), contiguous.data());
        }

        auto debug_getId() const -> GLuint { assert_loaded(); return m_id; }


    private:
        static inline std::string const shaders_dir = "shaders/";

        GLuint m_id = 0;
        bool m_loaded = false;

        void assert_loaded() const
        {
            if (!m_loaded)
                throw std::runtime_error("Cannot bind an unloaded shader.");
        }

        auto load_codeChunk(std::string const& path) const -> std::string;
        void check_compile_errors(GLuint const shader_id, ShaderType const type, std::vector<std::string> const& paths) const;
        void check_linking_errors(GLuint const program_id, std::vector<std::string> const& vshader_paths, std::vector<std::string> const& fshader_paths) const;


        template <typename T>
        auto flatten(Vector2<T> const* vector_array, std::size_t const length) const -> std::vector<T>
        {
            std::size_t const sfvector_size = 2;

            std::vector<T> contiguous(sfvector_size * length);//NRVO

            for (std::size_t i = 0; i < length; ++i)
            {
                contiguous[sfvector_size * i]	  = vector_array[i].x;
                contiguous[sfvector_size * i + 1] = vector_array[i].y;
            }

            return contiguous; 
        }
};


} //namespace tgm



#endif //GM_SHADER_HH