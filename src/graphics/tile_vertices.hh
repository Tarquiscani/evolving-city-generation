#ifndef GM_TILE_VERTICES_HH
#define GM_TILE_VERTICES_HH


#include <vector>
#include <map>

#include <glad/glad.h> //needed only for OpenGL types

#include "settings/graphics_settings.hh"
#include "graphics/data_structures/tileset_vertex_data.hh"
#include "graphics/textures/texture_2d_array.hh"
#include "gimp_square.hh"
#include "texture_subimage.hh"
#include "map/tiles/border_type.hh"
#include "map/tiles/tile.hh"

#include "settings/debug/debug_settings.hh"
#include "debug/logger/streams.h"


namespace tgm
{



class TileSubimages
{
	public:

		////
		//	@pos: Position of the atlas of the first subimage associated to this BorderType (GIMP texture unit reference system).
		//	@subimages_count: Number of subimage versions representing the same tile.
		//	Note: Works only on subimage sequences arranged on two row of the same length (in the case of odd number of sprites, 
		//		  then the first row has one more sprite)
		////
		TileSubimages(Vector2i const pos, int const subimages_count);

		////
		//	@subimage_version: Indicate which of the subimages associated to this Tile must be picked.
		////
		auto get_subimage(int const subimage_version) const -> TextureSubimage const&
		{
			if (subimage_version >= m_subimages.size())
			{
				throw std::runtime_error("Trying to pick an unexistent subimage_version.");
			}

			return m_subimages[subimage_version];
		}
		
		////
		//	@subimage_version: Indicate which of the subimages associated to this Tile must be picked.
		////
		auto get_layer(int const subimage_version) const -> int const&
		{
			if (subimage_version >= m_layers.size())
			{
				throw std::runtime_error("Trying to pick an unexistent subimage_version.");
			}

			return m_layers[subimage_version];
		}

	private:
		//texture subimages that representes different version of the same TileType
		std::vector<TextureSubimage> m_subimages;
		
		//subtextures that representes different version of the same TileType (used when the atlas is converted to a Texture2DArray)
		std::vector<int> m_layers;
};


class BorderSubimages
{
	public:
		////
		//	@n, @m: Atlas column and row of the first subimage associated to this BorderType (GIMP texture unit reference system).
		////
		BorderSubimages(int const n, int const m);

		auto get_subimage(BorderType const type) const -> TextureSubimage const&
		{
			auto it = m_subimages.find(type);
			if (it == m_subimages.cend())
			{
				throw std::runtime_error("No subimage associated to that BorderType.");
			}

			return it->second;
		}

		auto get_layer(BorderType const type)
		{
			auto it = m_layers.find(type);
			if (it == m_layers.cend())
			{
				throw std::runtime_error("No subimage associated to that BorderType.");
			}

			return it->second;
		}

	private:
		//coordinates on the texture of the sprites associated to the border
		std::map<BorderType, TextureSubimage> m_subimages; //4 vertices of a quad
		std::map<BorderType, int> m_layers; //used when the atlas is used with a Texture2DArray
};



class TileVertices
{
	private:
		static constexpr int triangles_per_tile = 2;
		static constexpr int vertices_per_triangle = 3;

		using VertCont = std::vector<TilesetVertexData>;
		using RandCont = std::vector<int>;

		// Size of a chunk of the buffer. It must be a multiple of 6 because each tile is made up of 6 vertices.
		// It's int because in glBufferSubData GLsizeiptr is a signed integer. //TODO: NOW: Ma non è int per niente... controlla
		static constexpr VertCont::size_type chunk_size = GraphicsSettings::chunkSize_inTile * triangles_per_tile * vertices_per_triangle;



		enum class TileVerticesState
		{
			uninitialized,				// Doesn't contain any data
			reset,						// New data loaded, but not yet in GPU memory
			synchronized,				// Almost all data are synchronized between CPU and GPU memory. There could still be some small changes not already updated with GPU
		};

	public:
		
		bool uninitialized() const { return m_state == TileVerticesState::uninitialized; }
		////
		//	Indicate whether the vertices have been reset. In this case their count could be changed as well.
		////
		bool has_been_reset() const { return m_state == TileVerticesState::reset; }


		////
		//	Indicate whether any change happened to the attributes of any vertex.
		////
		bool has_changed() const { assert_synchronized();  return !m_changed_chunks.empty(); }
		
		////
		//	@return: A vector in which each element correspond to a modified chunk. Each pair contains the offset of the chunk and a pointer to the chunk.
		////
		auto get_changes() const -> std::vector<std::pair<GLintptr, TilesetVertexData const*>>;

		static constexpr auto chunk_byteSize() { return chunk_size * sizeof(TilesetVertexData); }


		auto map_length() const -> int { assert_initialization(); return m_map_length; }
		auto map_width()  const -> int { assert_initialization(); return m_map_width; }
		auto map_height() const -> int { assert_initialization(); return m_map_height; }

		////
		//	Size of the buffer (in bytes).
		////
		auto buffer_byteSize() const { assert_initialization(); return m_vertices.size() * sizeof(TilesetVertexData); }
		auto vertices_count()  const { assert_initialization(); return static_cast<GLsizei>(m_vertices.size()); }
		auto tile_count() const { assert_initialization(); return static_cast<GLsizeiptr>(m_map_length) * m_map_width * m_map_height; }
		auto entityId_maxValue() const { assert_initialization(); return static_cast<GLsizeiptr>(compute_entityId(m_map_length - 1, m_map_width - 1, m_map_height - 1)); }

