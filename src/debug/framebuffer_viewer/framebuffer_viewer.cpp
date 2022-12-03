#include "framebuffer_viewer.hh"


#include <sstream>

#include "settings/graphics_settings.hh"


namespace tgm
{



FramebufferViewer::FramebufferViewer() :
	m_window{ windows_manager().create_window() }
{ }

FramebufferViewer::~FramebufferViewer()
{
	if (m_open)
	{
		std::cout << "WARNING: Trying to destroy a not closed FramebufferViewer. You must close the viewer before its destruction." << std::endl;
	}
}


void FramebufferViewer::open(std::string const title, GLuint const texture, GLsizei const width, GLsizei const height, std::string const shader_name)
{
	if (GSet::video_mode.fullscreen()) // Opening framebuffer viewer in fullscreen mode it's complicated.
	{ 
		std::cout << "Cannot open " << title << " FramebufferViewer while in fullscreen mode." << std::endl;
		return; 
	}

	#if DYNAMIC_ASSERTS
		if (m_open) { throw std::runtime_error("Trying to reopen an already open FramebufferViewer"); }
	#endif


	m_texture = texture;


	auto window_title = "Framebuffer Viewer - " + title;

	auto previous_window = windows_manager().activeWindow_id();

	WindowOptions opt{};
	opt.title = window_title;
	opt.width = width;
	opt.height = height;
	opt.resizable = false;
	opt.shared_context = WindowManager::mainWindow_id;

	std::cout << "Framebuffer window: " << m_window << std::endl;
	m_window.open(opt);


	// LOAD SHADER
	std::string const fbwShaders_dir{"framebuffer_viewer/"};
	auto const vshader_path = fbwShaders_dir + "default.vshader";
	auto fshader_path = fbwShaders_dir;
	if (shader_name.empty())
		fshader_path += "default.fshader";
	else
		fshader_path += "custom/" + shader_name + ".fshader";

	m_shader.load(vshader_path, fshader_path);



	//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClearColor(1.f, 0.f, 0.f, 1.0f);


	
	// WINDOW VAO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
		
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_VAO_vertices.size(), m_VAO_vertices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind VBO
	glBindVertexArray(0); //unbind VAO

	
	m_open = true;


	// Restore the previous target, because otherwise update() cannot realize which was the previous target to restore
	if (previous_window)
	{
		windows_manager().activate_window(previous_window.value());
	}
}


void FramebufferViewer::update()
{
	if (GSet::video_mode.fullscreen()) { return; } // Opening framebuffer viewer in fullscreen mode it's complicated.

	#if DYNAMIC_ASSERTS
		assert_open();
	#endif		
	

	auto previous_window = windows_manager().activeWindow_id();

	m_window.activate();


	glClear(GL_COLOR_BUFFER_BIT);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	// Bind a shader specifically created for the screen quad
	m_shader.bind();

	glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLuint>(m_VAO_vertices.size() / 4));
	glBindVertexArray(0);//unbind

	m_window.display();

	glBindTexture(GL_TEXTURE_2D, 0); //unbind


	// Restore the previous RenderTarget
	if (previous_window)
	{
		windows_manager().activate_window(previous_window.value());
	}
}


void FramebufferViewer::close()
{
	if (GSet::video_mode.fullscreen()) { return; } // Opening framebuffer viewer in fullscreen mode it's complicated.

	#if DYNAMIC_ASSERTS
		assert_open();
	#endif


	auto previous_window = windows_manager().activeWindow_id();
	m_window.activate();


	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);


	m_open = false;

	m_window.close();
	
	// Restore the previous RenderTarget
	if (previous_window)
	{
		windows_manager().activate_window(previous_window.value());
	}

}


void FramebufferViewer::assert_open() const
{
	if (!m_open) { throw std::runtime_error("Unexpected situation. This FramebufferViewer is closed."); }
}



} // namespace tgm