#include "graphics_manager.hh"


#include <glm/gtc/matrix_transform.hpp>

#include "graphics_manager_core.hh"
#include "utilities/asserts.hh"


namespace tgm
{



//TODO: Find a better location for this functions
#pragma warning(disable: 4505)
static auto operator<<(std::ostream & os, glm::vec2 const& v) -> std::ostream &
{
	os << "glm::vec2(" << std::setw(5) << v.x << " ," << std::setw(5) << v.y << ")";

	return os;

	#pragma warning(default: 4505)
}
#pragma warning(disable: 4505)
static auto operator<<(std::ostream & os, glm::vec3 const& v) -> std::ostream &
{
	os << "glm::vec3(" << std::setw(5) << v.x << " ," << std::setw(5) << v.y << " ," << std::setw(5) << v.z << ")";

	return os;
	
	#pragma warning(default: 4505)
}
#pragma warning(disable: 4505)
static auto operator<<(std::ostream & os, glm::vec4 const& v) -> std::ostream &
{
	os << "glm::vec4(" << std::setw(5) << v.x << " ," << std::setw(5) << v.y << " ," << std::setw(5) << v.z << " ," << std::setw(5) << v.w << ")";

	return os;
	
	#pragma warning(default: 4505)
}

GraphicsManager::GraphicsManager(Vector2i const defaultFbo_size, Vector2i const window_size,
								 TileVertices & tile_vertices, DynamicVertices const* dynamic_vertices, RoofVertices & roof_vertices, 
								 Camera & camera) :
	m_defaultFbo_size(defaultFbo_size), m_window_size(window_size),
	m_tile_vertices(tile_vertices), m_dynamic_vertices(*dynamic_vertices), m_roof_vertices(roof_vertices), 
	m_camera(camera)
{
	assert_nonNullptrs(dynamic_vertices);
	std::cout << "Default FBO resolution: " << defaultFbo_size.x << "x" << defaultFbo_size.y << std::endl;
}

GraphicsManager::~GraphicsManager()
{
	if (m_is_init)
	{
		free_objects();
	}
}


void GraphicsManager::init()
{
	GraphicsManagerCore::init("Main window");
	GraphicsManagerCore::print_contextInfos("Main window");


	//--- Load all the shaders
	m_tile_main_shader.load_from_multipleFiles({ "main_shader.vshader" }, 
											 { "main_shader.fshader", "discard_utilities.fshader" }, 
											 {
												{"SHOW_LOD", "0 //false"},
												{"EDGEABLE_IDS", "0 //false"},
												{"TILE_SHADER", "1 //true"},
	#if GSET_TILESET_TEXARRAY
												{"GSET_TILESET_TEXARRAY", "1 //true"}
	#else
												{"GSET_TILESET_TEXARRAY", "0 //false"}
	#endif
											 }												);			//Don't use boolean literals with GLSL preprocessor directives, they aren't supported.

	m_dynamic_main_shader.load_from_multipleFiles({ "main_shader.vshader" }, 
												{ "main_shader.fshader", "discard_utilities.fshader" },
												{
													{"SHOW_LOD", "0 //false"},
													{"EDGEABLE_IDS", "0 //false"},
													{"TILE_SHADER", "0 //false"},
												}											);			//Don't use boolean literals with GLSL preprocessor directives, they aren't supported.
	
	m_edgeableIds_main_shader.load_from_multipleFiles({ "main_shader.vshader" }, 
													  { "main_shader.fshader", "discard_utilities.fshader" },
													  {
															{"SHOW_LOD", "0 //false"},
															{"EDGEABLE_IDS", "1 //true"},
															{"TILE_SHADER", "0 //false"},
													  }										);			//Don't use boolean literals with GLSL preprocessor directives, they aren't supported.
	
	#if OCCLUSION_CULLING
		m_entityIds_shader.load_from_multipleFiles({ "entity_ids.vshader" }, 
												   { "entity_ids.fshader","discard_utilities.fshader" },
												   {
	#if GSET_TILESET_TEXARRAY
														{"GSET_TILESET_TEXARRAY", "1 //true"}
	#else
														{"GSET_TILESET_TEXARRAY", "0 //false"}
	#endif
												   });
		m_visibleBufferBuilder_shader.load("visible_buffer_builder.vshader", "visible_buffer_builder.fshader");
	#endif

	#if EDGE_DETECTION_FILTER
		m_edgeDetector_shader.load("postprocessing/default.vshader", "postprocessing/edge_detector.fshader");
		m_edgeThickener_shader.load("postprocessing/default.vshader", "postprocessing/erosion.fshader");
		m_edgeTextureMixer_shader.load("postprocessing/default.vshader", "postprocessing/edge_texture_mixer.fshader");
	#endif
		
	#if OVERDRAW_FILTER
		m_overdraw_screenShader.load("postprocessing/default.vshader", "postprocessing/overdraw.fshader");
	#endif


	generate_objects();


	m_is_init = true;
}


void GraphicsManager::generate_objects()
{
	prepare_windowQuadVAO();


	#if EDGE_DETECTION_FILTER
		generate_edgeDetectionFilterObjects();
	#elif OVERDRAW_FILTER
		generate_overdrawObjects();
	#endif


	//--- Occlusion culling objects (must be called after the generation of tile_VAO).
	#if OCCLUSION_CULLING
		generate_occlusionCullingObjects();
	#endif

	

	// DYNAMIC VAO
	glGenVertexArrays(1, &m_dynamic_VAO);
	glGenBuffers(1, &m_dynamic_VBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(m_dynamic_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_dynamic_VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_dynamic_vertices.attribute_count(), m_dynamic_vertices.get_ptr(), GL_DYNAMIC_DRAW);
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// texture coord attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0); 
	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0); 
	


	// Buildings VAO
	//generate_freeTriangleVerticesObjects(m_building_VBO, m_building_VAO, m_building_vertices);


	// RoofVertices VAO
	generate_freeTriangleVerticesObjects(m_roofSouth_VBO, m_roofSouth_VAO, m_roof_vertices.south_roof);

	generate_freeTriangleVerticesObjects(m_roofWest_VBO,  m_roofWest_VAO,  m_roof_vertices.west_roof);

	generate_freeTriangleVerticesObjects(m_roofNorth_VBO, m_roofNorth_VAO, m_roof_vertices.north_roof);

	generate_freeTriangleVerticesObjects(m_roofEast_VBO,  m_roofEast_VAO,  m_roof_vertices.east_roof);



	auto const mipmap_levels = compute_maxMipmapLevels(GSet::pptxu(), GSet::pptxu());
	#if GSET_TILESET_TEXARRAY
		//--- Create the tileset texture array
		glGenTextures(1, &m_tilesetTexarray_id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_tilesetTexarray_id);
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipmap_levels, GL_RGBA8, m_tileset_texarray.width(), m_tileset_texarray.height(), m_tileset_texarray.layer_count());
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, m_tileset_texarray.width(), m_tileset_texarray.height(), m_tileset_texarray.layer_count(), 
							GL_RGBA, GL_UNSIGNED_BYTE, m_tileset_texarray.data());
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		m_tileset_texarray.free();
	#else
		//--- Create the tileset texture
		glGenTextures(1, &m_tilesetTexture_id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tilesetTexture_id);
			glTexStorage2D(GL_TEXTURE_2D, mipmap_levels, GL_RGBA8, m_tileset_texture.width(), m_tileset_texture.height());
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tileset_texture.width(), m_tileset_texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, m_tileset_texture.data());
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
		m_tileset_texture.free();
	#endif



	//--- Create the dynamic texture
	glGenTextures(1, &m_dynamicTexture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_dynamicTexture_id);
		glTexStorage2D(GL_TEXTURE_2D, mipmap_levels, GL_RGBA8, m_dynamic_texture.width(), m_dynamic_texture.height());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_dynamic_texture.width(), m_dynamic_texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, m_dynamic_texture.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	m_dynamic_texture.free();



	// CREATE THE ROOF TEXTURE 
	glGenTextures(1, &m_roofTexture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_roofTexture_id);
		glTexStorage2D(GL_TEXTURE_2D, mipmap_levels, GL_RGBA8, m_roof_texture.width(), m_roof_texture.height());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_roof_texture.width(), m_roof_texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, m_roof_texture.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// The roof texture use mipmaps, however the LODs between 0.f and 4.3f are forced to be 0.f. So the mipmap comes into play only when the 
		// distance from the camera is really long. This is because sampling the base texture produces a better visual output compared to the generated mipmap.
		// The special LOD level behavior for the roof texture is determined in the fragment shader.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST /*GL_LINEAR*/ /*GL_NEAREST*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR /*GL_NEAREST*/);								
	m_roof_texture.free();



	// BIND THE TEXTURES
	
	// bind the tileset texarray
	glActiveTexture(GL_TEXTURE0 + tilesetTexarray_unit);
	#if GSET_TILESET_TEXARRAY
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_tilesetTexarray_id);
	#else
		glBindTexture(GL_TEXTURE_2D, m_tilesetTexture_id);		
	#endif

	// bind the main texture
	glActiveTexture(GL_TEXTURE0 + dynamicTexture_unit);
	glBindTexture(GL_TEXTURE_2D, m_dynamicTexture_id);

	// bind the roof texture
	glActiveTexture(GL_TEXTURE0 + roofTexture_unit);
	glBindTexture(GL_TEXTURE_2D, m_roofTexture_id);




	// INITIALIZE UNIFORMS
	m_tile_main_shader.set_fragDataLocation(0, "fs_textured_frag_color");
	m_tile_main_shader.set_fragDataLocation(1, "fs_edgeable_id");
	m_tile_main_shader.set_vec2("u_viewport", Vector2f{ m_defaultFbo_size });

	m_dynamic_main_shader.set_fragDataLocation(0, "fs_textured_frag_color");
	m_dynamic_main_shader.set_fragDataLocation(1, "fs_edgeable_id");
	m_dynamic_main_shader.set_vec2("u_viewport", Vector2f{ m_defaultFbo_size });

	m_edgeableIds_main_shader.set_fragDataLocation(0, "fs_textured_frag_color");
	m_edgeableIds_main_shader.set_fragDataLocation(1, "fs_edgeable_id");
	m_edgeableIds_main_shader.set_vec2("u_viewport", Vector2f{ m_defaultFbo_size });

}

