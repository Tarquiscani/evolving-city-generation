#ifndef GM_VISUALDEBUG_PLAYERMOVEMENT_SETTINGS_HH
#define GM_VISUALDEBUG_PLAYERMOVEMENT_SETTINGS_HH


#include "debug_settings.hh"


////
//  When enabled it activates the Visual Debug for the Trail Algorithm. A really bad collision algorithm that isn't worth
//  explaining. 
//  Only effective when VISUALDEBUG is active.
////
#define VISUALDEBUG_OPT_PLAYERMOVEMENT false



////
//  IMPLEMENTATION
//  Do not touch.
////

#define PLAYERMOVEMENT_VISUALDEBUG (VISUALDEBUG && VISUALDEBUG_OPT_PLAYERMOVEMENT)



#endif //GM_VISUALDEBUG_PLAYERMOVEMENT_SETTINGS_HH