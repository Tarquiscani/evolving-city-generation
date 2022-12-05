#include "visual_debug_graphics_manager.hh"

#if VISUALDEBUG

#include <iomanip>
#define GLM_FORCE_SILENT_WARNINGS
#include <glm/glm.hpp>

#include "graphics/graphics_manager_core.hh"

namespace tgm
{



VisualDebugGraphicsManager::VisualDebugGraphicsManager(DebugVertices & vertices) :
	m_vertices(vertices) {}


void VisualDebugGraphicsManager::init(Vector2i const fbo_size, Vector2i const window_size)
{
	m_defaultFbo_size = fbo_size;
	m_window_size = window_size;


	GraphicsManagerCore::init("Visual debug");


	m_shader.load("visual_debug/visual_debug.vert", "visual_debug/visual_debug.frag");


	//glEnable(GL_DEPTH_TEST); //with enabled depth test I couldn't see highlightings that lied on the same z
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	generate_objects();
}

void VisualDebugGraphicsManager::generate_objects()
{
	//--- Tile VAO
	glGenVertexArrays(1, &m_tile_VAO);
	glGenBuffers(1, &m_tile_VBO);

	glBindVertexArray(m_tile_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_tile_VBO);
			glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

			static char const tile_VAO_label[] = "VisualDebug tile_VAO";
			glObjectLabel(GL_VERTEX_ARRAY, m_tile_VAO, sizeof(tile_VAO_label), tile_VAO_label);
			static char const tile_VBO_label[] = "VisualDebug tile_VBO";
			glObjectLabel(GL_BUFFER, m_tile_VBO, sizeof(tile_VBO_label), tile_VBO_label);

			// world position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertexData), nullptr);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(DebugVertexData), (void*)(offsetof(DebugVertexData, color)));
			glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void VisualDebugGraphicsManager::draw()
{
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/);


	glBindBuffer(GL_ARRAY_BUFFER, m_tile_VBO);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.byte_size(), m_vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_tile_VBO);


	m_shader.bind();
	m_shader.set_mat4("u_view", compute_viewMatrix());
	m_shader.set_mat4("u_projection", compute_projectionMatrix(m_vertices.frame_inPixels()));


	glBindVertexArray(m_tile_VAO);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertices.vertex_count()));
	glBindVertexArray(0);
}

auto VisualDebugGraphicsManager::compute_viewMatrix() -> glm::mat4
{
	return glm::mat4(1.f); //identity, I don't need to change view, the vertices are already defined in the correct view space
}

auto VisualDebugGraphicsManager::compute_projectionMatrix(FloatParallelepiped const& frame) -> glm::mat4
{
	return glm::ortho(0.f, frame.width, -frame.length, 0.f, -10.f, 10.f);
}

void VisualDebugGraphicsManager::resize_fbo(Vector2i const new_fbo_size)
{
	m_defaultFbo_size = new_fbo_size;

	glViewport(0, 0, new_fbo_size.x, new_fbo_size.y);
}

void VisualDebugGraphicsManager::resize_window(Vector2i const new_window_size)
{
	m_window_size = new_window_size;
}

void VisualDebugGraphicsManager::destroy_objects()
{
	glDeleteVertexArrays(1, &m_tile_VAO);
	glDeleteBuffers(1, &m_tile_VBO);
}



auto VisualDebugGraphicsManager::glfwScreenPixel_to_framePos(Vector2f const glfw_cursorPos, FloatParallelepiped const& frame) -> Vector2f
{
	auto const fbo_pos = GraphicsManagerCore::glfwScreenRS_to_openGlFramebufferRS(glfw_cursorPos, m_window_size, m_defaultFbo_size);

	auto const frame_pos = GraphicsManagerCore::fragmentRS_to_openGlWorldSpaceRS(fbo_pos, 0.f, compute_viewMatrix(), compute_projectionMatrix(frame), m_defaultFbo_size);

	return { - frame_pos.y, frame_pos.x };
}



} //namespace tgm



#endif //VISUALDEBUG