void GraphicsManager::generate_tileObjects()
{
	glGenVertexArrays(1, &m_tile_VAO);
	glGenBuffers(1, &m_tile_VBO);

	glBindVertexArray(m_tile_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_tile_VBO);
			glBufferData(GL_ARRAY_BUFFER, m_tile_vertices.buffer_byteSize(), m_tile_vertices.get_ptr(), GL_STATIC_DRAW);

			static char const tile_VAO_label[] = "tile_VAO";
			glObjectLabel(GL_VERTEX_ARRAY, m_tile_VAO, sizeof(tile_VAO_label), tile_VAO_label);
			static char const tile_VBO_label[] = "tile_VBO";
			glObjectLabel(GL_BUFFER, m_tile_VBO, sizeof(tile_VBO_label), tile_VBO_label);

			// world position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TilesetVertexData), nullptr);
			glEnableVertexAttribArray(0);
			// texture coord attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TilesetVertexData), (void*)(offsetof(TilesetVertexData, tex_coords)));
			glEnableVertexAttribArray(1);
			// layer attribute
			glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(TilesetVertexData), (void*)(offsetof(TilesetVertexData, layer)));
			glEnableVertexAttribArray(2);
			// entity_id attribute
			glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(TilesetVertexData), (void*)(offsetof(TilesetVertexData, entity_id)));
			glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0); 
}

#if EDGE_DETECTION_FILTER
	void GraphicsManager::generate_edgeDetectionFilterObjects()
	{
		#if ALPHA_TO_COVERAGE
			//--- Generate multisampled edge-detection scene FBO
			glGenFramebuffers(1, &m_edfScene_msFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, m_edfScene_msFBO);
				glGenRenderbuffers(1, &m_edfScene_msTexturedColorRBO);
				glBindRenderbuffer(GL_RENDERBUFFER, m_edfScene_msTexturedColorRBO);
					glRenderbufferStorageMultisample(GL_RENDERBUFFER, GSet::samples, GL_RGBA8, m_defaultFbo_size.x, m_defaultFbo_size.y);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_edfScene_msTexturedColorRBO);

				glGenTextures(1, &m_edfScene_msEdgeableIdsTex);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_edfScene_msEdgeableIdsTex);
					glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, GSet::samples, GL_R32UI, m_defaultFbo_size.x, m_defaultFbo_size.y, GL_TRUE);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, m_edfScene_msEdgeableIdsTex, 0);

				static const GLenum edfScene_msFBOdrawbuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
				glDrawBuffers(2, edfScene_msFBOdrawbuffers);
				
				glGenRenderbuffers(1, &m_edfScene_msDepthStencilRBO);
				glBindRenderbuffer(GL_RENDERBUFFER, m_edfScene_msDepthStencilRBO);
					glRenderbufferStorageMultisample(GL_RENDERBUFFER, GSet::samples, GL_DEPTH24_STENCIL8, m_defaultFbo_size.x, m_defaultFbo_size.y);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_edfScene_msDepthStencilRBO);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { throw std::runtime_error("The multi-sampled edfScene FBO isn't complete."); }

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		#endif



		// GENERATE EDGE-DETECTION FRAMEBUFFER 
		// it generates two color outputs: 1) Textured scene; 2) EdgeableIds scene (used as the starting point for edge-detection filter).
		glGenFramebuffers(1, &m_edfScene_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_edfScene_FBO);

			// Generate the texture that will store the color output of the textured scene
			glGenTextures(1, &m_edfScene_texturedColorTex);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_edfScene_texturedColorTex);
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, m_defaultFbo_size.x, m_defaultFbo_size.y);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, 0); //unbind
		
			// Attach the color texture to the framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_edfScene_texturedColorTex, 0);


			// Generate the uint texture that will store in each texel the edgeable_id's of the polygon 
			// (then, along the boundaries between different edgeable_ids, the edges will be drawn)
			glGenTextures(1, &m_edfScene_edgeableIdsTex);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_edfScene_edgeableIdsTex);
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, m_defaultFbo_size.x, m_defaultFbo_size.y);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, 0); //unbind
		
			// Attach the color texture to the framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_edfScene_edgeableIdsTex, 0);


			// Specify that there are two output color buffers
			static const GLenum drawbuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			glDrawBuffers(2, drawbuffers);


			// Generate a renderbuffer to allow depth test (stencil test isn't necessary)
			glGenRenderbuffers(1, &m_edfScene_depthStencilRBO);
			glBindRenderbuffer(GL_RENDERBUFFER, m_edfScene_depthStencilRBO);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_defaultFbo_size.x, m_defaultFbo_size.y); 
			glBindRenderbuffer(GL_RENDERBUFFER, 0); //unbind

			// Attach the depth&stencil renderbuffer to the framebuffer
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_edfScene_depthStencilRBO);


			// Check if the framebuffer is complete
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				throw std::runtime_error("The edfScene FBO is not complete.");


		glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind		

		


		// GENERATE EDGES FRAMEBUFFER (used to draw the only-edge scene).
		glGenFramebuffers(1, &m_edfEdges_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_edfEdges_FBO);

			// Generate the texture that will store the color output of overdraw_FBO
			glGenTextures(1, &m_edfEdges_colorTex);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_edfEdges_colorTex);
				//the texture has to be large enought to store whatever window size (up to the size of the desktop)
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, m_defaultFbo_size.x, m_defaultFbo_size.y);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, 0); //unbind
		
			// Attach the color texture to the framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_edfEdges_colorTex, 0);


			// Check if the framebuffer is complete
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				throw std::runtime_error("The edges framebuffer is not complete.");

		glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind
		


		// GENERATE THICKENED EDGES FRAMEBUFFER (used to draw the thickened edges of the only-edge scene).
		glGenFramebuffers(1, &m_edfThickenedEdges_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_edfThickenedEdges_FBO);

			// Generate the texture that will store the color output of overdraw_FBO
			glGenTextures(1, &m_edfThickenedEdges_colorTex);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_edfThickenedEdges_colorTex);
				//the texture has to be large enought to store whatever window size (up to the size of the desktop)
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, m_defaultFbo_size.x, m_defaultFbo_size.y);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, 0); //unbind
		
			// Attach the color texture to the framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_edfThickenedEdges_colorTex, 0);


			// Check if the framebuffer is complete
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				throw std::runtime_error("The thickened-edges framebuffer is not complete.");
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind



		
		#if EDGE_DETECTION_FILTER_INTERMEDIATE_STEPS
			m_textured_viewer.open("Textured scene", m_edfScene_texturedColorTex, m_defaultFbo_size.x, m_defaultFbo_size.y, "");
			m_edgeableIds_viewer.open("EdgeableIds scene", m_edfScene_edgeableIdsTex, m_defaultFbo_size.x, m_defaultFbo_size.y, "edgeable_ids");
			m_edges_viewer.open("Thickened edge scene", m_edfThickenedEdges_colorTex, m_defaultFbo_size.x, m_defaultFbo_size.y, "");
		#endif
	}
