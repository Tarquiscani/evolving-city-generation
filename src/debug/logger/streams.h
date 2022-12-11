#ifndef GM_LOGGER_STREAMS_H
#define GM_LOGGER_STREAMS_H


#include <fstream>

#include "settings/debug/debug_settings.hh"
#include "utilities/filesystem_utilities.hh"
#include "logger.h"
#include "debug_printers.h"


#if DEBUGLOG

	namespace tgm
	{



	#if BUILDEXP_DEBUGLOG
		inline auto debug_stream = FsUtil::create_unique("_debug/debuglogs/building_expansion/BElog.txt");
		inline Logger BElog{ debug_stream };
	#endif

	#if PLAYERMOVEMENT_DEBUGLOG
		inline Logger PMlog{ std::cout };
	#endif

	#if VISUALDEBUG_DEBUGLOG
		inline Logger VDlog{ std::cout };
	#endif



	}

#endif //DEBUGLOG



#endif //GM_LOGGER_STREAMS_H