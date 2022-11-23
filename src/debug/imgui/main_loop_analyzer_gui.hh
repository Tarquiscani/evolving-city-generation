#ifndef GM_MAIN_LOOP_ANALYZER_GUI_HH
#define GM_MAIN_LOOP_ANALYZER_GUI_HH


#include "settings/graphics_settings.hh"
#include "utilities/timed_counter.hh"
#include "utilities/main_loop_data.hh"

#include "debug/imgui/base_gui.hh"


namespace tgm
{



class MainLoopAnalyzerGui : public BaseGui
{
	public:
		void generate_layout(TimedCounter const& fps_counter, TimedCounter const& ups_counter, MainLoopData const& mainLoop_data);

	private:
		bool m_frozen = false;
		
		std::vector<float> fps_history{};
		std::vector<float> ups_history{};
		
		float m_max = 0.f;
		std::vector<float> tickT_history{};
		std::vector<float> updateT_history{};
		std::vector<float> renderingT_history{};
		std::vector<float> swapT_history{};
		std::vector<float> inputT_history{};
};



}


#endif //GM_MAIN_LOOP_ANALYZER_GUI_HH