#ifndef GM_TIMED_COUNTER_HH
#define GM_TIMED_COUNTER_HH


#include <vector>

#include "system/clock.hh"


namespace tgm
{
	class TimedCounter
	{
		public:
			auto operator()() const noexcept { return m_current; }

			////
			//	@return: Elapsed time since the last increment.
			////
			auto elapsed_time() const { return m_timer.getElapsedTime(); }

			auto operator++() -> TimedCounter &
			{
				++m_current;

				// Update the history of the elapsed times between increments
				m_history[m_current_historyIndex++] = std::lroundf(1 / m_timer.getElapsedTime().asSeconds());
				if (m_current_historyIndex >= m_history_size) { m_current_historyIndex = 0; }
				m_timer.restart();

				return *this;
			}
			
			////
			//	@return: Average time elapsed between each increment.
			////
			auto perSecond_average() const noexcept -> int
			{
				auto mean_fps = 0;
				for (auto i = 0; i < m_history_size; ++i) { mean_fps += m_history[i]; }
				mean_fps /= m_history_size;

				return mean_fps;
			}

			auto const& history() const { return m_history; }

		private:
			long long m_current = 0ll;	//current tick
			Clock m_timer{};			//time elapsed since the last increment
			
			static auto const m_history_size = 100;
			std::vector<int> m_history = std::vector<int>(m_history_size, 0);	//history of the elapsed times between increments
			std::vector<int>::size_type m_current_historyIndex = 0;				//index of the last added value, needed in order to replace only oldest values
	};
}


#endif //GM_TIMED_COUNTER_HH