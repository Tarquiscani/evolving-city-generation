#ifndef GM_DOOR_EV_HH
#define GM_DOOR_EV_HH

#include "base_event.hh"
#include "event_queues_impl.hh"
#include "system/vector2.hh"
#include "system/vector3.hh"
#include "map/map_forward_decl.hh"
#include "map/tiles/tile.hh"



class TryOpenDoorEv : public BaseEvent
{
	public:
		TryOpenDoorEv(DoorId a_door_id) :
			door_id(a_door_id) {}

		DoorId const door_id;
};

class DebugInteractWithAllDoorsEv : public BaseEvent {};


using DoorEventQueues = EventQueuesImpl<TryOpenDoorEv, DebugInteractWithAllDoorsEv>;

#endif //GM_DOOR_EV_HH