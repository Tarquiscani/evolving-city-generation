#include "graphics_tests.hh"


#include "settings/simulation/simulation_settings.hh"


namespace tgm
{



namespace tests
{
    #pragma warning(disable: 4100)
    void graphics_tests(TileVertices & tile_vertices, DynamicManager & dynamic_manager)
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
        
            
        //FreePolygon testpoly;
        //testpoly.set_pos({ 0.f, 0.f, 30.f });
        //testpoly.push_vertex({  0.f,  0.f, 0.f,   0.f,   0.f });
        //testpoly.push_vertex({ 10.f,  0.f, 0.f,   0.f, 400.f });
        //testpoly.push_vertex({ 10.f, 10.f, 0.f, 400.f, 400.f });
        //testpoly.push_vertex({  3.f,  7.f, 0.f, 300.f, 100.f });
        //testpoly.push_vertex({  7.f,  7.f, 0.f, 300.f, 400.f });
        //testpoly.push_vertex({  0.f, 10.f, 0.f, 400.f,   0.f });
        //roof_vertices.north_roof.create_polygon(testpoly);


        #pragma warning(default: 4100)
    }
}



}	// namespace tgm