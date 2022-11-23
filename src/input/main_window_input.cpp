#include "main_window_input.hh"


#include "settings/simulation/simulation_settings.hh"
#include "map/buildings/building_recipe.hh"
#include "map/buildings/tests/building_tests.hh"
#include "graphics/algorithms/hip_roof/tests/hip_roof_tests.hh"

#include "settings/debug/buildingexpansion_visualdebug.hh"
#include "settings/debug/playermovement_visualdebug.hh"
#include "settings/debug/hiproofmatrix_visualdebug_settings.hh"
#include "debug/visual/building_expansion/building_expansion_visual_debug.hh"

namespace tgm
{



namespace MainWindow
{
	static void debug_expand_city(GameMap & map, std::vector<BuildingId> & created_buildings)
	{
		static std::mt19937 rnd{ 105u };	// TODO: Use time as a seed
				
		auto const random = rnd();
		auto const should_expand = random % 100u > 40u;

		if (!created_buildings.empty() && should_expand)
		{
			map.debug_expand_random_building();

			std::cout << "Building expanded." << std::endl;
		}
		else
		{					
					
			auto const more_dim = static_cast<int>(random % 4u);

			auto const building_recipe = BuildingRecipe{ { 200.f, 200.f }, AreaType::cowshed, 
														 { 10 + more_dim, 10 + more_dim }, "farm" };
	

			auto const building_id = map.debug_buildBuilding_inNearestCity(building_recipe);
			if (building_id != 0)
			{
				created_buildings.push_back(building_id);
			}

			std::cout << "Building built in a city." << std::endl;
		}
	}


