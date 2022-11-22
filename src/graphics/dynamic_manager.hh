#ifndef GM_DYNAMIC_MANAGER_HH
#define GM_DYNAMIC_MANAGER_HH


#include "graphics/dynamic_vertices.hh"
#include "graphics/camera.hh"
#include "utilities/asserts.hh"

namespace tgm
{



class DynamicManager
{
	public:
		DynamicManager(Camera const* camera, DynamicVertices & dynamic_vertices) : m_camera(*camera), m_dynamic_vertices(dynamic_vertices) 
		{
			assert_nonNullptrs(camera); 
		}

		DynamicManager(DynamicManager const&) = delete;
		auto operator=(DynamicManager const&) -> DynamicManager & = delete;

		////
		//	Create a sprite that starting from the base of the front volume raises diagonally for an height that depends on the dimension of the subimage.
		//	@volume: Volume of the object (in units -- map reference system).
		////
		auto create(FloatParallelepiped const volume, DynamicSubimage const& subimage, bool const round_to_nearest_pixel = false) -> SpriteId;
		
		
		////
		//	@volume: Volume of the object (in units -- map reference system)
		//	@drc: Direction towards which the sprite is moving.
		////
		void modify(SpriteId const id, FloatParallelepiped const volume, DynamicSubimage const& subimage, Direction const drc, bool const round_to_nearest_pixel);
		void modify(SpriteId const id, FloatParallelepiped const volume, DynamicSubimage const& subimage) { modify(id, volume, subimage, Direction::none, false); }

		void destroy(SpriteId const id);


	private:
		Camera const& m_camera;
		DynamicVertices & m_dynamic_vertices;
};



}
using namespace tgm;


#endif //GM_DYNAMIC_MANAGER_HH