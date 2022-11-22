#ifndef GM_SIM_INPUT_HH
#define GM_SIM_INPUT_HH


#include "system/vector3.hh"


enum class SimInputType
{
	PlayerChangeDrc,
	DebugPlayerTeleportation,
	DebugPlayerDecreaseVelocity,
	DebugPlayerIncreaseVelocity,
	DebugPlayerShrinkFeetSquare,
	DebugPlayerEnlargeFeetSquare,

	DebugAddBuilding,
	DebugRemoveBuilding,
	DebugExpandCity,

	DebugSwitchAllDoors,
	DebugCreateDestroyDoor,
};

class SimInput
{
	SimInputType type;

	Vector3i param1;
};


#endif //GM_SIM_INPUT_HH