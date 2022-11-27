#ifndef GM_TUTORIAL_HH
#define GM_TUTORIAL_HH

#include <mutex>
#include <optional>
#include <string>

#include "mediators/queues/tutorial_ev.hh"


namespace tgm
{
	


struct TutorialStep
{
	std::string id;
	int image;
	std::string message;
};


class Tutorial
{
	public:
		Tutorial() = default;
		Tutorial(Tutorial const&) = delete;
		Tutorial & operator=(Tutorial const&) = delete;
		
		bool is_over() const noexcept { return m_current_step_idx >= m_steps.size(); }
		auto get_current_step() const -> std::optional<TutorialStep>;

		void add_step(TutorialStep const& tutorial_step);
		void update();

	private:
		int m_current_step_idx = 0;
		std::vector<TutorialStep> m_steps;

		TutorialEventQueues m_events;

		std::mutex m_mutex;

		void handle_trigger(std::string const& trigger_step_id);
};



} //namespace tgm
using namespace tgm;


#endif //GM_TUTORIAL_HH