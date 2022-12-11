#include "log_buffer.hh"


#include "utilities/filesystem_utilities.hh"


namespace tgm
{



auto LogBuffer::create_out_file_stream() -> std::ofstream
{
    return FsUtil::create_unique("_logs/log.txt", true);
}



} // namespace tgm

