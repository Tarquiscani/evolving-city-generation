#ifndef GM_DATA_WRITER_HH
#define GM_DATA_WRITER_HH


#include <string>
#include <fstream>

#include "map/gamemap.h"


namespace tgm
{



namespace DataWriter
{
    auto generate_saveStream(std::string const& filename) -> std::ofstream;
} //namespace DataWriter



} //namespace tgm



#endif //GM_DATA_WRITER_HH