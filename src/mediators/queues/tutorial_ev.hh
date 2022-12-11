#ifndef GM_TUTORIAL_EV_HH
#define GM_TUTORIAL_EV_HH


#include <string>

#include "base_event.hh"
#include "event_queues_impl.hh"


namespace tgm
{



struct TutorialTriggerEv : public BaseEvent
{
        TutorialTriggerEv(std::string const& a_tutorial_step_id)
            : tutorial_step_id{ a_tutorial_step_id } {}
        
        std::string const tutorial_step_id;
};

struct TutorialGoBackEv : public BaseEvent {};
struct TutorialGoAheadEv : public BaseEvent {};


using TutorialEventQueues = EventQueuesImpl< TutorialTriggerEv, TutorialGoBackEv, TutorialGoAheadEv >;



} // namespace tgm


#endif //GM_TUTORIAL_EV_HH