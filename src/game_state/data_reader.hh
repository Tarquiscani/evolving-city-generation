#ifndef GM_DATA_READER_HH
#define GM_DATA_READER_HH


#include <string>
#include <fstream>


namespace tgm
{



namespace DataReader
{
    auto generate_loadStream(std::string const& filename) -> std::ifstream;
} //namespace DataReader



}


#endif //GM_DATA_READER_HH