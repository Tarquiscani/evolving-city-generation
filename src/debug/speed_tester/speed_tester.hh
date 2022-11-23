#ifndef GM_SPEED_TESTER_HH
#define GM_SPEED_TESTER_HH


#include <stdexcept>
#include <deque>
#include <type_traits>

#include "system/clock.hh"


namespace tgm
{



class SpeedTester
{
	public:
		SpeedTester() = default;
		SpeedTester(SpeedTester const&) = delete;
		SpeedTester & operator=(SpeedTester const&) = delete;

		void start();
		void stop();

		////
		//	Check if the last checked chunk of code has been performed in an average time.
		//	@k: Specify how much the time should be bigger than average to trigger this function.
		////
		bool is_average(int const k = 5);

		////
		//	Time elapsed during the last test (in microseconds).
		////
		auto last_time() 
		{ 
			if (m_historical_data.empty()) { throw std::runtime_error("Cannot access the last time of a never-started tester."); }

			return m_historical_data.back(); 
		}

		auto mean_time() { return m_mean_time; }

	private:
		bool is_recording = false;
		Clock m_timer;
		std::deque<long long> m_historical_data;

		long long m_min_time = 0;
		long long m_max_time = 0;
		long long m_mean_time = 0;
			
		static decltype(m_historical_data)::size_type constexpr minSize_to_computeMean = 20;
		static decltype(m_historical_data)::size_type constexpr historicalData_maxSize = 10000;
};



} //namespace tgm


#endif //GM_SPEED_TESTER_HH