#ifndef GM_TEST_LOGGER_STREAMS_H
#define GM_TEST_LOGGER_STREAMS_H


#include "settings/debug/debug_settings.hh"
#if TESTLOG


    #include "test_logger.h"

    #if TESTLOG_PLAYERMOVEMENT_TRAIL_SYSTEM_VS_BRUTE
    namespace tgm
    {

        inline static TestLogger PMtest("player_movement", "PMtest.txt");

    } // namespace tgm
    #endif //TESTLOG_PLAYERMOVEMENT_TRAIL_SYSTEM_VS_BRUTE


#endif //TESTLOG


#endif //GM_TEST_LOGGER_STREAMS_H