#include "utilities.hh"


#include <cmath>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <random>
#include <sstream>

#include "settings/graphics_settings.hh"


namespace tgm
{
    


namespace Utilities
{
    auto floor(Vector3f const v) -> Vector3f
    {
                return { ::floorf(v.x), ::floorf(v.y), ::floorf(v.z) }; //TODO: 11: It should be "std::floorf", but g++ doesn't allow it.
    }

    auto round(Vector3f const v) -> Vector3f
    {
                return { std::round(v.x), std::round(v.y), std::round(v.z) };
    }

    auto interp(float const current, float const target, float const delta_time, float const interp_speed, float const rel_tolerance) -> float
    {
        if (interp_speed <= 0.f)
        {
            return target;
        }

        auto const total_delta = target - current;

        // If the delta is very small, then don't interpolate
        if (total_delta * total_delta < rel_tolerance)
        {
            return target;
        }

        // Distance traveled in this frame
        auto const partial_delta = total_delta * std::clamp(delta_time * interp_speed, 0.f, 1.f);

        return current + partial_delta;
    } 
    
    auto units_to_tiles(Vector2f const v) -> Vector2i { return { GSet::units_to_tiles(v.x), GSet::units_to_tiles(v.y) }; }

    static auto current_time_in_mcs()
    {
        auto const now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        return static_cast<unsigned>(now % (std::numeric_limits<unsigned>::max() - 10));
    }
    
    // TODO: Not thread-safe
    auto rand(int const max) -> int
    {
        static auto random_engine = std::mt19937{ current_time_in_mcs() };

        auto const random = random_engine();

        return static_cast<int>(random % max); // Safe cast, because the result cannot be bigger than max (that is an integer).
    }
    
    // TODO: Not thread-safe
    static auto rand_normally_distributed_float(float const mean, float const standard_deviation) -> int
    {
        static auto random_engine = std::mt19937{ current_time_in_mcs() };

        auto dist = std::normal_distribution<float>{ mean, standard_deviation };
        return dist(random_engine);
    }

    // TODO: Not thread-safe
    auto rand_normally_distributed_int(float const mean, float const standard_deviation) -> int
    {
        auto const random = rand_normally_distributed_float(mean, standard_deviation);

        if (random > std::numeric_limits<int>::min() && random < std::numeric_limits<int>::max())
        {
            return static_cast<int>(random);
        }
        else
        {
            g_log << "Error. The the normal distribution generated a random integer that is out of the 'int' allowed range.\n";
            return mean;
        }
    }
}



} //namespace tgm
