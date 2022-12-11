#include "time_utilities.hh"


#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>


namespace tgm
{



namespace Time
{
    auto human_readable_datetime() -> std::string
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        auto ss = std::stringstream{};
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d__%H-%M-%S__UTC");

        return ss.str();
    }
}



} // namespace tgm 
