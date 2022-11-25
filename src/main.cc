#include <iostream>
#include <sstream>
#include <unordered_map>
#include <cfenv>	//TODO: NOW: Rimuovi dopo i test.

#include <imgui.h>
#include "window/glfw_wrapper.hh"			//must be included before imgui_impl_*.h

#include "settings/simulation/simulation_settings.hh"
#include "system/clock.hh"
#include "window/window_manager.hh"
#include "graphics/camera.hh"
#include "graphics/tile_vertices.hh"
#include "graphics/dynamic_vertices.hh"
#include "graphics/dynamic_manager.hh"
#include "graphics/graphics_manager.hh"
#include "graphics/preparation/tile_graphics_manager.hh"
#include "graphics/preparation/roof_graphics_manager.hh"
#include "mediators/queues/gui_ev.hh"
#include "mediators/tile_graphics_mediator.hh"
#include "map/gamemap.h"
#include "imgui_impl_custom/imgui_impl_glfw.h"
#include "imgui_impl_custom/imgui_impl_opengl3.h"
#include "input/camera_controller.hh"
#include "input/main_window_objects.hh"
#include "input/main_window_input.hh"
#include "game_state/data_writer.hh"
#include "game_state/data_reader.hh"
#include "utilities/timed_counter.hh"
#include "utilities/main_loop_data.hh"

#include "debug/logger/streams.h"
#include "debug/imgui/imgui_panels.hh"

//void allocatedmemory_forecast();
void test_graphicsTest(TileVertices & tile_vertices, DynamicManager & dynamic_manager);


