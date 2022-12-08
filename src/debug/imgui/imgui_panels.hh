#ifndef GM_IMGUI_PRINTERS_HH
#define GM_IMGUI_PRINTERS_HH


#include <chrono>
#include <vector>

#include "graphics/camera.hh"
#include "mediators/queues/gui_ev.hh"
#include "map/city_block.hh"
#include "map/gamemap.h"
#include "map/tiles/tile.hh"
#include "input/camera_controller.hh"
#include "system/clock.hh"
#include "system/vector3.hh"
#include "systems/player_manager.hh"
#include "systems/tutorial/demo_tutorial.hh"
#include "utilities/main_loop_data.hh"
#include "utilities/timed_counter.hh"
#include "window/imgui_wrapper.h"
#include "window/window_manager.hh"

#include "debug/imgui/main_menu_gui.hh"
#include "debug/imgui/main_loop_analyzer_gui.hh"
#include "debug/imgui/movement_gui.hh"
#include "debug/imgui/tutorial_panel.hh"


namespace tgm
{



class BasicGui
{
	public:
		void generate_layout(Vector2i const framebuffer_size, PlayerManager const& pmgr, Camera const& camera, CameraController const& camera_controller,
							 TimedCounter const& fps_counter, TimedCounter const& ups_counter, TimedCounter const& input_counter);
};


class TileGui
{
	public:
		TileGui(GuiEventQueues & gui_events) : m_gui_events{ gui_events } { }

		void generate_layout();
		void set_tile(Tile const*const t) { m_t = t; }

	private:
		Tile const* m_t = nullptr;					//tile currently rapresented (if "null" the window won't open)
		
		GuiEventQueues & m_gui_events;
};


class CityBlockGui
{
	public:
		CityBlockGui(GuiEventQueues & gui_events) : m_gui_events{ gui_events } { }

		void generate_layout();

	private:
		std::map<CityBlockId, bool> m_open_flags;
		GuiEventQueues & m_gui_events;

		auto check_events() -> std::pair<CityBlockId, CityBlock const*>;
};


class ControlGui : public BaseGui
{
	public:
		void generate_layout(Vector2i const framebuffer_size);
};

class OnScreenMessagePanel : public BaseGui
{
	public:
		void generate_layout(Vector2i const framebuffer_size, ImFont * custom_font);
};

class GuiManager
{
	public:
		GuiManager(Window const& window, GuiEventQueues & gui_events, DemoTutorial & demo_tutorial);


		BasicGui basic_gui{};
		MovementGui movement_gui{};
		TileGui tile_gui;
		CityBlockGui cityBlock_gui;
		MainMenuGui mainMenu_gui;
		MainLoopAnalyzerGui mainLoopAnalyzer_gui{};
		ControlGui control_gui{};
		TutorialPanel tutorial_panel;
		OnScreenMessagePanel on_screen_message_panel{};


		void switch_visibility() { m_hide = !m_hide; }
		void resize(Vector2i const new_fbo_size) { m_fbo_size = new_fbo_size; }

		void update();

		////
		//	Generate the panel layouts in ImGui metadata.
		////
		void generate_layout(GameMap const& map, Camera const& camera, CameraController const& camera_controller,
							 TimedCounter const& fps_counter, TimedCounter const& ups_counter, MainLoopData const& mainLoop_data, TimedCounter const& input_counter);

	private:
		bool m_hide = false;
		Vector2i m_fbo_size;

		GuiEventQueues & m_gui_events;

		ImFont* m_custom_font;
};



} //namespace tgm


#endif //GM_IMGUI_PRINTERS_HH