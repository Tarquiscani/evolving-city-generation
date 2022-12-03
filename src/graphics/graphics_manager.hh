#ifndef GM_GRAPHICS_MANAGER_HH
#define GM_GRAPHICS_MANAGER_HH

#include <glad/glad.h>
#define GLM_FORCE_SILENT_WARNINGS
#include <glm/glm.hpp>

#include "graphics/opengl/draw_arrays_indirect_command.hh"
#include "graphics/textures/texture_2d.hh"
#include "graphics/textures/texture_2d_array.hh"
#include "shader.hh"
#include "camera.hh"
#include "world_parallelepiped.hh"
#include "tile_vertices.hh"
#include "dynamic_vertices.hh"
#include "roof_vertices.hh"

#include "debug/framebuffer_viewer/framebuffer_viewer.hh"

//TODO: NOW: Forse andrebbe rinominato GPU manager
class GraphicsManager
{
	public:
		////
		//  @camera_target: The target towards the camera must point. 
		//	@camera_zoom: Multiplier of the distance polar coordinate of the camera. 1 is equivalent to 10 tiles of distance from the target.
		//  @camera_phi, @camera_theta: Angular polar coordinates of the camera (the origin is @camera_target) (in degrees).
		////
		GraphicsManager(Vector2i const defaultFbo_size, Vector2i const window_size,
						TileVertices & tile_vertices, DynamicVertices const* dynamic_vertices, RoofVertices & roof_vertices, 
						Camera & camera);
		GraphicsManager::~GraphicsManager();
		
		void init();

		void resize_fbo(Vector2i const new_fbo_size);
		void resize_window(Vector2i const new_window_size) noexcept;
		void draw();

		auto glfwWindowPixel_to_mapTile(Vector2f const glfw_cursorPos) const -> Vector3i;

	private:
		bool m_is_init = false;

		Vector2i m_defaultFbo_size;
		Vector2i m_window_size;

		Shader m_tile_main_shader;
		Shader m_dynamic_main_shader;
		Shader m_edgeableIds_main_shader;
		static constexpr int tilesetTexarray_unit = 1;
		static constexpr int dynamicTexture_unit = 2;
		static constexpr int roofTexture_unit = 3;

		#if OCCLUSION_CULLING
			Shader m_entityIds_shader;
			Shader m_visibleBufferBuilder_shader;
			static constexpr GLuint entityIdsTexture_unit = 10;
			static constexpr GLuint visibleVertexACB_unit = 0;
			static constexpr GLuint visibleTileACB_unit = 1;
			static constexpr GLuint evaluatedEntitiesSSBO_unit = 10;
			static constexpr GLuint tileBufferSSBO_unit = 11;
			static constexpr GLuint visibleTileBufferSSBO_unit = 12;
		#endif

		#if EDGE_DETECTION_FILTER
			Shader m_edgeDetector_shader;
			static constexpr int edgeDetectorEdgeableIdsTex_unit = 16;
			Shader m_edgeThickener_shader;
			static constexpr int edgeThickenerEdgesTex_unit = 16;
			Shader m_edgeTextureMixer_shader;
			static constexpr int edgeTextureMixerTexturedColorTex_unit = 15;
			static constexpr int edgeTextureMixerThickenedEdgesTex_unit = 16;
		#endif
		
		#if OVERDRAW_FILTER
			Shader m_overdraw_screenShader;
			static constexpr int overdrawDepthStencilTex_unit = 16;
		#endif


		// An alias useful to bind the default framebuffer
		static GLuint constexpr default_FBO = 0;



		#if EDGE_DETECTION_FILTER

			#if ALPHA_TO_COVERAGE
				GLuint m_edfScene_msFBO = 0;
				GLuint m_edfScene_msTexturedColorRBO = 0;
				GLuint m_edfScene_msEdgeableIdsTex = 0;
				GLuint m_edfScene_msDepthStencilRBO = 0;				
			#endif

			GLuint m_edfScene_FBO = 0;
			GLuint m_edfScene_texturedColorTex = 0;
			GLuint m_edfScene_edgeableIdsTex = 0;
			GLuint m_edfScene_depthStencilRBO = 0;

			GLuint m_edfEdges_FBO = 0;
			GLuint m_edfEdges_colorTex = 0;

