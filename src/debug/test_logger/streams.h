#ifndef GM_TEST_LOGGER_STREAMS_H
#define GM_TEST_LOGGER_STREAMS_H


#include "settings/debug/debug_settings.hh"
#if TESTLOG


	#include "test_logger.h"

	#if PLAYERMOVEMENT_TRAILSYSTEMVSBRUTE_TESTLOG
	namespace tgm
	{

		inline static TestLogger PMtest("player_movement", "PMtest.txt");

	} // namespace tgm
	#endif //PLAYERMOVEMENT_TRAILSYSTEMVSBRUTE_TESTLOG


#endif //TESTLOG


#endif //GM_TEST_LOGGER_STREAMS_H