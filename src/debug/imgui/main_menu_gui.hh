#ifndef GM_MAIN_MENU_GUI_HH
#define GM_MAIN_MENU_GUI_HH


#include "settings/graphics_settings.hh"
#include "system/vector2.hh"
#include "mediators/queues/gui_ev.hh"

#include "debug/imgui/base_gui.hh"


namespace tgm
{



class MainMenuGui : public BaseGui
{
	public:
		MainMenuGui(GuiEventQueues & gui_events) : m_gui_events{ gui_events } {}

		void generate_layout(Vector2i const framebuffer_size);

	private:
		GuiEventQueues & m_gui_events;

		static inline float const childModal_width = 400.f * GSet::imgui_scale();
		static inline float const childModal_height = 200.f * GSet::imgui_scale();

		void generate_saveModal_layout();
		void generate_loadModal_layout();
		void generate_optionModal_layout();
};



}



#endif //GM_MAIN_MENU_GUI_HH