#ifndef GM_TEST_LOGGER_STREAMS_H
#define GM_TEST_LOGGER_STREAMS_H


#include "settings/debug/debug_settings.hh"
#if TESTLOG



	#include "test_logger.h"

	#if PLAYERMOVEMENT_TRAILSYSTEMVSBRUTE_TESTLOG
		inline static TestLogger PMtest("player_movement", "PMtest.txt");
	#endif



#endif


#endif //GM_TEST_LOGGER_STREAMS_H