#endif

#if OVERDRAW_FILTER

	void GraphicsManager::generate_overdrawObjects()
	{
		// Generate the overdraw framebuffer
		glGenFramebuffers(1, &m_overdraw_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_overdraw_FBO);

			// Generate a useless texture that will store the color output of overdraw_FBO
			glGenRenderbuffers(1, &m_overdraw_colorRBO);
			glBindRenderbuffer(GL_RENDERBUFFER, m_overdraw_colorRBO);
				//the RBO is large enought to store whatever window size (up to the size of the desktop)
				glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, m_defaultFbo_size.x, m_defaultFbo_size.y);
			glBindRenderbuffer(GL_RENDERBUFFER, 0); //unbind
		
			// Attach the color texture to the framebuffer
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_overdraw_colorRBO);



			// Generate a texture to depth and stencil buffer (also the depth buffer is needed because otherwise depth test couldn't be done)
			glGenTextures(1, &m_overdraw_depthStencilTex);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_overdraw_depthStencilTex);
				//the texture is large enough to store whatever window size (up to the size of the desktop)
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_defaultFbo_size.x, m_defaultFbo_size.y); 
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_INDEX);
			glBindTexture(GL_TEXTURE_2D, 0); //unbind
		
			// Attach the depth and stencil texture to the framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_overdraw_depthStencilTex, 0);


			// Check if the framebuffer is complete
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				throw std::runtime_error("The overdraw framebuffer is not complete.");


		glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind

	}

#endif


#if OCCLUSION_CULLING
	void GraphicsManager::generate_occlusionCullingObjects()
	{
		// VISIBLE ENTITIES FRAMEBUFFER



		// Compute dimensions that are good for culling (they must not be too big, not bigger than the resolution, since otherwise there would be a framerate drop)

		//The height of the screen is larger than the length of the map due to floor slipping. The slipping is only considered for "map_height - 1" floors, since
		//the first floor has already been considered in the map length. Furthermore the value is rounded to the ceiling integer.
		auto const floorSlipping_inTiles = static_cast<int>(std::ceilf((m_tile_vertices.map_height() - 1) * GSet::wySliding_ratio()));

		//Compute a good tile dimension so that the whole map can fill up the screen resolution
		//TODO: PERFORMANCE: Maybe when the resolution of the screen is really high (ultra hd), I could create an FBO smaller than screen dimensions. 
		//					 I could set a max cap to improve performance.
		auto const entityIdsFBO_ppt = GSet::video_mode.height() / (m_tile_vertices.map_length() + floorSlipping_inTiles);
		std::cout << "entityIdsFBO_ppt: " << entityIdsFBO_ppt << std::endl;

		m_entityIdsFBO_width  = static_cast<GLsizei>(m_tile_vertices.map_width() * entityIdsFBO_ppt);
		m_entityIdsFBO_height = static_cast<GLsizei>((m_tile_vertices.map_length() + floorSlipping_inTiles) * entityIdsFBO_ppt);
		
		glGenFramebuffers(1, &m_entityIds_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_entityIds_FBO);




		// Generate an unsigned_integer-based texture that will store the ids of the visible entities
		glGenTextures(1, &m_entityIds_tex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_entityIds_tex);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, m_entityIdsFBO_width, m_entityIdsFBO_height);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0); //unbind
		
		// Attach the color texture to the framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_entityIds_tex, 0);



		// Generate a renderbuffer to allow depth test (stencil test isn't necessary)
		glGenRenderbuffers(1, &m_entityIds_depthStencilRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_entityIds_depthStencilRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_entityIdsFBO_width, m_entityIdsFBO_height); 
		glBindRenderbuffer(GL_RENDERBUFFER, 0); //unbind
		
		// Attach the depth and stencil texture to the framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_entityIds_depthStencilRBO);



		// Check if the framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw std::runtime_error("The visible entities framebuffer is not complete.");


		glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind


		#if SHOW_VISIBLE_ENTITIES_FBO
			// Initialize (or reinitilize) the FBO viewer
			if (m_entityIds_viewer.is_open()) {	m_entityIds_viewer.close();	}
			m_entityIds_viewer.open("EntityIds scene", m_entityIds_tex, m_entityIdsFBO_width, m_entityIdsFBO_height, "entity_ids");
		#endif




		// VISIBLE BUFFER BUILDER FBO

		glGenFramebuffers(1, &m_visibleBufferBuilder_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_visibleBufferBuilder_FBO);

		// Generate an unused color renderbuffer just to make the framebuffer complete
		glGenRenderbuffers(1, &m_visibleBufferBuilder_colorRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_visibleBufferBuilder_colorRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_R8I, m_entityIdsFBO_width, m_entityIdsFBO_height); 
		glBindRenderbuffer(GL_RENDERBUFFER, 0); //unbind
		
		// Attach unused color renderbuffer to the framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_visibleBufferBuilder_colorRBO);

		// Check if the framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw std::runtime_error("The visible buffer builder FBO is not complete.");

		glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind

		
		// Bind the original tile buffer as an SSBO
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, tileBufferSSBO_unit, m_tile_VBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // glBindBufferBase() has as a side effect to bind the buffer also to the general binding point. I don't need it.


		// EVALUATED ENTITIES SSBO (needed by visible_buffer_builder shader to mark already copied entities)
		glGenBuffers(1, &m_evaluatedEntities_SSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_evaluatedEntities_SSBO);
			// Divided by 32 because each flag (one per tile) is stored in 1 bit and GLuint contains 32 bits.
			auto const evaluatedEntitiesSSBO_maxSize = sizeof(GLuint) * (m_tile_vertices.entityId_maxValue() / 32 + 32); 
			auto BEG = reinterpret_cast<const unsigned char*>(&m_tile_vertices.get_ptr()[99]);
			auto END = reinterpret_cast<const unsigned char*>(&m_tile_vertices.get_ptr()[100]);
			auto TEST_STRIDE = END - BEG;
			std::cout << "TILESETVERTEXDATA sizeof: " << sizeof(TilesetVertexData) << std::endl;
			std::cout << "TILEVERTICES stride: " << TEST_STRIDE << std::endl;
			glBufferData(GL_SHADER_STORAGE_BUFFER, evaluatedEntitiesSSBO_maxSize, nullptr, GL_DYNAMIC_DRAW);
			glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr); //nullptr ask to clear all values to zero
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, evaluatedEntitiesSSBO_unit, m_evaluatedEntities_SSBO);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

		


		// VISIBLE TILE INDIRECT COMMAND BUFFER
		glGenBuffers(1, &m_visibleTile_indirectCommandBuffer);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_visibleTile_indirectCommandBuffer);
			//TODO: Add a label to each OpenGL object
			static char const visibleTile_indirectCommandBuffer_label[] = "visibleTile_indirectCommandBuffer";
			glObjectLabel(GL_BUFFER, m_visibleTile_indirectCommandBuffer, sizeof(visibleTile_indirectCommandBuffer_label), visibleTile_indirectCommandBuffer_label);
			
			static DrawArraysIndirectCommand visibleTileIndirectCommandBuffer_startingValue{ 0, 1, 0, 0 };
			glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand), &visibleTileIndirectCommandBuffer_startingValue, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0); //unbind


		//--- Visible vertex atomic counter.
		glGenBuffers(1, &m_visibleVertex_ACB);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_visibleVertex_ACB);
			glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &m_visibleVertexACB_clearValue, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, visibleVertexACB_unit, m_visibleVertex_ACB);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);



		//TODO: PERFORMANCE: Using OpenGL 4.6 I could get rid of the following atomic counter, I could use only m_visibleVertex_ACB in conjuntion with atomicCounterAdd()
		//					 in the shader. But in order to make the game compatible with the integrated GPU, I have to use a prior version of OpenGL, the 4.3, so that 
		//					 function isn't available. In the future it's better to switch to that single-atomic-counter design getting rid of the following counter.
		//--- Visible tile atomic counter
		glGenBuffers(1, &m_visibleTile_ACB);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_visibleTile_ACB);
			glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &m_visibleTileACB_clearValue, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, visibleTileACB_unit, m_visibleTile_ACB);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0); //unbind




		// VISIBLE TILE VAO
		glGenVertexArrays(1, &m_visibleTile_VAO);
		glGenBuffers(1, &m_visibleTile_buffer);

		glBindVertexArray(m_visibleTile_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_visibleTile_buffer);
				//TODO: PERFORMANCE: This maxSize could be shrinked, since it's quite impossible that all the entities are simultaneously visible on the screen. The problem is that
				//					 if in the fragment shader this max size is overstepped, there is no way to generate an error. So for now I'll keep the highest possible size.
				auto const visibleTileBuffer_maxSize = m_tile_vertices.buffer_byteSize();
				glBufferData(GL_ARRAY_BUFFER, visibleTileBuffer_maxSize, nullptr, GL_DYNAMIC_DRAW);
				glClearBufferData(GL_ARRAY_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr);
	
				// world position attribute
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TilesetVertexData), nullptr);
				glEnableVertexAttribArray(0);
				// texture coord attribute
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TilesetVertexData), (void*)(offsetof(TilesetVertexData, tex_coords)));
				glEnableVertexAttribArray(1);
				// layer attribute
				glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(TilesetVertexData), (void*)(offsetof(TilesetVertexData, layer)));
				glEnableVertexAttribArray(2);
				// entity_id attribute
				glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(TilesetVertexData), (void*)(offsetof(TilesetVertexData, entity_id)));
				glEnableVertexAttribArray(3);
				
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, visibleTileBufferSSBO_unit, m_visibleTile_buffer);

			glBindBuffer(GL_ARRAY_BUFFER, 0); 
		glBindVertexArray(0);



		// Bind the entity ids texture
		glActiveTexture(GL_TEXTURE0 + entityIdsTexture_unit);
		glBindTexture(GL_TEXTURE_2D, m_entityIds_tex);
	}