		auto get_ptr() const { assert_initialization(); return m_vertices.data(); }

		////
		//	Initialize for the first time or reset.
		////
		void reset(int const map_length, int const map_width, int const map_height);
		
		////
		//	Make TileVertices aware that all the new vertices have been loaded in the GPU memory.
		////
		void reset_acquired() { assert_reset(); m_state = TileVerticesState::synchronized; }
		
		////
		//	Make TileVertices aware that the changed vertices have been loaded in the GPU memory.
		////
		void changes_acquired() { assert_synchronized(); m_changed_chunks.clear(); }

		////
		//	@x, @y, @z: Tile coordinates.
		////
		void set_tileGraphics(int const x, int const y, int const z,
							  bool const is_border, TileType const tile_type, BorderType const border_type, BorderStyle const border_style);



		void debug_print(int sprite_start, int sprite_end) const;
		
		//TODO: 99: What are these functions for?
		float tx(float y) const
		{
			return (y / m_map_width * 2) - 1.f;
		}
		float ty(float x) const
		{
			return (x / m_map_length * 2) + 1.f;
		}
		float tu(float v) const
		{
			return v / 4096;
		}
		float tv(float u) const
		{
			return (4096 - u) / 4096;
		}

		auto debug_internalVector() -> VertCont & { return m_vertices; }


	private:
		TileVerticesState m_state = TileVerticesState::uninitialized;

		VertCont m_vertices;


		std::unordered_set<VertCont::size_type> m_changed_chunks;


		int m_map_length = 0;
		int m_map_width = 0;
		int m_map_height = 0;

		RandCont random_0to9_numberSequence;

		

		static inline std::map<TileType, TileSubimages> flat_sprites = {
				//{ TileType::ground,		 { { 5,  4}, 10 }  },
				{ TileType::ground,		 { { 0,  4}, 10 }  },
				{ TileType::underground, { {25,  8}, 10 }  },
				{ TileType::sky,		 { {22, 24}, 10 }  },
				{ TileType::wooden,      { { 0,  6}, 10 }  },
				{ TileType::grout,		 { { 5,  6}, 10 }  }
			};

		static inline std::map<BorderStyle, BorderSubimages> border_sprites = {
				{BorderStyle::brickWall, { 8, 8} }
			};

		
		void assert_uninitializedOrSynchronized() const 
		{ 
			if (!(m_state == TileVerticesState::uninitialized || m_state == TileVerticesState::synchronized)) { throw std::runtime_error("Neither uninitialized nor synchronized."); }
		}
		void assert_initialization() const { if (m_state == TileVerticesState::uninitialized) { throw std::runtime_error("Uninitialized TileVertices."); } }
		void assert_synchronized() const { if (m_state != TileVerticesState::synchronized) { throw std::runtime_error("Non-synchronized TileVertices."); } }
		void assert_reset() const { if (m_state != TileVerticesState::reset) { throw std::runtime_error("Non-reset TileVertices."); } }
		
		void init_polygons();

		////
		//	@tex_left, @tex_bottom, @tex_right, @tex_top: OpenGL-like coordinates.
		////
		void set_tileTexture(int const x, int const y, int const z, 
							 float const tex_left, float const tex_bottom, float const tex_right, float const tex_top, GLuint const texarray_layer);

		auto compute_index(int const x, int const y, int const z) const noexcept -> VertCont::size_type
		{
			return (static_cast<VertCont::size_type>(z) * m_map_width * m_map_length * triangles_per_tile * vertices_per_triangle)
				 + (static_cast<VertCont::size_type>(y)				  * m_map_length * triangles_per_tile * vertices_per_triangle)
				 + (static_cast<VertCont::size_type>(x)								 * triangles_per_tile * vertices_per_triangle);
		}

		////
		//	Check if position is contained in an arbitrary plane of the map.
		////
		bool contains(Vector2i const pos) noexcept
		{
			return pos.x >= 0 && pos.x < m_map_length && pos.y >= 0 && pos.y < m_map_width;
		}
		
		auto compute_entityId(int const x, int const y, int const z) const noexcept -> GLuint
		{
			return z * m_map_width * m_map_length 
				 + y * m_map_length 
				 + x 
				 + 1; //"+ 1" to avoid an entity_id with value 0
		}

		////
		//	For each tile on the plane return a random number that identify a particular version of the associated sprite.
		//	Otherwise all the tiles would be equal and the graphic effect would be quite ugly.
		////
		auto get_random_0to9(int const x, int const y) -> int { return random_0to9_numberSequence[static_cast<RandCont::size_type>(x) * m_map_width + y]; }
		////
		//	Make a sequence of random numbers ranging from 0 to 9. The size of the sequence is the same as the number of tiles in one plane of the map.
		//  This random sequence will be used to obtain an homogenous terrain pattern when drawing the tiles.
		////
		void cache_random0to9NumberSequence();
};



} //namespace tgm
using namespace tgm;

#endif //GM_TILE_VERTICES_HH