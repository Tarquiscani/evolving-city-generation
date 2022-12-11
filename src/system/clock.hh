#ifndef GM_CLOCK_HH
#define GM_CLOCK_HH


#include <chrono>


namespace tgm
{



namespace detail
{
    using ClockType = std::chrono::steady_clock;		// This clock is guaranteed to always go forward. Time zones, synchronizations with remote clocks, daylight savings time change, all of them won't affect it
}


class TimeInterval
{
    private:
        using IntervalType = decltype(std::chrono::time_point<detail::ClockType>{} - std::chrono::time_point<detail::ClockType>{});


    public:

        TimeInterval(IntervalType const interval) :
            m_interval(interval) { }

        ////
        //	@return: Time in seconds.
        ////
        auto asSeconds() const -> float
        {
            using seconds_asFloat = std::chrono::duration<float, std::chrono::seconds::period>;

            return std::chrono::duration_cast<seconds_asFloat>(m_interval).count();
        }

        ////
        //	@return: Time in milliseconds.
        ////
        auto asMilliseconds() const
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(m_interval).count();
        }

        ////
        //	@return: Time in microseconds.
        ////
        auto asMicroseconds() const
        {
            return std::chrono::duration_cast<std::chrono::microseconds>(m_interval).count();
        }


    private:

        IntervalType m_interval;
};



class Clock
{
        public:
        ////
        //	The clock starts automatically after being constructed.
        ////
        Clock() :
            m_start_time{detail::ClockType::now()} { }

        auto getElapsedTime() const -> TimeInterval
        {
            return { detail::ClockType::now() - m_start_time };
        }

        ////
        //	Restart the clock.
        //	@return: Time elapsed since the clock was started.
        ////
        auto restart() -> TimeInterval
        {
            auto const now = detail::ClockType::now();

            auto const elapsed_time = TimeInterval{ now - m_start_time };

            m_start_time = now;

            return elapsed_time;
        }


    private:
        std::chrono::time_point<detail::ClockType> m_start_time; //Time of last reset, in microseconds
};



} //namespace tgm


#endif //GM_CLOCK_HH
