#include "log_streams.hh"


#include "utilities/filesystem_utilities.hh"


namespace tgm
{


    
#if DEBUGLOG
    #if BUILDEXP_DEBUGLOG
        auto create_building_expansion_log_stream() -> std::ofstream
        {
            return FsUtil::create_unique("_debug/debuglogs/building_expansion/BElog.txt");
        }
    #endif //BUILDEXP_DEBUGLOG
#endif //DEBUGLOG



} // namespace tgm 
