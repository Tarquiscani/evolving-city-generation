#ifndef GM_DOOR_EV_HH
#define GM_DOOR_EV_HH


#include "base_event.hh"
#include "event_queues_impl.hh"
#include "map/map_forward_decl.hh"
#include "map/tiles/tile.hh"
#include "system/vector2.hh"
#include "system/vector3.hh"


namespace tgm
{



class TryOpenDoorEv : public BaseEvent
{
	public:
		TryOpenDoorEv(DoorId a_door_id) :
			door_id(a_door_id) {}

		DoorId const door_id;
};

class DebugInteractWithAllDoorsEv : public BaseEvent {};


using DoorEventQueues = EventQueuesImpl<TryOpenDoorEv, DebugInteractWithAllDoorsEv>;



} //namespace tgm


#endif //GM_DOOR_EV_HH