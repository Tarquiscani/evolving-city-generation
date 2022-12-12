#ifndef GM_DEBUG_SETTINGS_HH
#define GM_DEBUG_SETTINGS_HH



////
//  RUNTIME CHECKS
////

////
//  It enables dynamic asserts and other useful sanity checks (that will decrease the performance though).
////
#define DEBUG_MODE true

////
//  It enables dynamic asserts, that can be then dropped at release. 
////
// TODO: Replace DYNAMIC_ASSERTS with the check() or exec_check() macros throughout the codebase.
#define DYNAMIC_ASSERTS (DEBUG_MODE && true)



////
//  DEBUG LOGS
////

////
//  It enables debug logs. They are simple logs that will be printed on console and on file, with additional debug information.
//  It is only meaningful if also at least one of the options are enabled.
////
#define DEBUGLOG false

////
//  It enables debug logs related to the Building Expansion Algorithm (aka City Expansion Algorithm).
////
#define DEBUGLOG_OPT_BUILDING_EXPANSION  false

////
//  It enables debug logs related to the Visual Debug. Yes, even the Visual Debug needs, in turn, to be debugged.
//  It is only effective when DEBUGLOG is enabled.
////
#define DEBUGLOG_OPT_PLAYER_MOVEMENT false

////
//  It enables debug logs related to the Visual Debug. Yes, even the Visual Debug needs, in turn, to be debugged.
//  It is only effective when DEBUGLOG is enabled.
////
#define DEBUGLOG_OPT_VISUALDEBUG false

////
//  It enables debug logs related to the Visual Debug framing. A frame is a sub-area of the game map. When the Visual Debug records a particular
//  action, it zooms in to the interesting sub-area where the action is taking place.
//  It is only effective when DEBUGLOG and DEBUGLOG_OPT_VISUALDEBUG are enabled.
////
#define DEBUGLOG_OPT_VISUALDEBUG_OPT_FRAMEINFOS false



////
//  VISUAL DEBUG
////

////
//  It enables the Visual Debug. The visual debug is a simplified map of tiles, in which are "logged" actions taken by the algorithms.
//  With the Visual Debug the developer can watch live what the algorithm is doing and whether there are any oddities. This allow the developer
//  to catch any issue, before it generates an actual error.
//  Moreover it also records all the steps taken by the algorithm and keeps them in memory, so that the developer can watch them later, like the frames of
//  a movie.
//  It is only meaningful if also at least one of the actual Visual Debug instances are enabled.
////
#define VISUALDEBUG true



////
//  TEST LOG
////

////
//	This debug system does the specified tests in background while the game is running. If any inconsistency emerges, 
//	that will be silently logged in a file in _debug/tests directory.
////
#define TESTLOG true

////
//	If enabled will compare two different collision systems implemented in the engine and check whether their results match.
////
#define TESTLOG_OPT_PLAYERMOVEMENT_TRAIL_SYSTEM_VS_BRUTE true



////
//  DEPENDENCIES
////

////
//  This option will completely remove any feature related to ImGui (useful for debug purpose).
////
#define ENABLE_IMGUI true



////
//  IMPLEMENTATION
//  Do not touch.
////

#define TESTLOG_PLAYERMOVEMENT_TRAIL_SYSTEM_VS_BRUTE (TESTLOG && TESTLOG_OPT_PLAYERMOVEMENT_TRAIL_SYSTEM_VS_BRUTE)

#define BUILDEXP_DEBUGLOG		                    (DEBUGLOG && DEBUGLOG_OPT_BUILDING_EXPANSION)
#define PLAYERMOVEMENT_DEBUGLOG                     (DEBUGLOG && DEBUGLOG_OPT_PLAYER_MOVEMENT)
#define VISUALDEBUG_DEBUGLOG	                    (DEBUGLOG && DEBUGLOG_OPT_VISUALDEBUG)
#define DEBUGLOG_VISUALDEBUG_FRAMEINFOS             (VISUALDEBUG_DEBUGLOG && DEBUGLOG_OPT_VISUALDEBUG_OPT_FRAMEINFOS)



#if VISUALDEBUG
namespace tgm
{

    inline bool visualDebug_runtime_openWindow = true;			// Enable the opening of the window at each step (can be changed in-game by the user).
    inline int visualDebug_runtime_maxRecordableDepth = 0;		// Indicate what type of steps have to be recorded (can be changed in-game by the user).
    inline int constexpr visualDebug_maxStepDepth = 5;			// Indicate the maximum possible depth for a step.

    inline bool visualDebug_runtime_openWindowForBuildingExpansion = false;		// Enable the opening of the window at each step only for BuildingExpansionVisualDebug.
    inline bool visualDebug_runtime_openWindowForPlayerMovement = false;	    // Enable the opening of the window at each step only for PlayerMovementVisualDebug.
    inline bool visualDebug_runtime_openWindowForHipRoofMatrix = false;			// Enable the opening of the window at each step only for HipRoofMatrixVisualDebug.

    inline long long debug_highlightings_count = 0;
    inline long long debug_unhighlightings_count = 0;

} // namespace tgm
#endif //VISUALDEBUG
 


#endif //GM_DEBUG_SETTINGS_HH