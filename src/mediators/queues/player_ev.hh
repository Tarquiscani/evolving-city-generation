#ifndef GM_PLAYER_EV_HH
#define GM_PLAYER_EV_HH


#include "base_event.hh"
#include "event_queues_impl.hh"
#include "map/direction.h"


namespace tgm
{



class PlayerMovementEv : public BaseEvent
{
	public:
		PlayerMovementEv(Direction a_direction) :
			direction(a_direction) {}

		Direction const direction = Direction::none;
};


class DebugDecreasePlayerVelocityEv : public BaseEvent {};
class DebugIncreasePlayerVelocityEv : public BaseEvent {};



using PlayerEventQueues = EventQueuesImpl< PlayerMovementEv, DebugDecreasePlayerVelocityEv, DebugIncreasePlayerVelocityEv >;



} // namespace tgm


#endif //GM_PLAYER_EV_HH