			GLuint m_edfThickenedEdges_FBO = 0;
			GLuint m_edfThickenedEdges_colorTex = 0;

			
			#if EDGE_DETECTION_FILTER_INTERMEDIATE_STEPS
				FramebufferViewer m_textured_viewer;
				FramebufferViewer m_edgeableIds_viewer;
				FramebufferViewer m_edges_viewer;
			#endif
		#endif

		#if OVERDRAW_FILTER
			GLuint m_overdraw_FBO = 0;
			GLuint m_overdraw_colorRBO = 0;
			GLuint m_overdraw_depthStencilTex = 0;
		#endif
			

		// Quad that spans the whole window. Used to render textures.
		std::vector<float> m_windowQuad_vertices;
		GLuint m_windowQuad_VAO = 0;
		GLuint m_windowQuad_VBO = 0;


		GLuint m_tile_VBO = 0;
		GLuint m_tile_VAO = 0;


		#if OCCLUSION_CULLING
			//Framebuffer used to compute the texture of the entity ids
			GLuint m_entityIds_FBO = 0;
			GLsizei m_entityIdsFBO_width = 0;
			GLsizei m_entityIdsFBO_height = 0;
			GLuint m_entityIds_tex = 0;
			GLuint m_entityIds_depthStencilRBO = 0;

			#if SHOW_VISIBLE_ENTITIES_FBO
				FramebufferViewer m_entityIds_viewer;
			#endif

		
			//Framebuffer used to build the VBO of visible tiles 
			GLuint m_visibleBufferBuilder_FBO = 0;
			GLuint m_visibleBufferBuilder_colorRBO = 0;

			//Buffer of the indirect command used to draw visible tiles
			GLuint m_visibleTile_indirectCommandBuffer = 0;

			//Visible vertex atomic counter
			static const GLuint m_visibleVertexACB_clearValue = 0;
			GLuint m_visibleVertex_ACB = 0;

			//Visible tile atomic counter
			static const GLuint m_visibleTileACB_clearValue = 0u;
			GLuint m_visibleTile_ACB = 0;

			// SSBO of the already evaluated entities (used in visible_buffer_builder.fshader). Each GLuint stores 32 bits, each one representing a boolean flag 
			// that indicates whether the corresponding entity_id has been already evaluated.
			GLuint m_evaluatedEntities_SSBO = 0;
		
			//Buffer of the visible tiles
			GLuint m_visibleTile_buffer = 0;
			GLuint m_visibleTile_VAO = 0;
		#endif

		GLuint m_dynamic_VBO = 0;
		GLuint m_dynamic_VAO = 0;

		GLuint m_building_VBO = 0;
		GLuint m_building_VAO = 0;

		GLuint m_roofSouth_VBO = 0;
		GLuint m_roofSouth_VAO = 0;
		GLuint m_roofWest_VBO = 0;
		GLuint m_roofWest_VAO = 0;
		GLuint m_roofNorth_VBO = 0;
		GLuint m_roofNorth_VAO = 0;
		GLuint m_roofEast_VBO = 0;
		GLuint m_roofEast_VAO = 0;


		////
		//	Generate all the objects required to render the scene. Mainly FBOs, VBOs, VAOs, textures.
		//	Note: The function must be called only after TileVertices have been initialized in GameMap. 
		////
		void generate_objects();
		void free_objects();

		////
		//	Prepare a quad that spans the whole window. The quad is used to render textures and framebuffers
		////
		void prepare_windowQuadVAO();

		void generate_tileObjects();
		void free_tileObjects();

		#if OCCLUSION_CULLING
			void generate_occlusionCullingObjects();
			void free_occlusionCullingObjects();
		#endif

		void generate_freeTriangleVerticesObjects(GLuint & VBO_id, GLuint & VAO_id, FreeTriangleVertices const& ftv);


		#if GSET_TILESET_TEXARRAY
			GLuint m_tilesetTexarray_id = 0;
			Texture2DArray & m_tileset_texarray = default_texture_tileset;
		#else
			GLuint m_tilesetTexture_id = 0;
			Texture2D & m_tileset_texture = default_texture_tileset;
		#endif

		GLuint m_dynamicTexture_id = 0;
		Texture2D & m_dynamic_texture = default_texture_dynamics;

		GLuint m_roofTexture_id = 0;
		Texture2D & m_roof_texture = roof_texture;


		TileVertices & m_tile_vertices;
		DynamicVertices const& m_dynamic_vertices;
		RoofVertices & m_roof_vertices;