	void pressedKey_callback(Window & window, int const key)
	{
		auto u_ptr = std::any_cast<MainWindowObjects *>(window.user_pointer());
		auto & roof_vertices = u_ptr->roof_vertices;
		auto & camera = u_ptr->camera;
		auto & map = u_ptr->map;
		auto & gui_mgr = u_ptr->gui_mgr;
		auto & created_buildings = u_ptr->created_buildings;


		// Process pressed pressed keys (those that aren't expected to be held)
		switch (key)
		{
			case GLFW_KEY_P:
				camera.switch_projection();
				break;

			case GLFW_KEY_I:
				gui_mgr.switch_visibility();
				break;

			//// No key assigned
			//{
			//	// Teleport the player to the upper level
			//	auto player_pos = map.debug_getPlayerPosition_inTiles();
			//	map.new_mobileEvent<DebugPlayerTeleportationEv>(player_pos + Vector3i{ 0, 0, 1 });
			//	break;
			//}

			//// No key assigned
			//{
			//	// Teleport the player to the lower level
			//	auto player_pos = map.debug_getPlayerPosition_inTiles();
			//	map.new_mobileEvent<DebugPlayerTeleportationEv>(player_pos - Vector3i{ 0, 0, 1 });
			//	break;
			//}


			case GLFW_KEY_V:
				map.new_input<DebugDecreasePlayerVelocityEv>();
				break;

			case GLFW_KEY_B:
				map.new_input<DebugIncreasePlayerVelocityEv>();
				break;
					
			// No key assigned
				//map.new_mobileEvent<DebugShrinkPlayerFeetSquareEv>();
				//break;
					
			// No key assigned
				//map.new_mobileEvent<DebugEnlargePlayerFeetSquareEv>();
				//break;


			case GLFW_KEY_Y:
			{
				#if VISUALDEBUG
					++visualDebug_runtime_maxRecordableDepth;
					if (visualDebug_runtime_maxRecordableDepth > visualDebug_maxStepDepth)
						visualDebug_runtime_maxRecordableDepth = 0;

					std::cout << "VisualDebug max recordable depth: " << visualDebug_runtime_maxRecordableDepth << std::endl;
				#endif

				break;
			}

			case GLFW_KEY_1:
			{
				#if BUILDEXP_VISUALDEBUG
					visualDebug_runtime_openWindowForBuildingExpansion = (visualDebug_runtime_openWindowForBuildingExpansion ? false : true);
					std::cout << "BuildingExpansionVisualDebug: " << (visualDebug_runtime_openWindowForBuildingExpansion ? "activated" : "deactivated") << std::endl;
				#endif

				break;
			}

			case GLFW_KEY_2:
			{
				#if PLAYERMOVEMENT_VISUALDEBUG
					visualDebug_runtime_openWindowForPlayerMovement = (visualDebug_runtime_openWindowForPlayerMovement ? false : true);
					std::cout << "PlayerMovementVisualDebug: " << (visualDebug_runtime_openWindowForPlayerMovement ? "activated" : "deactivated") << std::endl;
				#endif

				break;
			}

			case GLFW_KEY_3:
			{
				#if HIPROOFMATRIX_VISUALDEBUG
					visualDebug_runtime_openWindowForHipRoofMatrix = (visualDebug_runtime_openWindowForHipRoofMatrix ? false : true);
					std::cout << "HipRoofMatrixVisualDebug: " << (visualDebug_runtime_openWindowForHipRoofMatrix ? "activated" : "deactivated") << std::endl;
				#endif

				break;
			}

			case GLFW_KEY_0:
			{
				#if VISUALDEBUG
					std::cout << "highlightings_count: " << debug_highlightings_count + debug_unhighlightings_count << std::endl;
				#endif

				break;
			}

			//// No key assigned
			//{
			//	// VisualDebug tests
			//	//BuildingExpansionVisualDebugTests::test_buildingSynchronization();

			//	//TODO: NOW: Fare in modo che i test non influiscano direttamente sulla mappa, ma mandino un input da elaborare nell'update.
			//	// BuildingManager tests
			//	//BuildingAlgorithmTests::blockOutline_tests(map);
			//	//BuildingAlgorithmTests::automatic_cityDevelopment(map, created_buildings);



			//	// HipRoofAlgorithm tests
			//	HipRoofAlgorithm::oldTest_roofPerimeterTileType_specialCases(map, roof_vertices);
			//	//HipRoofAlgorithm::test_polygons_specialCases(map, roof_vertices);
			//	//HipRoofAlgorithm::automatically_test_building_expansion(map);

			//	//std::cout << HipRoofAlgorithm::roofPerimeterMicrotileType_stats;
			//	//HipRoofAlgorithm::test_everyRoofPerimeterMicrotileTypeCase();


			//	////Trail algorithm tests
			//	//std::cout << "Trail algorithm comparison on an empty map." << std::endl;
			//	//TrailSystem::debug_compareMoveAlgorithms();

			//	//map.debug_compareMoveAlgorithms();

			//	break;
			//}

			case GLFW_KEY_SPACE:
			{
				//TODO: NOW: Manda un input alla simulazione e svolgi l� queste operazioni

				//auto prefab = BuildingSpecialCases::tris_building(map.debug_getPlayerPosition_inTiles());
				//auto prefab = BuildingSpecialCases::inner_yard(map.debug_getPlayerPosition_inTiles());
				//auto prefab = BuildingSpecialCases::twoTile_innerYard(map.debug_getPlayerPosition_inTiles());
				//auto prefab = BuildingSpecialCases::singleTile_innerYard(map.debug_getPlayerPosition_inTiles());
				//auto prefab = BuildingSpecialCases::double_innerYard(map.debug_getPlayerPosition_inTiles());
				//auto prefab = BuildingSpecialCases::replaceable_in_innerYard(map.debug_getPlayerPosition_inTiles());
				//auto prefab = BuildingSpecialCases::full_replacement(map.debug_getPlayerPosition_inTiles());
				//auto prefab = BuildingSpecialCases::disconnecting_replacement(map.debug_getPlayerPosition_inTiles());
				auto prefab = BuildingSpecialCases::disconnecting_replacement2(map.debug_getPlayerPosition_inTiles());
				//auto prefab = BuildingSpecialCases::nondisconnecting_replacement(map.debug_getPlayerPosition_inTiles());
				//auto prefab = BuildingSpecialCases::firstArea_replacement(map.debug_getPlayerPosition_inTiles());
				//auto prefab = BuildingSpecialCases::thinInnerYard_replacement(map.debug_getPlayerPosition_inTiles());

				//auto prefab = PrefabBuilding{ "farm" };
				//prefab.append_area(AreaType::field, map.debug_getPlayerPosition_inTiles() + Vector3i( 2, 2, 0), { 10, 10 });


				auto const& [bid, building] = map.debug_build_prefabBuilding(prefab);
				if (bid != 0)
				{
					created_buildings.push_back(bid);
				}

				break;
			}

			case GLFW_KEY_BACKSPACE:
			{
				//TODO: NOW: Manda un input alla simulazione e svolgi l� queste operazioni

				auto const t = map.debug_getTile(map.debug_getPlayerPosition_inTiles() + Vector3i(2, 2, 0));
				if (t && t->is_innerArea())
				{
					auto const bid = t->get_innerAreaInfo().bid();
					if (std::find(created_buildings.cbegin(), created_buildings.cend(), bid) == created_buildings.cend()) { std::cout << "Cannot remove an automatically built building" << std::endl; }
					map.debug_remove_building(bid);
					created_buildings.erase(std::find(created_buildings.begin(), created_buildings.end(), bid));
				}

				break;
			}

			// No key assigned
				//sim_settings.map.generate_roofs = !sim_settings.map.generate_roofs;
				//break;

			//case GLFW_KEY_F:
			//{
			//	//TODO: NOW: Manda un input alla simulazione e svolgi l� queste operazioni
			//	debug_expand_city(map, created_buildings);
			//	break;
			//}

			case GLFW_KEY_O:
				map.debug_interactWithAllDoors();
				break;

			//case GLFW_KEY_D:
			//{
			//	#if VISUALDEBUG
			//		visualDebug_runtime_openWindow = (visualDebug_runtime_openWindow ? false : true);
			//		Logger lgr{ std::cout };
			//		lgr << "VisualDebug: " << (visualDebug_runtime_openWindow ? "activated" : "deactivated");
			//		lgr << Logger::addt;

			//		#if BUILDEXP_VISUALDEBUG
			//			lgr << Logger::nltb << "BuildingExpansionVisualDebug: " << (visualDebug_runtime_openWindow && visualDebug_runtime_openWindowForBuildingExpansion  ? "activated" : "deactivated");
			//		#endif
			//		#if PLAYERMOVEMENT_VISUALDEBUG
			//			lgr << Logger::nltb << "PlayerMovementVisualDebug: " << (visualDebug_runtime_openWindow && visualDebug_runtime_openWindowForPlayerMovement  ? "activated" : "deactivated");
			//		#endif
			//		#if HIPROOFMATRIX_VISUALDEBUG
			//			lgr << Logger::nltb << "HipRoofMatrixVisualDebug: " << (visualDebug_runtime_openWindow && visualDebug_runtime_openWindowForHipRoofMatrix  ? "activated" : "deactivated");
			//		#endif

			//		lgr << std::endl;
			//	#endif

			//	break;
			//}

			case GLFW_KEY_LEFT_SHIFT:		//pressure-release pair
				camera.change_zoomSpeed();
				break;

			case GLFW_KEY_ESCAPE:
				gui_mgr.mainMenu_gui.switch_state();
				break;
		}
	}


