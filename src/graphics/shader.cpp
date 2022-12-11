#include "shader.hh"


#include "debug/logger/debug_printers.hh"


namespace tgm
{



auto operator<<(std::ostream & os, ShaderType const scp)->std::ostream &
{
	switch (scp)
	{
		case ShaderType::Vertex:
			os << "Vertex";
			break;
		case ShaderType::Fragment:
			os << "Fragment";
			break;
		default:
			break;
	}

	return os;
}


void Shader::load_from_multipleFiles(std::vector<std::string> const& vshader_paths, std::vector<std::string> const& fshader_paths, 
									 std::vector<std::pair<std::string, std::string>> const& preprocessor_defs)
{
	//--- Build the head of the shaders with #verison definition and user_defined preprocessor #define-s.
	std::string shader_head{};
	if (!preprocessor_defs.empty())
	{
		shader_head += "#version 430 core\n";
		for (auto const& p : preprocessor_defs)
		{
			shader_head += "#define " + p.first + " " + p.second + "\n";
		}
	}

	//--- Retrieve the vertex/fragment source code from filePath
	std::vector<std::string> vshader_codeChunks;
	for (auto const& path : vshader_paths)
	{
		vshader_codeChunks.emplace_back(load_codeChunk(path));
		//std::cout << vshader_codeChunks.back() << "\n";
	}

	std::vector<std::string> fshader_codeChunks;
	for (auto const& path : fshader_paths)
	{
		fshader_codeChunks.emplace_back(load_codeChunk(path));
		//std::cout << fshader_codeChunks.back() << "\n";
	}

	//--- Convert C++ strings to C strings
	std::vector<char const*> vshader_cstrings = {shader_head.c_str()},
							 fshader_cstrings = {shader_head.c_str()};

	for (auto const& str : vshader_codeChunks)
		vshader_cstrings.push_back(str.c_str());

	for (auto const& str : fshader_codeChunks)
		fshader_cstrings.push_back(str.c_str());


	// Compile shaders

	GLuint vertex = 0,
		   fragment = 0;

	//Create and compile vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, static_cast<GLsizei>(vshader_cstrings.size()), vshader_cstrings.data(), NULL);
	glCompileShader(vertex);
	check_compileErrors(vertex, ShaderType::Vertex);

	//Create and compile fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, static_cast<GLsizei>(fshader_cstrings.size()), fshader_cstrings.data(), NULL);
	glCompileShader(fragment);
	check_compileErrors(fragment, ShaderType::Fragment);

	//Create and link shader program
	m_id = glCreateProgram();
	glAttachShader(m_id, vertex);
	glAttachShader(m_id, fragment);
	glLinkProgram(m_id);
	check_linkingErrors(m_id);
	static std::string shader_label = vshader_paths[0];
	glObjectLabel(GL_PROGRAM, m_id, static_cast<GLsizei>(shader_label.size()), shader_label.data());

	//Delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);


	m_loaded = true;
}


auto Shader::load_codeChunk(std::string const& path) const -> std::string
{
	std::string chunk_code; //NRVO

	std::ifstream chunk_ifstream;

	//Ensure ifstream objects can throw exceptions
	chunk_ifstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		chunk_ifstream.open(shaders_dir + path);

		std::stringstream chunk_sstream;

		//Read file's buffer contents into streams
		chunk_sstream << chunk_ifstream.rdbuf();

		chunk_ifstream.close();

		//Convert stream into string
		chunk_code = chunk_sstream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::ostringstream oss; oss << "Cannot read the shader file '" << path << "'";
		throw std::runtime_error(oss.str());
	}

	return chunk_code;
}


void Shader::check_compileErrors(GLuint const shader_id, ShaderType const type) const
{
	GLint success;
	char infoLog[1024];

	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader_id, 1024, NULL, infoLog);

		std::cout << type << " shader compilation error"
					<< "\n------------------------------------------------------\n"
					<< infoLog
					<< "\n------------------------------------------------------\n"
					<< std::endl;

		throw std::runtime_error("Shader compilation error");
	}
}

void Shader::check_linkingErrors(GLuint const program_id) const
{
	GLint success;
	char infoLog[1024];

	glGetProgramiv(program_id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program_id, 1024, NULL, infoLog);

		std::cout << "Shader linking error" 
					<< "\n------------------------------------------------------\n"
					<< infoLog
					<< "\n------------------------------------------------------\n" 
					<< std::endl;

		throw std::runtime_error("Shader linking error");
	}
}



} // namespace tgm