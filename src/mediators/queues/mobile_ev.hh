#ifndef GM_MOBILE_EV_HH
#define GM_MOBILE_EV_HH


#include "base_event.hh"
#include "event_queues_impl.hh"

#include "system/vector3.hh"
#include "map/map_forward_decl.hh"


class DebugPlayerTeleportationEv : public BaseEvent
{
	public:
		/////
		//  Note: Map reference system (in pixels)
		////
		DebugPlayerTeleportationEv(Vector3i const a_tile_pos) :
			tile_pos(a_tile_pos) {}

		Vector3i const tile_pos;
};

class DebugShrinkPlayerFeetSquareEv : public BaseEvent {};
class DebugEnlargePlayerFeetSquareEv : public BaseEvent {};


using MobileEventQueues = EventQueuesImpl< DebugPlayerTeleportationEv, DebugShrinkPlayerFeetSquareEv, DebugEnlargePlayerFeetSquareEv >;

#endif //GM_MOBILE_EV_HH
