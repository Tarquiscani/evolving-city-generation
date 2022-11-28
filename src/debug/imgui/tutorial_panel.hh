#ifndef GM_TUTORIAL_PANEL_HH
#define GM_TUTORIAL_PANEL_HH


#include "system/vector2.hh"

#include "debug/imgui/base_gui.hh"


namespace tgm
{



class TutorialPanel : public BaseGui
{
	public:
		void generate_layout(Vector2i const framebuffer_size);
};



}


#endif //GM_TUTORIAL_PANEL_HH