int main()
{
	{//THIS SCOPING IS FOR DEBUG PURPOSE

	////allocatedMemory_forecast();

	init_glfw();


	auto & main_window = windows_manager().activate_window(WindowManager::mainWindow_id);

	main_window.set_callabacks(MainWindow::pressedKey_callback, MainWindow::heldKey_callback, MainWindow::releasedKey_callback,
							   nullptr, MainWindow::mouseButton_callback, MainWindow::mouseScroll_callback,
							   nullptr,
							   MainWindow::framebufferSize_callback, MainWindow::windowSize_callback);
	


	
	auto tile_vertices = TileVertices{};
	auto dynamic_vertices = DynamicVertices{ 60000u };
	auto roof_vertices = RoofVertices{};
	
	auto camera = Camera{};

	auto graphics_manager = GraphicsManager{ main_window.fbo_size(), main_window.window_size(), tile_vertices, &dynamic_vertices, roof_vertices, camera };

	
	auto dynamic_manager = DynamicManager{ &camera, dynamic_vertices };
	auto tgraphics_mediator = TileGraphicsMediator{};
	auto rgraphics_mediator = RoofGraphicsMediator{};
	auto gui_events = GuiEventQueues{};
	//TODO: NOW: Vorrei che la constness delle varie dipendenze rifletta il fatto che vengano modificate o meno dal dipendente (usa tecnica puntatori vista su Stackoverflow)
	auto map = GameMap{ sim_settings.test_seed, dynamic_manager, camera, tgraphics_mediator, rgraphics_mediator, gui_events };

	auto tileGraphics_mgr = TileGraphicsManager{ tgraphics_mediator, tile_vertices };
	auto roofGraphics_mgr = RoofGraphicsManager{ rgraphics_mediator, roof_vertices };

	graphics_manager.init();

	
	test_graphicsTest(tile_vertices, dynamic_manager);


	std::vector<BuildingId> created_buildings;
	Clock expansion_time;

	GuiManager gui_mgr{ main_window, gui_events };


	auto camera_controller = CameraController{};

	auto mwo = MainWindowObjects{ roof_vertices, camera, camera_controller, graphics_manager, tgraphics_mediator, map, gui_mgr, created_buildings };
	main_window.set_userPointer(&mwo);


	auto fps_counter = TimedCounter{};
	//Clock meanFps_clock;
	//auto elapsed_fps = 0ull;
	
	auto ups_counter = TimedCounter{};
	auto mainLoop_data = MainLoopData{};

	auto input_clock = Clock{};

	while (!main_window.should_close())
	{
		mainLoop_data.tick_begin();

		//FreePolygon testpoly;
		//testpoly.set_pos({ 0.f, 0.f, 30.f });
		//testpoly.push_vertex({  0.f,  0.f, 0.f,   0.f,   0.f });
		//testpoly.push_vertex({ 10.f,  0.f, 0.f,   0.f, 400.f });
		//testpoly.push_vertex({ 10.f, 10.f, 0.f, 400.f, 400.f });
		//testpoly.push_vertex({  3.f,  7.f, 0.f, 300.f, 100.f });
		//testpoly.push_vertex({  7.f,  7.f, 0.f, 300.f, 400.f });
		//testpoly.push_vertex({  0.f, 10.f, 0.f, 400.f,   0.f });
		//roof_vertices.north_roof.create_polygon(testpoly);

		////console-output frame per seconds
		//++elapsed_fps;
		//if (meanFps_clock.getElapsedTime().asSeconds() >= 15.f)
		//{
		//	std::cout << "FPS (mean of a 15s)" << static_cast<float>(elapsed_fps) / meanFps_clock.getElapsedTime().asSeconds() << std::endl;
		//	elapsed_fps = 0;
		//	meanFps_clock.restart();
		//}

		//expand added farms every 15 seconds
		/*if(expansion_time.getElapsedTime().asSeconds() >= 0.5)
		{
			for(auto id : created_buildings)
				map.debug_request_buildingExpansion(id);

			expansion_time.restart();
		}*/
		
		//TODO: NOW: Risolvi tutti i warning con W4

		//TODO: NOW: Metti l'analisi di questi eventi in una funzione a parte
		if (!gui_events.get<ExitEv>().empty()) { main_window.set_shouldClose(); }

		auto & save_queue = gui_events.get<SaveWorldEv>();
		if (!save_queue.empty()) 
		{
			//TODO: NOW: Add error-handling
			auto ofstream = std::ofstream{ "_SAVES/flatbuffertest.save", std::ios::trunc | std::ios::binary };

			auto fbbuilder = flatbuffers::FlatBufferBuilder{ 1024 };	//TODO: NOW: Scegli una dimensione iniziale più vicina alla realtà.

			auto map_offset = map.write(fbbuilder);
			fbbuilder.Finish(map_offset);


			auto buf = fbbuilder.GetBufferPointer();
			auto size = fbbuilder.GetSize();

			ofstream.write((char*) buf, size);

			//TODO: NOW: Usa flatbuffers per salvare la mappa (vediamo quanto ci mette...)

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

		auto & load_queue = gui_events.get<LoadWorldEv>();
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

			auto ts = schema::GetGameMap(fileData.data());
			map.read(ts);


			//auto ifstream = DataReader::generate_loadStream(load_queue.front().filename);

			//ifstream >> map;

			//DataArray<double, true> da(40);
			//ifstream >> da;
			//auto lgr = Logger{ std::cout };
			//lgr << da;

			load_queue.pop();
		}

		auto & mainloopAnalyzer_queue = gui_events.get<MainLoopAnalyzerEv>();
		if (!mainloopAnalyzer_queue.empty()) 
		{
			gui_mgr.mainLoopAnalyzer_gui.switch_state();

			mainloopAnalyzer_queue.pop();
		}

		auto & movementAnalyzer_queue = gui_events.get<MovementAnalyzerEv>();
		if (!movementAnalyzer_queue.empty()) 
		{
			gui_mgr.movement_gui.switch_state();

			movementAnalyzer_queue.pop();
		}

		auto & control_panel_queue = gui_events.get<ControlPanelEv>();
		if (!control_panel_queue.empty()) 
		{
			gui_mgr.control_gui.switch_state();

			control_panel_queue.pop();
		}


		//TODO: NOW: Implementa propriamente il Fixed Timestep (come scritto sul blog di Gafferon Games). Forse serve anche una rivisitazione della fisica,
		//			 permettendo l'interpolazione del game state.
		//			 Ma forse più che il fixed timestep si potrebbe optare per una soluzione simile a factorio. Cercare di far girare la simulazione costantemente
		//			 a 60 UPS, e ridurre gli FPS in caso di scarse prestazioni a una frazione della frequenza d'aggiornamento dello schermo (30 FPS, 20 FPS, 15 FPS...) 
		
		//TODO: NOW: Qui vanno analizzati gli input che agiscono sulla simulazione (ma forse già è così)
																																			mainLoop_data.update_begin();
		map.update();
																																			mainLoop_data.update_end();
																																			++ups_counter;

																																			mainLoop_data.rendering_begin();
		tileGraphics_mgr.prepare(map);
		roofGraphics_mgr.prepare(map);
		graphics_manager.draw();

		main_window.activate_imguiCanvas();
		gui_mgr.generate_layout(map, camera, fps_counter, ups_counter, mainLoop_data, inputCounter);



		main_window.display(mainLoop_data);
																																			++fps_counter;

																																			mainLoop_data.input_begin();
		main_window.poll_events();
		camera_controller.update_camera(input_clock.getElapsedTime().asSeconds(), camera);
		input_clock.restart();
																																			mainLoop_data.input_end();

		
																																			mainLoop_data.tick_end();
	}

	
	graphics_manager.shutdown();

	windows_manager().close_allWindows();

	glfwTerminate();

	}//THIS SCOPING IS FOR DEBUG PURPOSE (to read potential error messages)
	//auto stop = 0;
	//std::cin >> stop;

	return 0;
}

