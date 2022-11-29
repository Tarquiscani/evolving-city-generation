#ifndef GM_TUTORIAL_PANEL_HH
#define GM_TUTORIAL_PANEL_HH


#include "system/vector2.hh"
#include "systems/tutorial/demo_tutorial.hh"

#include "debug/imgui/base_gui.hh"


namespace tgm
{



class TutorialPanel : public BaseGui
{
	public:
		TutorialPanel(DemoTutorial & demo_tutorial)
			: m_demo_tutorial{ demo_tutorial } {}

		void generate_layout(Vector2i const framebuffer_size);

	private:
		DemoTutorial & m_demo_tutorial;
};



}


#endif //GM_TUTORIAL_PANEL_HH