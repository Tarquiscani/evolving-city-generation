#ifndef GM_TUTORIAL_EV_HH
#define GM_TUTORIAL_EV_HH


#include <string>

#include "base_event.hh"
#include "event_queues_impl.hh"


namespace tgm
{



class TutorialTriggerEv : public BaseEvent
{
	public:
		TutorialTriggerEv(std::string const& a_tutorial_entry_id) :
			tutorial_entry_id(a_tutorial_entry_id) {}

		std::string const tutorial_entry_id;
};


using TutorialEventQueues = EventQueuesImpl< TutorialTriggerEv >;



} // namespace tgm


#endif //GM_TUTORIAL_EV_HH