#endif

void GraphicsManager::generate_freeTriangleVerticesObjects(GLuint & VBO_id, GLuint & VAO_id, FreeTriangleVertices const& ftv)
{
	glGenVertexArrays(1, &VAO_id);
	glGenBuffers(1, &VBO_id);

	glBindVertexArray(VAO_id);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
	glBufferData(GL_ARRAY_BUFFER, ftv.buffer_byteSize(), ftv.get_ptr(), GL_DYNAMIC_DRAW);


	// world position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(EdgeableVertexData), nullptr);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(EdgeableVertexData), (void*)(offsetof(EdgeableVertexData, tex_coords)));
	glEnableVertexAttribArray(1);
	// entity_id attribute
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(EdgeableVertexData), (void*)(offsetof(EdgeableVertexData, entity_id)));
	glEnableVertexAttribArray(2);
	// edgeable_id attribute
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(EdgeableVertexData), (void*)(offsetof(EdgeableVertexData, edgeable_id)));
	glEnableVertexAttribArray(3);


	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);
}

void GraphicsManager::prepare_windowQuadVAO()
{
	m_windowQuad_vertices = { 

		//top-left triangle
		-1.f, -1.f,		0.f, 0.f,
		-1.f,  1.f,		0.f, 1.f,
		 1.f,  1.f,		1.f, 1.f,

		//bottom-right triangle
		-1.f, -1.f,		0.f, 0.f,
		 1.f,  1.f,		1.f, 1.f,
		 1.f, -1.f, 	1.f, 0.f
	};

	glGenVertexArrays(1, &m_windowQuad_VAO);
	glGenBuffers(1, &m_windowQuad_VBO);
		
	glBindVertexArray(m_windowQuad_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_windowQuad_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_windowQuad_vertices.size(), m_windowQuad_vertices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind VBO
	glBindVertexArray(0); //unbind VAO
}

void GraphicsManager::resize_fbo(Vector2i const new_fbo_size)
{
	m_defaultFbo_size = new_fbo_size;
	std::cout << "Default FBO resized to " << m_defaultFbo_size.x << "*" << m_defaultFbo_size.y << std::endl;

	if (is_defaulFbo_null()) { return; }

    glViewport(0, 0, new_fbo_size.x, new_fbo_size.y);

	m_tile_main_shader.set_vec2("u_viewport", Vector2f{ new_fbo_size });
	m_dynamic_main_shader.set_vec2("u_viewport", Vector2f{ new_fbo_size });
	m_edgeableIds_main_shader.set_vec2("u_viewport", Vector2f{ new_fbo_size });



	#if EDGE_DETECTION_FILTER
		//Recreate the framebuffer and its textures from scratch in order to fit the new window size.
		free_edgeDetectionFilterObjects();
		generate_edgeDetectionFilterObjects();
	#endif

	#if OVERDRAW_FILTER
		//Recreate the framebuffer and its textures from scratch in order fitting the new window size.
		free_overdrawObjects();
		generate_overdrawObjects();
	#endif
}

void GraphicsManager::resize_window(Vector2i const new_window_size) noexcept
{
	m_window_size = new_window_size;
}


void GraphicsManager::draw()
{
	if (is_defaulFbo_null())
		return;

	//--- Regenerate tile objects and occlusion culling objects
	if (m_tile_vertices.has_been_reset())
	{
		free_tileObjects();
		generate_tileObjects();

		#if OCCLUSION_CULLING
			free_occlusionCullingObjects();
			generate_occlusionCullingObjects();
		#endif

		m_tile_vertices.reset_acquired();
	}
	
	//--- Rebuff tile vertices and recompute visible entities
	if (m_tile_vertices.has_changed())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_tile_VBO);

		for (auto chunk : m_tile_vertices.get_changes())
		{
			glBufferSubData(GL_ARRAY_BUFFER, chunk.first, m_tile_vertices.chunk_byteSize(), (void const*) chunk.second);
		}
		m_tile_vertices.changes_acquired();

		glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind
		
		#if OCCLUSION_CULLING
			occlusion_culling();
		#endif
	}

	if(m_camera.are_noRoofRects_changed())
	{
		#if OCCLUSION_CULLING
			occlusion_culling();
		#endif
	}


	#if EDGE_DETECTION_FILTER
		drawScene_with_edgeDetectionFilter();
	#elif OVERDRAW_FILTER
		drawScene_with_overdrawFilter();
	#else
		drawScene_without_filters();
	#endif
}

