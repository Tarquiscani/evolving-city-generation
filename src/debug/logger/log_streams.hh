#ifndef GM_LOG_STREAMS_HH
#define GM_LOG_STREAMS_HH


#include <fstream>
#include <iostream>
#include <sstream>

#include "debug/logger/debug_printers.hh"
#include "debug/logger/log_buffer.hh"
#include "debug/logger/logger.hh"
#include "settings/debug/debug_settings.hh"
#include "utilities/filesystem_utilities.hh"


namespace tgm
{



inline auto & g_log = std::ostream{ &g_log_buffer };


#if DEBUGLOG
    #if BUILDEXP_DEBUGLOG
        inline auto g_be_debug_stream = FsUtil::create_unique("_debug/debuglogs/building_expansion/BElog.txt");
        inline Logger BElog{ g_be_debug_stream };
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