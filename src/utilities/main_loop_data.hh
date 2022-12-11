#ifndef GM_MAIN_LOOP_DATA_HH
#define GM_MAIN_LOOP_DATA_HH


#include <vector>

#include "system/clock.hh"


namespace tgm
{


class MainLoopData
{
    public:
        auto const& tickT_hst() const noexcept { return m_tickTime_history; }
        auto const& updateT_hst() const noexcept { return m_updateTime_history; }
        auto const& renderingT_hst() const noexcept { return m_renderingTime_history; }
        auto const& swapT_hst() const noexcept { return m_swapTime_history; }
        auto const& inputT_hst() const noexcept { return m_inputTime_history; }

        static inline auto time_measure = &TimeInterval::asMilliseconds;

        void tick_begin() { m_total_clock.restart(); }
        void tick_end() 
        {
            m_tickTime_history[m_current_historyIndex] = static_cast<int>( (m_total_clock.getElapsedTime().*time_measure)() );
            ++m_current_historyIndex;
            if (m_current_historyIndex >= m_history_size) { m_current_historyIndex = 0; }
        }


        void update_begin() { m_single_clock.restart(); }
        void update_end() { m_updateTime_history[m_current_historyIndex] = static_cast<int>( (m_single_clock.getElapsedTime().*time_measure)() ); }

        void rendering_begin() { m_single_clock.restart(); }		
        void rendering_end() { m_renderingTime_history[m_current_historyIndex] = static_cast<int>( (m_single_clock.getElapsedTime().*time_measure)() ); }

        void swap_begin() {	m_single_clock.restart(); }
        void swap_end()	{ m_swapTime_history[m_current_historyIndex] = static_cast<int>( (m_single_clock.getElapsedTime().*time_measure)() ); }

        void input_begin() { m_single_clock.restart(); }
        void input_end() { m_inputTime_history[m_current_historyIndex] = static_cast<int>( (m_single_clock.getElapsedTime().*time_measure)() ); }


    private:
        Clock m_single_clock{};
        Clock m_total_clock{};

        static auto const m_history_size = 100;

        std::vector<int> m_tickTime_history = std::vector<int>(m_history_size, 0);		//in ms

        std::vector<int> m_updateTime_history = std::vector<int>(m_history_size, 0);	//in ms
        std::vector<int> m_renderingTime_history = std::vector<int>(m_history_size, 0);	//in ms
        std::vector<int> m_swapTime_history = std::vector<int>(m_history_size, 0);		//in ms
        std::vector<int> m_inputTime_history = std::vector<int>(m_history_size, 0);		//in ms

        std::vector<int>::size_type m_current_historyIndex = 0;	//index of the last added value, needed in order to replace only oldest values
};


}


#endif //GM_MAIN_LOOP_DATA_HH