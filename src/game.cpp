#include "game.hh"


#include "input/main_window_input.hh"
#include "ui/on_screen_messages.hh"
#include "window/window_manager.hh"


namespace tgm
{



Game::Game() 
	: m_main_window{ windows_manager().activate_window(WindowManager::mainWindow_id) }
	, m_dynamic_vertices{ 60000u }
	, m_graphics_manager{ m_main_window.fbo_size(), m_main_window.window_size(), m_tile_vertices, &m_dynamic_vertices, m_roof_vertices, m_camera }
	, m_dynamic_manager{ &m_camera, m_dynamic_vertices }
	//TODO: NOW: Vorrei che la constness delle varie dipendenze rifletta il fatto che vengano modificate o meno dal dipendente (usa tecnica puntatori vista su Stackoverflow)
	, m_map{ sim_settings.test_seed, m_dynamic_manager, m_camera, m_tile_graphics_mediator, m_roof_graphics_mediator, m_audio_manager, m_gui_events }
	, m_tile_graphics_manager{ m_tile_graphics_mediator, m_tile_vertices }
	, m_roof_graphics_manager{ m_roof_graphics_mediator, m_roof_vertices }
	, m_demo_tutorial{ m_audio_manager }
	, m_gui_manager{ m_main_window, m_gui_events, m_demo_tutorial }
	, m_main_window_objects{ m_roof_vertices, m_camera, m_camera_controller, m_graphics_manager, m_tile_graphics_mediator, m_map, m_gui_manager, m_created_buildings }
{	
	m_audio_manager.reproduce_sound_loop("media/audio/alexander_nakarada_adventure.mp3");


	m_main_window.set_callabacks(MainWindow::pressedKey_callback, MainWindow::heldKey_callback, MainWindow::releasedKey_callback,
								 nullptr, MainWindow::mouseButton_callback, MainWindow::mouseScroll_callback,
								 nullptr,
								 MainWindow::framebufferSize_callback, MainWindow::windowSize_callback);
	
	m_graphics_manager.init();
	
	//tests::graphics_tests(tile_vertices, dynamic_manager);

	m_main_window.set_userPointer(&m_main_window_objects);
}

		
//TODO: NOW: Solve all W4 warnings

void Game::tick()
{
	m_main_loop_data.tick_begin();

	tick_major_events();
	tick_update();
	tick_rendering();
	tick_input();

	m_main_loop_data.tick_end();
}


void Game::tick_major_events()
{
	if (!m_gui_events.get<ExitEv>().empty()) { m_main_window.set_shouldClose(); }

	auto & save_queue = m_gui_events.get<SaveWorldEv>();
	if (!save_queue.empty()) 
	{
		//TODO: NOW: Add error-handling
		auto ofstream = std::ofstream{ "_SAVES/flatbuffertest.save", std::ios::trunc | std::ios::binary };

		auto fbbuilder = flatbuffers::FlatBufferBuilder{ 1024 };	//TODO: NOW: Scegli una dimensione iniziale più vicina alla realtà.

		auto map_offset = m_map.write(fbbuilder);
		fbbuilder.Finish(map_offset);


		auto buf = fbbuilder.GetBufferPointer();
		auto size = fbbuilder.GetSize();

		ofstream.write((char*) buf, size);


		//auto ofstream = DataWriter::generate_saveStream(save_queue.front().filename);

		//ofstream << map;



		/*DataArray<double, true> da(200);
		auto id = da.create(0.3333333333).id();
		for (auto i = 0; i < 20; ++i)
		{
			da.create(1.0 / (i + 1.0));
		}
		da.destroy(id);
		da.create(0.66666666666666666);
		ofstream.setf(std::ios_base::fixed, std::ios_base::floatfield);
		ofstream.precision(std::numeric_limits<double>::max_digits10);
		ofstream << da;*/

		save_queue.pop();
	}

	auto & load_queue = m_gui_events.get<LoadWorldEv>();
	if (!load_queue.empty()) 
	{
		//TODO: NOW: Add error-handling
		// open the file:
		auto fileSize = std::streampos{};
		auto ifstream = std::ifstream{ "_SAVES/flatbuffertest.save", std::ios::in | std::ios::binary | std::ios::ate };

		// get its size:
		fileSize = ifstream.tellg();
		ifstream.seekg(0, std::ios::beg);

		// read the data:
		auto fileData = std::vector<std::uint8_t>(fileSize);
		ifstream.read((char*) fileData.data(), fileSize);

		auto ts = tgmschema::GetGameMap(fileData.data());
		m_map.read(ts);


		//auto ifstream = DataReader::generate_loadStream(load_queue.front().filename);

		//ifstream >> map;

		//DataArray<double, true> da(40);
		//ifstream >> da;
		//auto lgr = Logger{ std::cout };
		//lgr << da;

		load_queue.pop();
	}
}

void Game::tick_update()
{
	//TODO: NOW: Implementa propriamente il Fixed Timestep (come scritto sul blog di Gafferon Games). Forse serve anche una rivisitazione della fisica,
	//			 permettendo l'interpolazione del game state.
	//			 Ma forse più che il fixed timestep si potrebbe optare per una soluzione simile a factorio. Cercare di far girare la simulazione costantemente
	//			 a 60 UPS, e ridurre gli FPS in caso di scarse prestazioni a una frazione della frequenza d'aggiornamento dello schermo (30 FPS, 20 FPS, 15 FPS...) 

	m_main_loop_data.update_begin();
	m_map.update();
	m_gui_manager.update();
	m_demo_tutorial.update();
	g_on_screen_messages.update();
	m_main_loop_data.update_end();
	++m_ups_counter;
}

void Game::tick_rendering()
{
	m_main_loop_data.rendering_begin();
	m_tile_graphics_manager.prepare(m_map);
	m_roof_graphics_manager.prepare(m_map);
	m_graphics_manager.draw();

	m_main_window.activate_imguiCanvas();
	m_gui_manager.generate_layout(m_map, m_camera, m_fps_counter, m_ups_counter, m_main_loop_data, inputCounter);

	m_main_window.display(m_main_loop_data);
	++m_fps_counter;
}

void Game::tick_input()
{
	m_main_loop_data.input_begin();

	m_main_window.poll_events();
	m_camera_controller.update_camera(m_input_clock.getElapsedTime().asSeconds(), m_camera);
	m_input_clock.restart();
	m_main_loop_data.input_end();
}



} // namespace tgm