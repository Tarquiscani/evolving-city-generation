#ifndef GM_MAIN_WINDOW_OBJECTS_HH
#define GM_MAIN_WINDOW_OBJECTS_HH


#include "input/camera_controller.hh"
#include "mediators/tile_graphics_mediator.hh"
#include "map/gamemap.h"
#include "graphics/camera.hh"
#include "graphics/graphics_manager.hh"
#include "debug/imgui/imgui_panels.hh"


namespace tgm
{


	
struct MainWindowObjects
{
	RoofVertices & roof_vertices;
	Camera & camera;
	CameraController & camera_controller;
	GraphicsManager & graphics_manager;
	TileGraphicsMediator& tileGraphics_mediator;
	GameMap & map;
	GuiManager & gui_mgr;

	std::vector<BuildingId> & created_buildings;
};



}


#endif //GM_MAIN_WINDOW_OBJECTS_HH