#if OCCLUSION_CULLING
	void GraphicsManager::occlusion_culling()
	{
		// DRAW VISIBLE ENTITIES TEXTURE

		// Bind entity ids framebuffer and set its context
		glBindFramebuffer(GL_FRAMEBUFFER, m_entityIds_FBO);
		glViewport(0, 0, m_entityIdsFBO_width, m_entityIdsFBO_height);
		glEnable(GL_DEPTH_TEST);

		static GLuint const clear_color[] = {0u, 0u, 0u, 0u};
		glClearBufferuiv(GL_COLOR, 0, clear_color); //I can't use glClear(GL_COLOR_BUFFER_BIT) to clear a buffer of unsigned int 
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


		// Set the uniforms that change every time

		m_entityIds_shader.bind();

		//vertex shader uniforms
		auto const occlusion_view = compute_occlusionViewMatrix();
		auto const occlusion_projection = compute_occlusionProjMatrix();
		m_entityIds_shader.set_mat4("u_view", occlusion_view);	
		m_entityIds_shader.set_mat4("u_projection", occlusion_projection);
		
		//fragment shader uniforms
		m_entityIds_shader.set_int("u_texture", tilesetTexarray_unit); //"1" means texture bound at the unit "GL_TEXTURE1"
		m_entityIds_shader.set_vec3("u_light_position", m_light_pos);
		m_entityIds_shader.set_vec3("u_frag_normal", m_light_pos); //normal to tiles and sprites surface
		m_entityIds_shader.set_float("u_cameraTarget_zWorldPos", floor_highestZ(m_camera.target().z));
		m_entityIds_shader.set_uint("u_noRoofRects_size", m_camera.noRoofRects_size());
		m_entityIds_shader.set_vec2array("u_noRoofRects", m_camera.noRoofRects_ptr(), Camera::max_noRoofRects * 2);
		m_entityIds_shader.set_uint("u_noRoofIntersections_size", m_camera.noRoofIntersections_size());
		m_entityIds_shader.set_vec2array("u_noRoofIntersections", m_camera.noRoofIntersections_ptr(), Camera::max_noRoofIntersections * 2);


		// Draw all the tiles of the game map
		glBindVertexArray(m_tile_VAO);	
			glDrawArrays(GL_TRIANGLES, 0, m_tile_vertices.vertices_count());
		glBindVertexArray(0);


		//debug_printUnsignedIntBuffer(0, 0, m_entityIdsFBO_width, m_entityIdsFBO_height);
		



		// BUILD THE VISIBLE BUFFER FROM VISIBLE ENTITIES TEXTURE

		// Bind visible buffer builder FBO
		glBindFramebuffer(GL_FRAMEBUFFER, m_visibleBufferBuilder_FBO);
			glDisable(GL_DEPTH_TEST);

			// Clear evaluated entities SSBO
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_evaluatedEntities_SSBO);
			glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr); //nullptr ask to clear all values to zero
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			// Clear the atomic counter of the visible vertex
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_visibleVertex_ACB);
				glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &m_visibleVertexACB_clearValue);
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
		
			// Clear the atomic counter of the visible tiles
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_visibleTile_ACB);
				glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &m_visibleTileACB_clearValue);
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

			// Clear visible tiles buffer
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_visibleTile_buffer);
				//GL_R32UI doesn't match the actual data type stored by the buffer, but this method work well and it's portable, since OpenGL 
				//use IEEE-754 floating point and 4 zeroed bytes correspond to 0.f.
				glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr); //nullptr ask to clear all values to zero
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		
			// Bind the visible buffer builder shader
			m_visibleBufferBuilder_shader.bind();

			// Fake draw. Actually this is a trick to make the fragment shader build the visibleTile_buffer starting from the visibleEntities_tex
			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, visibleVertexACB_unit, m_visibleVertex_ACB); // I need to rebind these atomic counters because of an Intel driver bug.
			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, visibleTileACB_unit, m_visibleTile_ACB);
			glBindVertexArray(m_windowQuad_VAO);
				glDrawArrays(GL_TRIANGLES, 0, static_cast<GLuint>(m_windowQuad_vertices.size()) / 4);
			glBindVertexArray(0);//unbind

			// Copy visible vertex count into the indirect command buffer (I can't directly bind the indirect command buffer as an atomic counter, because
			// the Intel driver doesn't immediately synchronize between the output of the visibleBufferBuilder shader and the content of the indirect command buffer,
			// so in the frame immediately after the occlusion culling the number of vertex would be zero and no tile would be drawn).
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_visibleVertex_ACB);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_visibleTile_indirectCommandBuffer);
				glCopyBufferSubData(GL_ATOMIC_COUNTER_BUFFER, GL_DRAW_INDIRECT_BUFFER, 0, 0, sizeof(GLuint));
			glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

			
		//debug_printSSBO(m_evaluatedEntities_SSBO, 0, m_evaluatedEntitiesSSBO_maxSize);
		//debug_printTilesetVertexDataBuffer(m_tile_VBO, 0, 100);
		//debug_inspectTilesetVertexDataBuffer(m_visibleTile_buffer, m_tile_vertices.vertices_count());
		//debug_printTilesetVertexDataBuffer(m_visibleTile_buffer, 0, 100);
		//GraphicsManagerCore::debug_printDrawArraysIndirectCommandBuffer(m_visibleTile_indirectCommandBuffer);


		// RESTORE DEFAULT FRAMEBUFFER AND SET ITS CONTEXT
		glBindFramebuffer(GL_FRAMEBUFFER, default_FBO);
			glViewport(0, 0, m_defaultFbo_size.x, m_defaultFbo_size.y);
			glEnable(GL_DEPTH_TEST);
	
		
		#if SHOW_VISIBLE_ENTITIES_FBO
			// Update the entityIds_tex viewer
			m_entityIds_viewer.update();
		#endif
	}
#endif


void GraphicsManager::drawScene_without_filters()
{
	static auto test_counter = 0;
	//std::cout << "drawScene_without_filters: #" << test_counter << std::endl;
	++test_counter;

	// Bind the default framebuffer (it can be either the real-screen framebuffer or the internal-screen framebuffer).
	glBindFramebuffer(GL_FRAMEBUFFER, default_FBO);

	// Specify that the second color buffer (the edgeable_id) doesn't have to be written in any buffer
	static const GLenum drawbuffers[2] = { GL_BACK_LEFT, GL_NONE };
	glDrawBuffers(2, drawbuffers);

	glEnable(GL_DEPTH_TEST);

	#if POLYGON_MODE
		glClearColor(1.f, 1.f, 1.f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	#else
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	draw_scene();



	#if POLYGON_MODE
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	#endif
}


#if EDGE_DETECTION_FILTER
	void GraphicsManager::drawScene_with_edgeDetectionFilter()
	{
		#if ALPHA_TO_COVERAGE
			// Bind a multisampled framebuffer where the scene must be rendered (it has 2 color outputs, one for the textured scene and one for the edgeableIds scene).
			glBindFramebuffer(GL_FRAMEBUFFER, m_edfScene_msFBO);
				glEnable(GL_DEPTH_TEST);
				
				static float const texturedScene_clearColor[] = { 0.2f, 0.3f, 0.3f, 1.0f };
				glClearBufferfv(GL_COLOR, 0, texturedScene_clearColor);
				static GLuint const edgeableIdsTex_clearColor[] = {0u, 0u, 0u, 0u};
				glClearBufferuiv(GL_COLOR, 1, edgeableIdsTex_clearColor); //I can't use glClear(GL_COLOR_BUFFER_BIT) to clear a buffer of unsigned int 
				glClear(GL_DEPTH_BUFFER_BIT);

				draw_scene();	

			// Blit every attachment to the non-multisampled edfScene FBO.
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_edfScene_msFBO);
				glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_edfScene_FBO);
				static GLenum const edfEdgesFBOSolvedTextureScene_drawBuffer = GL_COLOR_ATTACHMENT0;
				glDrawBuffers(1, &edfEdgesFBOSolvedTextureScene_drawBuffer);
			glBlitFramebuffer(0, 0, m_defaultFbo_size.x, m_defaultFbo_size.y, 0, 0, m_defaultFbo_size.x, m_defaultFbo_size.y,
							 GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_edfScene_msFBO);
				glReadBuffer(GL_COLOR_ATTACHMENT1);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_edfScene_FBO);
				static GLenum const edfEdgesFBOSolvedEdgeableIdsScene_drawBuffer = GL_COLOR_ATTACHMENT1;
				glDrawBuffers(1, &edfEdgesFBOSolvedEdgeableIdsScene_drawBuffer);
			glBlitFramebuffer(0, 0, m_defaultFbo_size.x, m_defaultFbo_size.y, 0, 0, m_defaultFbo_size.x, m_defaultFbo_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		#else
			// Bind a custom framebuffer where the scene must be rendered (it has 2 color outputs, one for the textured scene and one for the edgeableIds scene)
			glBindFramebuffer(GL_FRAMEBUFFER, m_edfScene_FBO);
				glEnable(GL_DEPTH_TEST);
		
				// Clear buffers
				//set different clear colors for the textured scene texture and the uint texture with the edgeable_ids
				static float const texturedScene_clearColor[] = { 0.2f, 0.3f, 0.3f, 1.0f };
				glClearBufferfv(GL_COLOR, 0, texturedScene_clearColor);
				static GLuint const edgeableIdsTex_clearColor[] = {0u, 0u, 0u, 0u};
				glClearBufferuiv(GL_COLOR, 1, edgeableIdsTex_clearColor); //I can't use glClear(GL_COLOR_BUFFER_BIT) to clear a buffer of unsigned int 

				glClear(GL_DEPTH_BUFFER_BIT);


				draw_scene();		
		#endif	




		// Bind a custom framebuffer to store the edgeableIds scene
		glBindFramebuffer(GL_FRAMEBUFFER, m_edfEdges_FBO);
			glDisable(GL_DEPTH_TEST); //unuseful for a flat quad
			glClearColor(1.f, 1.f, 1.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Bind the texture storing the edgeable ids
			glActiveTexture(GL_TEXTURE0 + edgeDetectorEdgeableIdsTex_unit);
			glBindTexture(GL_TEXTURE_2D, m_edfScene_edgeableIdsTex);
		
			// Bind a shader specifically created to detect boundaries between different edgeable_id areas.
			m_edgeDetector_shader.bind();
		
			// Draw the color attachment on the screen
			glBindVertexArray(m_windowQuad_VAO);
				glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_windowQuad_vertices.size() / 4));
			glBindVertexArray(0);//unbind



		
		// Bind a custom framebuffer to store the edges-only scene
		glBindFramebuffer(GL_FRAMEBUFFER, m_edfThickenedEdges_FBO);
			glDisable(GL_DEPTH_TEST); //unuseful for a flat quad
			glClearColor(1.f, 1.f, 1.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);
		
			// Bind the texture storing the raw edges
			glActiveTexture(GL_TEXTURE0 + edgeThickenerEdgesTex_unit);
			glBindTexture(GL_TEXTURE_2D, m_edfEdges_colorTex);
		
			// Bind a shader specifically created to apply the sobel effect
			m_edgeThickener_shader.bind();

		
			auto edge_thickness = static_cast<int>(std::max<float>(0.f, (m_camera.zoom_level() + GSet::edgeThickness_factor()) / m_camera.zoom_level())); //TODO: Find a better function to compute the edge thickness
			m_edgeThickener_shader.set_int("u_edge_thickness", edge_thickness);
		
			// Draw the color attachment on the screen
			glBindVertexArray(m_windowQuad_VAO);
				glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_windowQuad_vertices.size() / 4));
			glBindVertexArray(0);//unbind



		//TODO: Add another intermediate step to apply an antialias to the m_edfThickenedEdges_colorTex
		//		beacause at low screen resolutions the edges appear pixelated.


		
		#if EDGE_DETECTION_FILTER_INTERMEDIATE_STEPS
			m_textured_viewer.update();
			m_edgeableIds_viewer.update();
			m_edges_viewer.update();
		#endif
		



		// Bind the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, default_FBO);
		glDisable(GL_DEPTH_TEST); //unuseful for a flat quad
		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Bind the texture storing the textured scene
		glActiveTexture(GL_TEXTURE0 + edgeTextureMixerTexturedColorTex_unit);
		glBindTexture(GL_TEXTURE_2D, m_edfScene_texturedColorTex);
		
		// Bind the texture storing the thickened edges.
		glActiveTexture(GL_TEXTURE0 + edgeTextureMixerThickenedEdgesTex_unit);
		glBindTexture(GL_TEXTURE_2D, m_edfThickenedEdges_colorTex);


		// Bind a shader specifically created to mix the colored texture with the edges.
		m_edgeTextureMixer_shader.bind();

		// Draw the color attachment on the screen
		glBindVertexArray(m_windowQuad_VAO);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_windowQuad_vertices.size() / 4));
		glBindVertexArray(0);//unbind
	}