	void heldKey_callback(Window & window, int const key)
	{
		auto u_ptr = std::any_cast<MainWindowObjects *>(window.user_pointer());
		auto & camera = u_ptr->camera;
		auto & map = u_ptr->map;
		auto & created_buildings = u_ptr->created_buildings;

		
		Direction drc = DirectionUtil::key_to_direction(key);
		if (drc != Direction::none) //if a direction-related key is pressed
		{
			map.new_input<PlayerMovementEv>(drc);
		}
		else
		{
			switch (key)
			{
				case GLFW_KEY_LEFT:
					camera.decrease_phi();
					break;

				case GLFW_KEY_RIGHT:
					camera.increase_phi();
					++inputCounter;
					break;

				case GLFW_KEY_UP:
					camera.decrease_theta();
					break;

				case GLFW_KEY_DOWN:
					camera.increase_theta();
					break;

				//case GLFW_KEY_E:
				//{
				//	for (auto bid : created_buildings)
				//	{
				//		map.debug_request_buildingExpansion(bid);
				//	}

				//	break;
				//}

				case GLFW_KEY_G:
				{
					debug_expand_city(map, created_buildings);
					break;
				}
			}
		}
	}


	void releasedKey_callback(Window & window, int const key)
	{
		auto u_ptr = std::any_cast<MainWindowObjects *>(window.user_pointer());
		auto & map = u_ptr->map;
		auto & camera = u_ptr->camera;


		if (DirectionUtil::key_to_direction(key) != Direction::none) //if a direction-related key is released
		{
			map.new_input<PlayerMovementEv>(Direction::none);
		}
		else
		{
			switch(key)
			{
				case GLFW_KEY_LEFT_SHIFT:		//pressure-release pair
					camera.change_zoomSpeed();
					break;
			}
		}
	}


