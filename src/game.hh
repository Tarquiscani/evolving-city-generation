#ifndef GM_GAME_HH
#define GM_GAME_HH


#include <vector>


#include "audio/audio_manager.hh"
#include "graphics/camera.hh"
#include "graphics/dynamic_manager.hh"
#include "graphics/dynamic_vertices.hh"
#include "graphics/graphics_manager.hh"
#include "graphics/preparation/roof_graphics_manager.hh"
#include "graphics/preparation/tile_graphics_manager.hh"
#include "graphics/roof_vertices.hh"
#include "graphics/tile_vertices.hh"
#include "map/gamemap.h"
#include "mediators/queues/gui_ev.hh"
#include "mediators/roof_graphics_mediator.hh"
#include "mediators/tile_graphics_mediator.hh"
#include "input/camera_controller.hh"
#include "input/main_window_objects.hh"
#include "system/clock.hh"
#include "systems/tutorial/demo_tutorial.hh"
#include "utilities/main_loop_data.hh"
#include "utilities/timed_counter.hh"
#include "window/glfw_wrapper.hh"

#include "debug/imgui/imgui_panels.hh"


namespace tgm
{



class Game
{
	public:
		Game();

		bool should_shutdown() const { return m_main_window.should_close(); }

		void tick();

	private:
		AudioManager m_audio_manager{};

		Window & m_main_window;

		TileVertices m_tile_vertices{};
		DynamicVertices m_dynamic_vertices;
		RoofVertices m_roof_vertices{};

		Camera m_camera{};

		GraphicsManager m_graphics_manager;

		DynamicManager m_dynamic_manager;
		TileGraphicsMediator m_tile_graphics_mediator{};
		RoofGraphicsMediator m_roof_graphics_mediator{};
		GuiEventQueues m_gui_events{};
	
		GameMap m_map;

		TileGraphicsManager m_tile_graphics_manager;
		RoofGraphicsManager m_roof_graphics_manager;

		DemoTutorial m_demo_tutorial;
		GuiManager m_gui_manager;

		CameraController m_camera_controller{};
		std::vector<BuildingId> m_created_buildings;

		MainWindowObjects m_main_window_objects;


		TimedCounter m_fps_counter{};	
		TimedCounter m_ups_counter{};
		MainLoopData m_main_loop_data{};

		Clock m_input_clock{};


		void tick_major_events();
		void tick_update();
		void tick_rendering();
		void tick_input();
};



} // namespace tgm


#endif // GM_GAME_HH