#include "speed_tester.hh"


#include "settings/debug/debug_settings.hh"


namespace tgm
{



void SpeedTester::start()
{
    #if DYNAMIC_ASSERTS
        if (is_recording) { throw std::runtime_error("Cannot start again an already started test."); }
    #endif


    m_timer.restart();

    is_recording = true;
}

void SpeedTester::stop()
{
    #if DYNAMIC_ASSERTS
        if (!is_recording) { throw std::runtime_error("Cannot stop a non-started test."); }
    #endif


    m_historical_data.push_back(m_timer.getElapsedTime().asMicroseconds());
    if (m_historical_data.size() > historicalData_maxSize)
    {
        m_historical_data.pop_front();
    }

    is_recording = false;
}

bool SpeedTester::is_average(int const k)
{
    #if DYNAMIC_ASSERTS
        if (is_recording) { throw std::runtime_error("Cannot compute the average for a started, non-stopped test."); }
    #endif


    if (m_historical_data.size() < minSize_to_computeMean)
    {
        return true;
    }

    auto const last_time = m_historical_data.back();

    m_mean_time = 0ll;
    for (auto const t : m_historical_data)
    {
        m_mean_time += t;
    }
    m_mean_time -= last_time;
    m_mean_time /= m_historical_data.size();

    return last_time < m_mean_time * k;
}



} //namespace tgm