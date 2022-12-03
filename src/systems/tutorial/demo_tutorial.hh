#ifndef GM_DEMO_TUTORIAL_HH
#define GM_DEMO_TUTORIAL_HH


#include "systems/tutorial/tutorial.hh"


namespace tgm
{



class DemoTutorial : public Tutorial
{
	public:
		DemoTutorial(AudioManager & audio_manager);
};



} //namespace tgm


#endif //GM_DEMO_TUTORIAL_HH