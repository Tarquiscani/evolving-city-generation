#ifndef GM_TUTORIAL_HH
#define GM_TUTORIAL_HH


#include <cassert>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "audio/audio_manager.hh"
#include "mediators/queues/tutorial_ev.hh"


namespace tgm
{
	


struct TutorialStep
{
	std::string id;
	std::string title;
	std::string message;
	std::string image_name;
	bool can_be_skipped;			// Indicate whether the step can be skipped by pressing "Next". Otherwise the user necessarily have to perform some action.
};


class Tutorial
{
	private:
		// A Tutorial event might be triggered in many different places, and a tutorial event doesn't affect any other system than the tutorial itself and 
		// its representation. So it makes sense for the event queues to be a global variable, easily accessible from everywhere.
		static inline std::unordered_map<std::string, TutorialEventQueues> events;
		static inline std::unordered_set<std::string> registered_tutorials;

	public:
		Tutorial(std::string const& name, AudioManager & audio_manager) : m_name{ name }, m_audio_manager(audio_manager)
		{
			// Check that no other tutorial with the same name exists
			assert(registered_tutorials.find("name") == registered_tutorials.cend());
		}
		Tutorial(Tutorial const&) = delete;
		Tutorial & operator=(Tutorial const&) = delete;
		
		////
		//	Usage example:
		//		Tutorial::add_event<TutorialTriggerEv>("tutorial-name", "my-step-id");
		////
		template<typename T, typename ...Args>
		static void add_event(std::string const& tutorial_name, Args&& ...args) { events[tutorial_name].push<T>(std::forward<Args>(args)...); }

		auto name() const noexcept { return m_name; }
		bool is_over() const noexcept { return m_current_step_idx >= m_steps.size(); }
		auto get_current_step() const -> std::optional<TutorialStep>;

		void add_step(TutorialStep const& tutorial_step);
		void update();

	private:
		std::string m_name;
		int m_current_step_idx = 0;
		std::vector<TutorialStep> m_steps;

		std::mutex m_mutex;

		AudioManager & m_audio_manager;

		void handle_trigger(std::string const& trigger_step_id);

		void go_back() noexcept;
		void go_ahead() noexcept;
		
		void internal_go_ahead();
};



} //namespace tgm


#endif //GM_TUTORIAL_HH