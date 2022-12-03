#ifndef GM_DEBUG_SETTINGS_HH
#define GM_DEBUG_SETTINGS_HH


////
//  Activate dynamic asserts and safer constructs (e.g. dynamic_cast instead of static_cast).
////
#define DEBUG_MODE true

#define DYNAMIC_ASSERTS (DEBUG_MODE && true)


#define DEBUGLOG false

#define BUILDEXP_DEBUGLOG		(DEBUGLOG && false)
#define PLAYERMOVEMENT_DEBUGLOG (DEBUGLOG && false)
#define VISUALDEBUG_DEBUGLOG	(DEBUGLOG && false)


#define VISUALDEBUG true

////
//	Sub-options
////
#define BUILDEXP_DEBUGLOG_SUITABLEPOSS_EVALUATION (BUILDEXP_DEBUGLOG && false)

#define VISUALDEBUG_DEBUGLOG_FRAMEINFOS true


////
//	This debug feature does the specified tests in background while the game is running. If any inconsistency emerges, 
//	that will be silently logged in a file in _debug/tests directory.
////
#define TESTLOG true


#define PLAYERMOVEMENT_TRAILSYSTEMVSBRUTE_TESTLOG (TESTLOG && true)



#if VISUALDEBUG


namespace tgm
{

	inline bool visualDebug_runtime_openWindow = true;			// Enable the opening of the window at each step.
	inline int visualDebug_runtime_maxRecordableDepth = 0;		// Indicate what type of steps have to be recorded.
	inline int constexpr visualDebug_maxStepDepth = 5;			// Indicate the maximum possible depth for a step.

	inline bool visualDebug_runtime_openWindowForBuildingExpansion = false;		// Enable the opening of the window at each step only for BuildingExpansionVisualDebug.
	inline bool visualDebug_runtime_openWindowForPlayerMovement = false;	    // Enable the opening of the window at each step only for PlayerMovementVisualDebug.
	inline bool visualDebug_runtime_openWindowForHipRoofMatrix = false;			// Enable the opening of the window at each step only for HipRoofMatrixVisualDebug.

	inline long long debug_highlightings_count = 0;
	inline long long debug_unhighlightings_count = 0;

} // namespace tgm


#endif //VISUALDEBUG
	

#endif //GM_DEBUG_SETTINGS_HH