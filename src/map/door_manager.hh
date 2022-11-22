#ifndef GM_DOOR_MANAGER_HH
#define GM_DOOR_MANAGER_HH

#include "system/parallelepiped.hh"
#include "data_strctures/data_array.hh"
#include "mediators/queues/door_ev.hh"
#include "graphics/dynamic_subimage.hh"
#include "graphics/dynamic_manager.hh"
#include "map/map_forward_decl.hh"
#include "map/tiles/tile_set.hh"
#include "objects/door.hh"

#include "debug/visual/player_movement_stream.hh"


namespace tgm
{



class DoorManager
{
	public:
		DoorManager(DoorEventQueues & door_events, DataArray<Door> & doors, TileSet & tiles, DynamicManager & dynamic_manager) :
			m_door_events(&door_events), m_doors(doors), m_tiles(&tiles), m_dynamic_manager(dynamic_manager) {}

		void update();
		auto create_door(Vector3i const& pos, bool const vertical) -> DoorId;
		void destroy_door(DoorId const did);

		bool is_vertical(DoorId const did);

	private:
		//TOOD: NOW: Perchè m_door_events e m_tiles sono puntatori? Omologare l'uso di reference e pointer nei membri
		DoorEventQueues * m_door_events;
		DataArray<Door> & m_doors;
		TileSet * m_tiles; 
		DynamicManager & m_dynamic_manager;
		

		void open_door(Door & d);
		void try_close_door(Door & d);


		static inline DynamicSubimage const horizontalClosed_subimage{ {   96.f,  0.f,			   192.f,  0.f },			  default_texture_dynamics };
		static inline DynamicSubimage const horizontalOpen_subimage{   {  170.f,  0.f, 12.f, 32.f, 340.f,  0.f, 24.f, 64.f }, default_texture_dynamics };
		static inline DynamicSubimage const verticalClosed_subimage{   {   32.f,  0.f,			    64.f,  0.f },			  default_texture_dynamics };
		static inline DynamicSubimage const verticalOpen_subimage{	   {  224.f,  8.f, 32.f, 24.f, 448.f, 16.f, 64.f, 48.f }, default_texture_dynamics };


		////
		//	@return: The volume of the door (in voxels -- map reference system).
		////
		static auto compute_volume(Vector3i const& pos, bool const vertical, bool const open) noexcept -> FloatParallelepiped;
};



} //namespace tgm
using namespace tgm;


#endif //GM_DOOR_MANAGER_HH