#endif


#if OVERDRAW_FILTER
	void GraphicsManager::drawScene_with_overdrawFilter()
	{
		// Bind a custom framebuffer to store the output on a texture
		glBindFramebuffer(GL_FRAMEBUFFER, m_overdraw_FBO);

			glEnable(GL_DEPTH_TEST);
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

			// Enable the stencil test and make it register the number of draw for the each pixel
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_ALWAYS, 1, 0xFF); //stencil test always pass
			glStencilOp(GL_KEEP, GL_INCR, GL_INCR); //the value stored in the stencil buffer is increased every time a fragment is not discarded

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


			draw_scene();



		// Rebind the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, default_FBO);
			glDisable(GL_DEPTH_TEST); //unuseful for a flat rectangle
			glClearColor(1.f, 1.f, 1.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);


			//// Binding the depth&stencil texture only allow to fetch the depth buffer values (it's not related to overdraw_mode but it can be useful in future)
			//glActiveTexture(GL_TEXTURE15);
			//glBindTexture(GL_TEXTURE_2D, m_overdraw_depthStencilTex);

			// Bind the stencil view of the depth&stencil texture (needed to fetch uint values from the texture)
			glActiveTexture(GL_TEXTURE0 + overdrawDepthStencilTex_unit);
			glBindTexture(GL_TEXTURE_2D, m_overdraw_depthStencilTex);

			// Bind a shader specifically created to show the stencil buffer
			m_overdraw_screenShader.bind();

			// Draw the color attachment on the screen
			glBindVertexArray(m_windowQuad_VAO);
				glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_windowQuad_vertices.size()) / 4);
			glBindVertexArray(0);//unbind

			glBindTexture(GL_TEXTURE_2D, 0);
	}
#endif

