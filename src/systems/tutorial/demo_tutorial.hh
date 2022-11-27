#ifndef GM_DEMO_TUTORIAL_HH
#define GM_DEMO_TUTORIAL_HH

#include "systems\tutorial\tutorial.hh"


namespace tgm
{
	

auto demo_tutorial() -> Tutorial &
{
	static auto is_init = false;
	static Tutorial ret;

	if (!is_init)
	{
		is_init = true;

		ret.add_step({ "step-1", 0, "Welcome" });
		ret.add_step({ "step-2", 0, "Do this" });
		ret.add_step({ "step-3", 0, "Do that" });
	}

	return ret;
}


} //namespace tgm
using namespace tgm;


#endif //GM_DEMO_TUTORIAL_HH