////Useful to compare the actual memory usage with the predicted one.
//void allocatedMemory_forecast()
//{
//    int tiles_count = Map::LENGTH * Map::WIDTH * Map::HEIGHT;
//
//    std::cout << "sizeof(char)"<< sizeof(char) << std::endl;
//    std::cout << "sizeof(Tile)"<< sizeof(Tile) << std::endl;
//    std::cout << "sizeof(Tile*)" << sizeof(Tile*) << std::endl;
//    std::cout << "Allocated memory:" << (sizeof(Tile) + sizeof(Tile*)*27 )*tiles_count << std::endl;
//
//    std::allocator<Tile> alloc;
//    Tile* tiles = alloc.allocate(tiles_count);
//    for(auto p=tiles; p!=tiles+tiles_count; ++p)
//        alloc.construct(p, Vector3i(0,0,0), TileType::ground);
//    std::cout << "Tile neighbor capacity: " << tiles->neighbors.capacity();
//
//    int pause;
//    std::cin>>pause;
//
//    for(auto p=tiles; p!=tiles+tiles_count; ++p)
//        alloc.destroy(p);
//    alloc.deallocate(tiles, tiles_count);
//}


#pragma warning(disable: 4100)
void test_graphicsTest(TileVertices& tile_vertices, DynamicManager& dynamic_manager)
{
	
	#if FREE_ASSETS
		DynamicSubimage free_wall{ {1000.f, 0, 602.f, 220.f, 1000.f, 0, 602.f, 220.f }, default_texture_dynamics };
		dynamic_vertices.create_sprite({ 5.f * GSet::upt, 5.f * GSet::upt, 5.f * GSet::upt, 0.4f * GSet::upt, 0.4f * GSet::upt, 0.6f * GSet::upt }, free_wall);
	#else
		DynamicSubimage tree{		{ 64.f, 700.f, 64.f, 55.f,  128.f, 1792.f, 128.f, 111.f},                     default_texture_dynamics };
		DynamicSubimage tree_roots{ { 64.f, 951.f, 64.f,  9.f,  128.f, 1903.f, 128.f,  17.f}, GraphicLayer::Mats, default_texture_dynamics };
		DynamicSubimage bookshelf{	{350.f, 213.f, 49.f, 49.f,  896.f,  544.f, 126.f, 128.f},		   		      default_texture_dynamics };
		DynamicSubimage box{		{  0.f,   0.f, 32.f, 32.f,    0.f,    0.f,  64.f,  64.f}, GraphicLayer::Mats, default_texture_dynamics };

		dynamic_manager.create({ 14.f * GSet::upt, 10.f * GSet::upt, sim_settings.map.ground_floor * GSet::upt,         1.f * GSet::upt,          2.f * GSet::upt,  84.f / 50.f * GSet::upt }, tree);
		dynamic_manager.create({ 15.f * GSet::upt, 10.f * GSet::upt, sim_settings.map.ground_floor * GSet::upt, 17.f / 64.f * GSet::upt,          2.f * GSet::upt,         0.1f * GSet::upt }, tree_roots);
		dynamic_manager.create({ 10.f * GSet::upt, 10.f * GSet::upt, sim_settings.map.ground_floor * GSet::upt, 26.f / 64.f * GSet::upt, 126.f / 64.f * GSet::upt, 102.f / 64.f * GSet::upt }, bookshelf);
		dynamic_manager.create({ 10.f * GSet::upt, 20.f * GSet::upt, sim_settings.map.ground_floor * GSet::upt, 33.f / 64.f * GSet::upt,          1.f * GSet::upt,          1.f * GSet::upt }, box);
	#endif

	/*
		tile_vertices.set_tileGraphics(4, 4, 30, false, TileType::ground, BorderType::none, BorderStyle::none);
		tile_vertices.set_tileGraphics(4, 5, 30, false, TileType::ground, BorderType::none, BorderStyle::none);
		tile_vertices.set_tileGraphics(5, 4, 30, false, TileType::ground, BorderType::none, BorderStyle::none);
		tile_vertices.set_tileGraphics(5, 5, 30, false, TileType::ground, BorderType::none, BorderStyle::none);
		tile_vertices.set_tileGraphics(5, 6, 30, false, TileType::ground, BorderType::none, BorderStyle::none);
		tile_vertices.set_tileGraphics(6, 5, 30, false, TileType::ground, BorderType::none, BorderStyle::none);
		tile_vertices.set_tileGraphics(6, 6, 30, false, TileType::ground, BorderType::none, BorderStyle::none);
	*/

	/*
		debmap.add_building(std::make_unique<TestBuilding>(Vector3i(40,30,29)));
		debmap.add_building(std::make_unique<TestBuilding>(Vector3i(43,30,29)));
		debmap.add_building(std::make_unique<TestBuilding>(Vector3i(43,30,25)));
		debmap.add_building(std::make_unique<TestBuilding>(Vector3i(10,30,29)));
		debmap.add_building(std::make_unique<TestBuilding>(Vector3i(43,30,29)));
		debmap.add_building(std::make_unique<TestBuilding>(Vector3i(80,30,29)));
	*/
		
	#pragma warning(default: 4100)
}