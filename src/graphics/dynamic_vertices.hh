#ifndef GM_DYNAMIC_VERTICES_HH
#define GM_DYNAMIC_VERTICES_HH

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>

#include "settings/graphics_settings.hh"
#include "system/parallelepiped.hh"
#include "data_strctures/slot_manager.hh"
#include "graphics/world_parallelepiped.hh"
#include "graphics/dynamic_subimage.hh"
#include "map/direction.h"

#include "settings/debug/debug_settings.hh"
#include "debug/logger/logger.h"


namespace tgm
{



using SpriteId = CompleteId;


class DynamicVertices
{
	public:
		using size_type = uint32_t;

		DynamicVertices(size_type const max_size);

		DynamicVertices(DynamicVertices const&) = delete;
		DynamicVertices & operator=(DynamicVertices const&) = delete;

		auto attribute_count() const { return m_attributes.size(); }
		auto vertices_count() const { return static_cast<int>(m_attributes.size() / 5); } //conversion needed because OpenGL require it to be a GLsizei
		auto get_ptr() const { return m_attributes.data(); }
		
		////
		//	Create a sprite that starting from the base of the front volume raise diagonally for an height that depends on the dimension of the subimage.
		//	@volume: Volume of the object (in units -- OpenGL world reference system).
		////
		auto create_sprite(WorldParallelepiped const volume, TextureSubimage const& subimage) -> SpriteId;
		
		////
		//	@volume: Volume of the object (in units -- map reference system)
		//	@drc: Direction towards which the sprite is moving.
		////
		void set_sprite(SpriteId const id, WorldParallelepiped const volume, TextureSubimage const& subimage);

		void destroy(SpriteId const id);


		auto debug_attributesVecSize() { return m_attributes.size(); }
		auto debug_attributesVec() { return m_attributes;	}
		auto & debug_slotManager() { return m_slot_mgr; }
		static auto debug_aps() { return aps; }

	private:
		std::vector<float> m_attributes;					//m_attributes != slots. A slot correspond to a whole sprite, i.e. 2 triangles, 6 vertices, 30 attributes.

		SlotManager m_slot_mgr;


		static int constexpr triangles_per_sprite = 2;
		static int constexpr vertices_per_triangle = 3;
		static int constexpr attributes_per_vertex = 5;
		static int constexpr aps = triangles_per_sprite * vertices_per_triangle * attributes_per_vertex;
		
		
		////
		//	@volume: Volume of the object (in units -- map reference system)
		//	@drc: Direction towards which the sprite is moving.
		////
		static void internal_setSprite(float *const a, WorldParallelepiped const volume, TextureSubimage const& subimage);


		friend auto operator<<(Logger & lgr, DynamicVertices const& dv) -> Logger &;
};



} //namespace tgm
using namespace tgm;


#endif //GM_DYNAMIC_VERTICES_HH