		Camera & m_camera;

		Vector3f m_light_pos{ 0.7f, -0.5f, 1.f };

		
		////
		// I can't create null textures to attach to the framebuffers, so I must check if the framebuffer has
		// non-null dimensions before doing any operation.
		////
		bool is_defaulFbo_null() const noexcept { return m_defaultFbo_size.x <= 0 || m_defaultFbo_size.y <= 0; }

		////
		//	Draw the scene without any filter.
		////
		void drawScene_without_filters();


		#if EDGE_DETECTION_FILTER
			void generate_edgeDetectionFilterObjects();
			void free_edgeDetectionFilterObjects();

			////
			//	Draw the scene applying the edge-detection filter.
			////
			void drawScene_with_edgeDetectionFilter();
		#endif
		
		#if OVERDRAW_FILTER
			void generate_overdrawObjects();
			void free_overdrawObjects();
			
			////
			//	Draw the scene applying the overdraw filter. It uses the stencil buffer to store how many times the same pixel is drawn.
			////
			void drawScene_with_overdrawFilter();
		#endif
			
		////
		//	Draw the vertices on the current framebuffer.
		////
		void draw_scene();

		////
		//	Bind @shader and set all the uniforms needed to draw the scene.
		////
		void set_sceneUniforms(Shader & shader, glm::mat4 const& view, glm::mat4 const& projection, bool const are_noRoofRects_changed) const;

		////
		//	Starting from the buffers of all the entities it builds the buffer of the visible entities.
		////
		void occlusion_culling();

		////
		//	Compute the smallest parallelepiped capable of containing the whole map (in units -- OpenGL world space reference system).
		//	Note: It doesn't contain triangles that are one floor above the last floor of the map. (e.g. if a the map has an height of 50
		//		  and floors_distance is 1000, the last floor is at 49000 and the map_box doesn't contain triangles that lies above 50000)
		////
		auto compute_mapBox() const -> WorldParallelepiped;

		auto compute_occlusionViewMatrix() const -> glm::mat4;
		auto compute_occlusionProjMatrix() const -> glm::mat4;

		auto compute_viewMatrix(WorldParallelepiped const& map_box) const -> glm::mat4;
		auto compute_projectionMatrix(WorldParallelepiped const& map_box) const -> glm::mat4;

		////
		//	Compute the max mipmap levels for a texture. The formula is that of the OpenGL specifications.
		////
		static auto compute_maxMipmapLevels(float const tex_width, float const tex_height) -> GLsizei { return static_cast<GLsizei>(std::log2(std::max(tex_width, tex_height)) + 1.f); }

		////
		//	@world_z: Whatever z-coordinate (in units -- wolrd space r.s.).
		//
		//	@return: A very high z-coordinate belonging to that floor, before the upper floor begins (in units -- world space r.s.).
		////
		static auto floor_highestZ(float const world_z) -> float;
		
		////
		//	@window_coord_x, @window_coord_y: (in pixels -- screen reference system)
		//
		//	@return: (in units -- map reference system)
		////
		auto glfwWindowPixel_to_mapUnits(Vector2f const glfw_cursorPos) const -> Vector3f;

		
		void rebuff_freeTriangleVerticesVBO(GLuint VBO_id, FreeTriangleVertices const& ftv);

		void draw_genericVAO(GLuint VAO_id, FreeTriangleVertices const& ftv);

		
		////
		//	Print a rectangle of pixels from the color attachment of the current framebuffer. It works only with color attachements whose 
		//	underlying format is one-channel unsigned integer (GL_R32UI)
		//	@x, @y, @width, @height: (OpenGL texture reference system)
		////
		void debug_printUnsignedIntBuffer(GLint const beg_x, GLint const beg_y, GLsizei const width, GLsizei const height);

		void debug_printSSBO(GLuint const SSBO, GLintptr const offset, GLsizeiptr const count);

		////
		//	Copy the content of @buffer in a std::vector, allowing to inspect him easily with Visual Studio debugger.
		////
		void debug_inspectTilesetVertexDataBuffer(GLuint const buffer, GLsizeiptr const vertex_count);
		void debug_printTilesetVertexDataBuffer(GLuint const buffer, GLintptr const offset, GLsizeiptr const length);
};

#endif //GM_GRAPHICS_MANAGER_HH