	void mouseButton_callback(Window & window, Vector2f const mouse_pos, int const button, int const action, int const)
	{
		auto u_ptr = std::any_cast<MainWindowObjects *>(window.user_pointer());
		auto & graphics_manager = u_ptr->graphics_manager;
		auto & tileGraphics_mediator = u_ptr->tileGraphics_mediator;
		auto & map = u_ptr->map;
		auto & gui_mgr = u_ptr->gui_mgr;

		if (action == GLFW_RELEASE)
		{
			switch (button)
			{
				case GLFW_MOUSE_BUTTON_LEFT:
				{

					auto const tile_pos = graphics_manager.glfwWindowPixel_to_mapTile(mouse_pos);
					map.new_mobileEvent<DebugPlayerTeleportationEv>(tile_pos);

					break;
				}

				case GLFW_MOUSE_BUTTON_MIDDLE:
				{
					auto const tile_pos = graphics_manager.glfwWindowPixel_to_mapTile(mouse_pos);

					std::cout << "Mid-clicked tile: " << tile_pos << std::endl;

					auto const t = map.debug_getTile({ tile_pos.x, tile_pos.y, tile_pos.z });
					if (t) { gui_mgr.tile_gui.set_tile(t); }	//TODO: NOW: Forse meglio impostare la posizione e poi accedere alla mappa direttamente dal gui_mgr (per rendere esplicita la dipendenza)

					break;
				}

				case GLFW_MOUSE_BUTTON_RIGHT:
				{
					auto const tile_pos = graphics_manager.glfwWindowPixel_to_mapTile(mouse_pos);
					//map.debug_createDestroy_door(tile_pos);
					//map.debug_buildBorder(tile_pos);
					tileGraphics_mediator.debug_record_tileStyleChange(tile_pos, TileType::sky);

					break;
				}
			}
		}
	}


	void mouseScroll_callback(Window & window, float const, float const y_offset)
	{
		auto u_ptr = std::any_cast<MainWindowObjects *>(window.user_pointer());
		auto & camera = u_ptr->camera;

		camera.shift_zoom( - y_offset / 10.f);
	}


	void framebufferSize_callback(Window & window, Vector2i const new_size)
	{
		auto u_ptr = std::any_cast<MainWindowObjects *>(window.user_pointer());
		auto & graphics_manager = u_ptr->graphics_manager;
		auto & gui_mgr = u_ptr->gui_mgr;

		graphics_manager.resize_fbo(new_size);
		gui_mgr.resize(new_size);
	}


	void windowSize_callback(Window & window, Vector2i const new_size)
	{
		auto u_ptr = std::any_cast<MainWindowObjects *>(window.user_pointer());
		auto & graphics_manager = u_ptr->graphics_manager;

		graphics_manager.resize_window(new_size);
	}

} //namespace MainWindow



} //namespace tgm