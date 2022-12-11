#ifndef GM_BASE_EVENT_HH
#define GM_BASE_EVENT_HH


#include "settings/debug/debug_settings.hh"

class BaseEvent 
{
    protected:
        // Disallow deleting/destroying derived-class objects through "BaseClass*"
        ~BaseEvent() {};
};

#endif //GM_BASE_EVENT_HH