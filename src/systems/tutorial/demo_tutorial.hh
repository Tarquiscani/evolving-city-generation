#ifndef GM_DEMO_TUTORIAL_HH
#define GM_DEMO_TUTORIAL_HH


#include "systems/tutorial/tutorial.hh"


namespace tgm
{


auto demo_tutorial() -> Tutorial &
{
	static auto is_init = false;
	static Tutorial ret;

	if (!is_init)
	{
		is_init = true;

		ret.add_step({ "step-1", "tutorial_test", R"RAW(
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed efficitur feugiat ipsum, vitae eleifend neque congue vel. Praesent molestie lorem id dui dignissim pretium. Nam bibendum risus non lectus mollis condimentum. Mauris porta scelerisque ligula, vel lacinia arcu ullamcorper non. Quisque non ante eu arcu condimentum porta id sed urna. Aliquam ut metus aliquam, sollicitudin ligula ut, pulvinar justo. Aliquam eu sapien elementum, porta sapien sed, tristique diam. Aliquam vel facilisis lectus. Vivamus erat orci, suscipit quis arcu nec, eleifend volutpat nibh.

Vivamus mi velit, convallis ut pharetra nec, scelerisque quis nisl. Aliquam ultrices dui nulla, ut rhoncus mi tincidunt ut. Phasellus eget leo lorem. Duis non nulla vitae leo ornare rhoncus id at diam. Nullam luctus laoreet orci at hendrerit. Maecenas congue elit id nisi egestas pellentesque. Curabitur sed aliquam neque. Vivamus eget massa vitae est euismod rhoncus eu nec nulla. Proin quis libero sapien. Curabitur auctor dapibus odio, sit amet sollicitudin mi.

Nulla in nunc vel diam sagittis maximus. Fusce iaculis auctor nisi. Aenean ornare ipsum ut metus malesuada, vel dapibus dui lobortis. Praesent scelerisque erat non lectus convallis, et iaculis nunc volutpat. In eget convallis neque. Aenean id ipsum turpis. Sed velit augue, tristique id augue quis, faucibus finibus ex. Pellentesque eget dictum dui. Suspendisse rhoncus in justo ac gravida. Donec tristique consequat odio eu consectetur. Sed dignissim massa iaculis urna tristique congue. Ut consectetur, metus quis pulvinar placerat, mauris nulla commodo velit, at porttitor orci felis ut ante. Proin ornare fermentum feugiat. Quisque pretium sapien eu est vehicula, dapibus tristique dui placerat.

Sed porta, quam ut eleifend sodales, leo lectus ornare magna, non malesuada ante nulla a massa. Ut enim quam, interdum vel dapibus at, efficitur sit amet elit. Fusce id tincidunt turpis. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Ut elementum purus mauris, a efficitur arcu viverra a. Morbi non tellus id mi tempor elementum eu in dui. Fusce semper urna tempus tortor luctus molestie. Quisque pharetra turpis sit amet varius mattis. In fermentum nunc metus, ac tincidunt nunc feugiat nec. Maecenas quis tellus nisl. Praesent in elit in orci scelerisque iaculis elementum eget odio. Nulla quis imperdiet sem, eget porta lacus. Etiam tempor tortor non diam scelerisque, ut pretium eros iaculis. In eget tempor lorem. Praesent at nulla non eros vehicula commodo ut et ligula. Nam suscipit eros in nisi dignissim, nec tincidunt mauris scelerisque. 
)RAW" });
		ret.add_step({ "step-2", "", "Do this" });
		ret.add_step({ "step-3", "", "Do that" });
	}

	return ret;
}


} //namespace tgm
using namespace tgm;


#endif //GM_DEMO_TUTORIAL_HH