void GraphicsManager::draw_scene()
{
	#if ALPHA_TO_COVERAGE
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);		// it requires also an active MSAA to work
	#endif

	// Transformations
	auto const map_box = compute_mapBox();

	auto const view = compute_viewMatrix(map_box);
	auto const projection = compute_projectionMatrix(map_box);

	bool are_noRoofRects_changed = false;
	if (m_camera.are_noRoofRects_changed())
	{
		are_noRoofRects_changed = true;
		m_camera.noRoofRects_flushed();
	}


	// Set the uniforms that change every frame
	set_sceneUniforms(m_tile_main_shader, view, projection, are_noRoofRects_changed);
	m_tile_main_shader.set_int("u_texture", tilesetTexarray_unit); //"1" means texture bound at the unit "GL_TEXTURE1"

	#if OCCLUSION_CULLING
		// Draw visible tile triangles
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_visibleTile_indirectCommandBuffer);
		glBindVertexArray(m_visibleTile_VAO);
			glDrawArraysIndirect(GL_TRIANGLES, nullptr); //nullptr because the indirect command is the buffer bound to GL_DRAW_INDIRECT_BUFFER
		glBindVertexArray(0); //unbind VAO
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0); //unbind command
	#else
		// Draw all the tiles of the game map
		glBindVertexArray(m_tile_VAO);	
		glDrawArrays(GL_TRIANGLES, 0, m_tile_vertices.vertices_count());
		glBindVertexArray(0);
	#endif

		
	set_sceneUniforms(m_dynamic_main_shader, view, projection, are_noRoofRects_changed);
	m_dynamic_main_shader.set_int("u_texture", dynamicTexture_unit); //"2" means texture at location of "GL_TEXTURE2"

	// Rebuff dynamic vertices
	glBindBuffer(GL_ARRAY_BUFFER, m_dynamic_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_dynamic_vertices.attribute_count(), m_dynamic_vertices.get_ptr(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	// Draw dynamic vertices
	glBindVertexArray(m_dynamic_VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	
	glDrawArrays(GL_TRIANGLES, 0, m_dynamic_vertices.vertices_count());

	glBindVertexArray(0); // no need to unbind it every time 



	// Rebuff and draw building vertices




	
	// Rebuff and draw roof vertices

	set_sceneUniforms(m_edgeableIds_main_shader, view, projection, are_noRoofRects_changed);
	m_edgeableIds_main_shader.set_int("u_texture", roofTexture_unit); //"3" means texture at location of "GL_TEXTURE3"


	if (m_roof_vertices.south_roof.has_changed())
	{
		rebuff_freeTriangleVerticesVBO(m_roofSouth_VBO, m_roof_vertices.south_roof);
		m_roof_vertices.south_roof.flushed();
	}
	if (m_roof_vertices.west_roof.has_changed())
	{
		rebuff_freeTriangleVerticesVBO(m_roofWest_VBO, m_roof_vertices.west_roof);
		m_roof_vertices.west_roof.flushed();
	}
	if (m_roof_vertices.north_roof.has_changed())
	{
		rebuff_freeTriangleVerticesVBO(m_roofNorth_VBO, m_roof_vertices.north_roof);
		m_roof_vertices.north_roof.flushed();
	}
	if (m_roof_vertices.east_roof.has_changed())
	{
		rebuff_freeTriangleVerticesVBO(m_roofEast_VBO, m_roof_vertices.east_roof);
		m_roof_vertices.east_roof.flushed();
	}

	m_edgeableIds_main_shader.set_vec3("u_frag_normal", { -1.f,  0.f,  1.f });
	draw_genericVAO(m_roofSouth_VAO, m_roof_vertices.south_roof);
	
	m_edgeableIds_main_shader.set_vec3("u_frag_normal", {  0.f,  1.f,  1.f });
	draw_genericVAO(m_roofWest_VAO, m_roof_vertices.west_roof);
	
	m_edgeableIds_main_shader.set_vec3("u_frag_normal", {  1.f,  0.f,  1.f });
	draw_genericVAO(m_roofNorth_VAO, m_roof_vertices.north_roof);
	
	m_edgeableIds_main_shader.set_vec3("u_frag_normal", {  0.f, -1.f,  1.f });
	draw_genericVAO(m_roofEast_VAO, m_roof_vertices.east_roof);


	#if ALPHA_TO_COVERAGE
		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	#endif
}

void GraphicsManager::set_sceneUniforms(Shader & shader, glm::mat4 const& view, glm::mat4 const& projection, bool const are_noRoofRects_changed) const
{
	shader.bind();
	//vertex shader uniforms
	shader.set_mat4("u_view", view);
	shader.set_mat4("u_projection", projection);

	//fragment shader uniforms
	shader.set_vec3("u_light_position", m_light_pos); //the normal vector to tiles and vertices
	shader.set_vec3("u_frag_normal", m_light_pos); //the normal vector to tiles and vertices
	shader.set_float("u_cameraTarget_zWorldPos", floor_highestZ(m_camera.target().z));
	if (are_noRoofRects_changed)
	{
		shader.set_uint("u_noRoofRects_size", m_camera.noRoofRects_size());
		shader.set_vec2array("u_noRoofRects", m_camera.noRoofRects_ptr(), Camera::max_noRoofRects * 2);
		shader.set_uint("u_noRoofIntersections_size", m_camera.noRoofIntersections_size());
		shader.set_vec2array("u_noRoofIntersections", m_camera.noRoofIntersections_ptr(), Camera::max_noRoofIntersections * 2);
	}
}

auto GraphicsManager::compute_mapBox() const -> WorldParallelepiped
{
	return { 
		0.f,
		GSet::tiles_to_units( - m_tile_vertices.map_length() ),			
		0.f, 
		GSet::tiles_to_units( m_tile_vertices.map_width() ),
		GSet::tiles_to_units( m_tile_vertices.map_length() + ( m_tile_vertices.map_height() - 1 ) * GSet::wySliding_ratio() ),
		GSet::tiles_to_units( m_tile_vertices.map_height() + GSet::roof_maxHeight) * GSet::floorsSpacing_ratio
	};
}

auto GraphicsManager::compute_occlusionViewMatrix() const -> glm::mat4
{
	glm::mat4 view(1.0f);
	
	auto map_box = compute_mapBox();

	// Place the camera in the middle of the map, just over the highest floor 
	glm::vec3 opengl_camera{ 
		map_box.center().x,
		map_box.center().y, 
		map_box.height 
	};


	// To simulate the camera moving, I translate the whole world in the opposite direction.
	view = glm::translate(view, - opengl_camera);	


	return view;
}

auto GraphicsManager::compute_occlusionProjMatrix() const -> glm::mat4
{
	glm::mat4 projection;

	auto map_box = compute_mapBox();

	auto near_plane = 0.1f;
	auto far_plane = map_box.height + 1.f;

	// glm::ortho requires as arguments the bounds of the visible rectangle around the origin (that the in the view space coincide with the camera).
	projection = glm::ortho( - map_box.length / 2.f , map_box.length / 2.f, 
							 - map_box.width  / 2.f , map_box.width  / 2.f, 
							 near_plane, far_plane );

	return projection;
}


auto GraphicsManager::compute_viewMatrix(WorldParallelepiped const& map_box) const -> glm::mat4
{
	glm::mat4 view(1.0f);
	
	// Conversion from Vector3f to glm::vec3
	glm::vec3 glm_cameraTarget{ m_camera.target().x, m_camera.target().y, m_camera.target().z };


	if (m_camera.projection() == Projection::Perspective)
	{
		// Distance of the camera from the target
		auto distance = m_camera.distance();

		glm::vec3 camera{ std::sin(glm::radians(m_camera.theta())) * std::cos(glm::radians(m_camera.phi()))   * distance, 
						  std::sin(glm::radians(m_camera.theta())) * std::sin(glm::radians(m_camera.phi()))   * distance, 
						  std::cos(glm::radians(m_camera.theta()))											  * distance };


		view = glm::lookAt( glm_cameraTarget + camera,
						    glm_cameraTarget,
						    glm::vec3(0.f, 0.f, 1.f) );
	}
	else
	{
		//TODO: Find a proper position for the camera when the target is below the highest floor

		// To simulate the camera moving, I translate the whole world in the opposite direction.
		view = glm::translate(view, glm::vec3(  - glm_cameraTarget.x, 
											    - glm_cameraTarget.y,
											    - map_box.height  ));
	}

	return view;
}

auto GraphicsManager::compute_projectionMatrix(WorldParallelepiped const& map_box) const -> glm::mat4
{
	glm::mat4 projection;

	if (m_camera.projection() == Projection::Perspective)
	{
		auto near_plane = 0.1f;
		auto far_plane = m_camera.distance() + map_box.diagonal_length(); //maximum theoretical distance of a triangle from the camera
		/*static Clock clock;
		if (clock.getElapsedTime().asSeconds() > 1.f)
		{
			std::cout << "far plane:" << far_plane << std::endl;
			clock.restart();
		}*/
		float window_ratio = m_defaultFbo_size.x * 1.f / m_defaultFbo_size.y * 1.f;

		projection = glm::perspective(glm::radians(45.f), window_ratio, near_plane, far_plane);
	}
	else
	{
		auto const near_plane = 0.1f;
		auto const far_plane = map_box.height + 1.f;
		

		Vector2f window_size{ m_defaultFbo_size.x * 1.f, m_defaultFbo_size.y * 1.f }; // in screen pixels

		// From screen pixels to units (non-zoomed).
		window_size = { GSet::texels_to_units(window_size.x), GSet::texels_to_units(window_size.y) };


		Vector2f zoomed_winSize = window_size * m_camera.zoom_level();


		// glm::ortho requires as arguments the bounds of the visible rectangle around the origin (that in the view space coincide with the camera).
		projection = glm::ortho( - zoomed_winSize.x / 2.0f , zoomed_winSize.x/2.0f, 
								 - zoomed_winSize.y / 2.0f , zoomed_winSize.y/2.0f, 
								 near_plane, far_plane );
	}

	return projection;
}

auto GraphicsManager::floor_highestZ(float const world_z) -> float
{
	return GSet::tiles_to_units(GSet::units_to_tiles(world_z / GSet::floorsSpacing_ratio) + 1) * GSet::floorsSpacing_ratio - GSet::upt / 100.f;
}

auto GraphicsManager::glfwWindowPixel_to_mapTile(Vector2f const glfw_cursorPos) const -> Vector3i
{
	auto map_units = glfwWindowPixel_to_mapUnits(glfw_cursorPos);

	return { GSet::units_to_tiles(map_units.x), 
			 GSet::units_to_tiles(map_units.y), 
			 GSet::units_to_tiles(map_units.z + 0.3f * GSet::upt) }; //adding "0.3 * upt" because GraphicsManager::windowPixel_to_mapUnits() isn't really accurate in perspective mode
}

auto GraphicsManager::glfwWindowPixel_to_mapUnits(Vector2f const glfw_cursorPos) const -> Vector3f
{
	//static auto test_counter = 0;
	//std::cout << "glfwWindowPixel_to_mapUnits: #" << test_counter << std::endl;
	//++test_counter;


	//std::cout << "\n\nGLFW screen reference system: " << glfw_cursorPos << std::endl;


	// Conversion of mouse coordinates from "GLFW screen reference system" to "OpengGL framebuffer reference system"
	auto openGl_fboPos = GraphicsManagerCore::glfwScreenRS_to_openGlFramebufferRS(glfw_cursorPos, m_window_size, m_defaultFbo_size);

	//std::cout << "OpenGL framebuffer reference system: " << openGl_fboPos << std::endl;
	


	// Retrieving the depth associated to the clicked pixel
	auto z_depth = 0.f;
	
	//bind the framebuffer where the actual depth test happens
	#if EDGE_DETECTION_FILTER
		glBindFramebuffer(GL_FRAMEBUFFER, m_edfScene_FBO);
	#elif OVERDRAW_FILTER
		glBindFramebuffer(GL_FRAMEBUFFER, m_overdraw_FBO);
	#else
		glBindFramebuffer(GL_FRAMEBUFFER, default_FBO);		
	#endif

	//read the depth component from the active framebuffer (be it the default one or an user-defined one)
	glReadPixels((GLint)openGl_fboPos.x, (GLint)openGl_fboPos.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z_depth);
	
	#if EDGE_DETECTION_FILTER || OVERDRAW_FILTER
		//rebind the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, default_FBO);
	#endif

	//std::cout << "Pixel z_depth: " << z_depth << std::endl;


	
	// Revert the rendering pipeline from the fragment back to the world space.
	auto const map_box = compute_mapBox();
	auto const view = compute_viewMatrix(map_box);
	auto const projection = compute_projectionMatrix(map_box);

	auto const world = GraphicsManagerCore::fragmentRS_to_openGlWorldSpaceRS(openGl_fboPos, z_depth, view, projection, m_defaultFbo_size);

	//auto const frag_pos = glm::vec3{ openGl_fboPos.x, openGl_fboPos.y, z_depth };			//position of the pixel and its depth
	//auto const viewport = glm::vec4{ 0, 0, m_defaultFbo_size.x, m_defaultFbo_size.y };		//window rectangle 
	//auto const world = glm::unProject(frag_pos, view, projection, viewport);				//(in pixels -- OpenGL world space reference system)
	


	// Convert from "OpenGL world space r.s. (units)" to "map r.s. (units)".
	auto map_z = world.z / GSet::floorsSpacing_ratio;

	auto wy_sliding = map_z * GSet::wySliding_ratio();
	auto map_x = - world.y + wy_sliding;

	auto map_y = world.x;

	Vector3f map_units{ map_x, map_y, map_z };


	
	//std::cout << "World coordinates (in pixels): " << world		<< '\n'
			  //<< "Map coordinates (in units):    " << map_units << '\n' << std::endl;



	return map_units;
}


void GraphicsManager::rebuff_freeTriangleVerticesVBO(GLuint VBO_id, FreeTriangleVertices const& ftv)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
	glBufferData(GL_ARRAY_BUFFER, ftv.buffer_byteSize(), ftv.get_ptr(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GraphicsManager::draw_genericVAO(GLuint VAO_id, FreeTriangleVertices const& ftv)
{
	glBindVertexArray(VAO_id);

	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(ftv.vertices_count()));

	glBindVertexArray(0); // no need to unbind it every time 
}


void GraphicsManager::free_objects()
{
	std::cout << "FREE OBJECTS" << std::endl;

	#if SHOW_VISIBLE_ENTITIES_FBO
		m_entityIds_viewer.close();
	#endif


	glDeleteVertexArrays(1, &m_windowQuad_VAO);
	glDeleteBuffers(1, &m_windowQuad_VBO);


	#if EDGE_DETECTION_FILTER
		free_edgeDetectionFilterObjects();
	#elif OVERDRAW_FILTER
		free_overdrawObjects();
	#endif
		
	free_tileObjects();


	#if OCCLUSION_CULLING
		free_occlusionCullingObjects();
	#endif	


	glDeleteVertexArrays(1, &m_dynamic_VAO);
	glDeleteBuffers(1, &m_dynamic_VBO);



	glDeleteVertexArrays(1, &m_building_VAO);
	glDeleteBuffers(1, &m_building_VBO);



	glDeleteVertexArrays(1, &m_roofSouth_VAO);
	glDeleteBuffers(1, &m_roofSouth_VBO);

	glDeleteVertexArrays(1, &m_roofWest_VAO);
	glDeleteBuffers(1, &m_roofWest_VBO);

	glDeleteVertexArrays(1, &m_roofNorth_VAO);
	glDeleteBuffers(1, &m_roofNorth_VBO);

	glDeleteVertexArrays(1, &m_roofEast_VAO);
	glDeleteBuffers(1, &m_roofEast_VBO);

	#if GSET_TILESET_TEXARRAY
		glDeleteTextures(1, &m_tilesetTexarray_id);
	#else
		glDeleteTextures(1, &m_tilesetTexture_id);
	#endif
	glDeleteTextures(1, &m_dynamicTexture_id);
	glDeleteTextures(1, &m_roofTexture_id);
}


#if EDGE_DETECTION_FILTER
	void GraphicsManager::free_edgeDetectionFilterObjects()
	{
		#if EDGE_DETECTION_FILTER_INTERMEDIATE_STEPS
			m_textured_viewer.close();
			m_edgeableIds_viewer.close();
			m_edges_viewer.close();
		#endif

		#if ALPHA_TO_COVERAGE
			glDeleteFramebuffers(1, &m_edfScene_msFBO);
			glDeleteRenderbuffers(1, &m_edfScene_msTexturedColorRBO);
			glDeleteTextures(1, &m_edfScene_msEdgeableIdsTex);
			glDeleteRenderbuffers(1, &m_edfScene_msDepthStencilRBO);
		#endif
		
		glDeleteFramebuffers(1, &m_edfScene_FBO);
		glDeleteTextures(1, &m_edfScene_edgeableIdsTex);
		glDeleteTextures(1, &m_edfScene_texturedColorTex);
		glDeleteRenderbuffers(1, &m_edfScene_depthStencilRBO);
		
		glDeleteFramebuffers(1, &m_edfEdges_FBO);
		glDeleteTextures(1, &m_edfEdges_colorTex);

		glDeleteFramebuffers(1, &m_edfThickenedEdges_FBO);
		glDeleteTextures(1, &m_edfThickenedEdges_colorTex);
	}
#endif

#if OVERDRAW_FILTER
	void GraphicsManager::free_overdrawObjects()
	{
		glDeleteFramebuffers(1, &m_overdraw_FBO);
		glDeleteRenderbuffers(1, &m_overdraw_colorRBO);
		glDeleteTextures(1, &m_overdraw_depthStencilTex);
	}
#endif

void GraphicsManager::free_tileObjects()
{
	glDeleteVertexArrays(1, &m_tile_VAO);
	glDeleteBuffers(1, &m_tile_VBO);
}

#if OCCLUSION_CULLING
	void GraphicsManager::free_occlusionCullingObjects()
	{
		glDeleteFramebuffers(1, &m_entityIds_FBO);
		glDeleteTextures(1, &m_entityIds_tex);
		glDeleteRenderbuffers(1, &m_entityIds_depthStencilRBO);

		glDeleteFramebuffers(1, &m_visibleBufferBuilder_FBO);
		glDeleteRenderbuffers(1, &m_visibleBufferBuilder_colorRBO);

		glDeleteBuffers(1, &m_visibleTile_indirectCommandBuffer);
		glDeleteBuffers(1, &m_visibleTile_ACB);

		glDeleteBuffers(1, &m_evaluatedEntities_SSBO);

		glDeleteBuffers(1, &m_visibleTile_VAO);
		glDeleteBuffers(1, &m_visibleTile_buffer);
	}
#endif


void GraphicsManager::debug_printUnsignedIntBuffer(GLint const beg_x, GLint const beg_y, GLsizei const width, GLsizei const height)
{
	using PixelCont = std::vector<GLuint>;
	PixelCont pixels(static_cast<PixelCont::size_type>(width) * height, 0u);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(beg_x, beg_y, width, height, GL_RED_INTEGER, GL_UNSIGNED_INT, pixels.data());
	

	std::cout << "Unsigned int buffer:" << std::endl;
	for (int y = 0; y < height; ++y)
	{
		auto const cast_y = static_cast<decltype(pixels)::size_type>(y);

		for (int x = 0; x < width; ++x)
		{
			std::cout << std::setw(8) << pixels[cast_y * width + x] << "   ";
		}

		std::cout << "\n\n";
	}

	std::cout << std::endl;
}


void GraphicsManager::debug_printSSBO(GLuint const SSBO, GLintptr const offset, GLsizeiptr const length)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	auto ptr = reinterpret_cast<GLuint *>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, offset, length, GL_MAP_READ_BIT));


	std::cout << "Reading an SSBO:\n";
	for (unsigned i = 0; i < length; ++i )
	{
		std::cout << "Entity #" << i << ": " << ptr[i]/*(ptr[i] == 1u ? "visible" : "not visible")*/ << "\n";
	}


	std::cout << "\n\n\n" << std::endl;


	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void GraphicsManager::debug_inspectTilesetVertexDataBuffer(GLuint const buffer, GLsizeiptr const vertex_count)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);

		std::vector<TilesetVertexData> test(vertex_count);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(TilesetVertexData) * vertex_count, (void*)test.data());

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GraphicsManager::debug_printTilesetVertexDataBuffer(GLuint const buffer, GLintptr const first_tile, GLsizeiptr const last_tile)
{
	auto const first_vert = first_tile * 6;
	auto const last_vert = last_tile * 6;
	auto const vert_count = last_vert - first_vert;


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);

		auto ptr = static_cast<TilesetVertexData *>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, first_vert, sizeof(TilesetVertexData) * vert_count, GL_MAP_READ_BIT));

		Logger lgr{ std::cout };
		lgr << "Reading TilesetVertexData buffer:\n";

		for (auto i = first_vert; i < last_vert; i += 6 )
		{
			lgr << "Tile #" << i / 6 << ": "
				<< Logger::addt
				<< Logger::nltb << ptr[i]
				<< Logger::nltb << ptr[i + 1]
				<< Logger::nltb << ptr[i + 2]
				<< Logger::nltb << ptr[i + 3]
				<< Logger::nltb << ptr[i + 4]
				<< Logger::nltb << ptr[i + 5]
				<< Logger::remt << '\n';
		}


		lgr << "\n\n\n" << std::endl;


		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}



} // namespace tgm