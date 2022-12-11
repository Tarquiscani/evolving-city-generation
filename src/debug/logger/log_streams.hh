#ifndef GM_LOG_STREAMS_HH
#define GM_LOG_STREAMS_HH


#include <fstream>
#include <iostream>
#include <sstream>

// This file will be included in almost all headers. So keep its dependencies at the minimum to avoid circular dependencies.
#include "debug/logger/debug_printers.hh"
#include "debug/logger/log_buffer.hh"
#include "debug/logger/logger.hh"
#include "settings/debug/debug_settings.hh"


namespace tgm
{



inline auto g_log = std::ostream{ &g_log_buffer };


#if DEBUGLOG
    #if BUILDEXP_DEBUGLOG
        auto create_building_expansion_log_stream() -> std::ofstream;
        inline auto g_be_log_stream = create_building_expansion_log_stream();
        inline Logger BElog{ g_be_log_stream };
    #endif

    #if PLAYERMOVEMENT_DEBUGLOG
        inline Logger PMlog{ g_log };
    #endif

    #if VISUALDEBUG_DEBUGLOG
        inline Logger VDlog{ g_log };
    #endif
#endif //DEBUGLOG



} // namespace tgm


#endif //GM_LOG_STREAMS_HH