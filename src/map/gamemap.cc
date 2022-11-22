#include "gamemap.h"

#include <algorithm>

#include <iostream>
#include <ctime>

#include "settings/simulation/simulation_settings.hh"

#include "debug/logger/streams.h"
#include "debug/visual/player_movement_stream.hh"
#include "debug/test_logger/streams.h"



namespace tgm
{



GameMap::GameMap(unsigned const seed, DynamicManager & dynamic_manager, Camera & camera, 
				 TileGraphicsMediator & tg_mediator, RoofGraphicsMediator & rg_mediator, GuiEventQueues & gui_events) :
	m_tiles(sim_settings.map.test_length, sim_settings.map.test_width, sim_settings.map.test_height, m_door_events),
	m_random_generator(seed),
	m_player_body{ 0.64f, {sim_settings.map.test_length / 2.f, sim_settings.map.test_width / 2.f}, sim_settings.map.ground_floor, MobileStyle::FatRich },
	player_manager{ m_input_events, m_player_body },
	mobile_manager{m_mobile_events, m_player_body, m_npc_bodies, camera, dynamic_manager, m_tiles, m_buildings, m_door_events },
	door_manager{ m_door_events, m_doors, m_tiles, dynamic_manager },
	m_building_manager{ m_random_generator, m_tiles, m_buildings, door_manager, tg_mediator, rg_mediator },
	m_tgraphics_mediator{ tg_mediator },
	m_gui_events{ gui_events }
{
	mobile_manager.add_playerBody_to_map();

	#if PLAYERMOVEMENT_VISUALDEBUG
		PMdeb.start(m_tiles);

		//debug of player movement
		//"]" shape
		//get_tile(4, 4, 29)->type = TileType::sky;
		//get_tile(4, 5, 29)->type = TileType::sky;
		//get_tile(4, 6, 29)->type = TileType::sky;
		//get_tile(5, 6, 29)->type = TileType::sky;
		//get_tile(6, 6, 29)->type = TileType::sky;
		//get_tile(6, 5, 29)->type = TileType::sky;
		//get_tile(6, 4, 29)->type = TileType::sky;
		//PMdeb.add_impassableTile({4, 4, 29});
		//PMdeb.add_impassableTile({4, 5, 29});
		//PMdeb.add_impassableTile({4, 6, 29});
		//PMdeb.add_impassableTile({5, 6, 29});
		//PMdeb.add_impassableTile({6, 6, 29});
		//PMdeb.add_impassableTile({6, 5, 29});
		//PMdeb.add_impassableTile({6, 4, 29});

		////filled square shape
		//get_tile(4, 4, 29)->type = TileType::sky;
		//get_tile(4, 5, 29)->type = TileType::sky;
		//get_tile(4, 6, 29)->type = TileType::sky;
		//get_tile(5, 6, 29)->type = TileType::sky;
		//get_tile(6, 6, 29)->type = TileType::sky;
		//get_tile(6, 5, 29)->type = TileType::sky;
		//get_tile(6, 4, 29)->type = TileType::sky;
		//get_tile(5, 4, 29)->type = TileType::sky;
		//get_tile(5, 5, 29)->type = TileType::sky;
		//PMdeb.add_impassableTile({4, 4, 29});
		//PMdeb.add_impassableTile({4, 5, 29});
		//PMdeb.add_impassableTile({4, 6, 29});
		//PMdeb.add_impassableTile({5, 6, 29});
		//PMdeb.add_impassableTile({6, 6, 29});
		//PMdeb.add_impassableTile({6, 5, 29});
		//PMdeb.add_impassableTile({6, 4, 29});
		//PMdeb.add_impassableTile({5, 4, 29});
		//PMdeb.add_impassableTile({ 5, 5, 29 });

	#endif
}

GameMap::~GameMap()
{
	#if PLAYERMOVEMENT_VISUALDEBUG
		PMdeb.stop();
	#endif
}


void GameMap::update()
{
	m_building_manager.expand_buildings();

	door_manager.update();
	player_manager.update();
	/*ia.update()*/

	mobile_manager.update();
	mobile_manager.move();

	//TODO: NOW: Modifica l'EventSystem se necessario. Approfondisci bene tutti gli aspetti e raccogli tutte le esperienze in rete, ma poi fai l'Event System definitivo!
	//			 O meglio l'event system più adatto agli input. Non tutto verrà realizzato tramite eventi asincroni, specialmente all'interno di update.
	//TODO: NOW: Usa l'event system massicciamente per desincronizzare gli input della simulazione dalla simulazione vera e propria.

	auto & rcb_queue = m_gui_events.get<RetrieveCityBlockEv>();

	while (!rcb_queue.empty())
	{
		auto & e = rcb_queue.front();
			
		auto const cb = debug_getBlock(e.cbid);
		if (cb) { m_gui_events.push<OpenCityBlockGuiEv>(e.user_request, e.cbid, cb); }

		rcb_queue.pop();
	}
}



//\\				DEBUG				 //\\

#pragma warning(disable: 4702)
void GameMap::debug_compareMoveAlgorithms() const
{
	std::cout << "\n\n\n\nTrail algorithm comparison on the current map from the current position." << std::endl;

	auto starting_pos = player_manager.debug_getPlayerPosition_inUnits();
	auto move_drc = m_player_body.get_moveDirection();
	auto drc = (move_drc == Direction::none) ? Direction::SE : move_drc;

	TrailSystem::debug_compareMoveAlgorithms(starting_pos.x, starting_pos.y, m_player_body.z_floor(), drc, m_tiles);
	
	#pragma warning(default: 4702)
}



auto GameMap::write(flatbuffers::FlatBufferBuilder & fbb) const -> flatbuffers::Offset<schema::GameMap>
{
	auto const tileset_offset = m_tiles.write(fbb);

	return schema::CreateGameMap(fbb, tileset_offset);
}

void GameMap::read(schema::GameMap const*const ms)
{
	m_tiles.read(ms->tileset());
	m_tgraphics_mediator.record_reset();
}



} //namespace tgm