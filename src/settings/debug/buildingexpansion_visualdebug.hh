#ifndef GM_VISUALDEBUG_BUILDINGEXPANSION_SETTINGS_HH
#define GM_VISUALDEBUG_BUILDINGEXPANSION_SETTINGS_HH


#include "debug_settings.hh"


////
//  When enabled it activates the Visual Debug for the Building Expansion (aka City Generation Algorithm).
//  There are multiple sub-options to visually inspect different parts of the algorithm. This is because the
//  Visual Debug records every tiny step of the algorithm and keeps it in memory until the game is closed. 
//  It quickly fills up the RAM when everything is enabled.
//  Only effective when VISUALDEBUG is active.
////
#define VISUALDEBUG_OPT_BUILDINGEXPANSION true

////
//	When enabled it allows the recording of the steps related to determining whether a particular area of
//  predetermined dimensions can be built in a certain space.
//  Only effective when VISUALDEBUG and BUILDEXP_VISUALDEBUG are active.
////
#define VISUALDEBUG_OPT_BUILDINGEXPANSION_OPT_IS_AREA_BUILDABLE false

////
//	When enabled it allows the recording of the steps related to determining how many tiles a particular buildable area
//  has in common with other areas of the same building or block.
//  Only effective when VISUALDEBUG and BUILDEXP_VISUALDEBUG are active.
////
#define VISUALDEBUG_OPT_BUILDINGEXPANSION_OPT_BUILDABLE_POSITIONS true

////
//	When enabled it allows the recording of the steps related to determining the starting roof shape, that will then be extruded
//  through the Roof Generation Algorithm.
//  Only effective when VISUALDEBUG and BUILDEXP_VISUALDEBUG are active.
////
#define VISUALDEBUG_OPT_BUILDINGEXPANSION_OPT_ROOF_GENERATION false

////
//	When enabled it allows the recording of the steps related to determining where and what kind of doors should be built in an area.
//  Only effective when VISUALDEBUG and BUILDEXP_VISUALDEBUG are active.
////
#define VISUALDEBUG_OPT_BUILDINGEXPANSION_OPT_DOORS true



////
//  IMPLEMENTATION
//  Do not touch.
////

#define BUILDEXP_VISUALDEBUG                      (VISUALDEBUG && VISUALDEBUG_OPT_BUILDINGEXPANSION)
#define BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE    (BUILDEXP_VISUALDEBUG && VISUALDEBUG_OPT_BUILDINGEXPANSION_OPT_IS_AREA_BUILDABLE)
#define BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS  (BUILDEXP_VISUALDEBUG && VISUALDEBUG_OPT_BUILDINGEXPANSION_OPT_BUILDABLE_POSITIONS)
#define BUILDEXP_VISUALDEBUG_ROOF_GENERATION      (BUILDEXP_VISUALDEBUG && VISUALDEBUG_OPT_BUILDINGEXPANSION_OPT_ROOF_GENERATION)
#define BUILDEXP_VISUALDEBUG_DOORS                (BUILDEXP_VISUALDEBUG && VISUALDEBUG_OPT_BUILDINGEXPANSION_OPT_DOORS)



#endif //GM_VISUALDEBUG_BUILDINGEXPANSION_SETTINGS_HH