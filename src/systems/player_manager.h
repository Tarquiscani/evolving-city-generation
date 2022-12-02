#ifndef GM_PLAYER_MANAGER_H
#define GM_PLAYER_MANAGER_H


#include "system/vector2.hh"
#include "mediators/queues/player_ev.hh"
#include "mediators/queues/mobile_ev.hh"
#include "map/map_forward_decl.hh"
#include "characters/mobile.h"
#include "tutorial/tutorial.hh"

#include "settings/debug/debug_settings.hh"

class PlayerManager
{
	public:
		PlayerManager(PlayerEventQueues & player_events, MobileBody & player_body) :
			m_player_events(player_events), m_player_body(player_body) {}
		

		void update()
		{
			auto & pm_queue = m_player_events.get<PlayerMovementEv>();
			while (!pm_queue.empty())
			{
				auto & e = pm_queue.front();

				if (e.direction == Direction::none)
				{
					m_player_body.set_moveDirection(Direction::none);
				}
				else
				{
					m_player_body.add_move_direction(e.direction);
					Tutorial::add_event<TutorialTriggerEv>("demo-tutorial", "movement");
				}

				pm_queue.pop();
			}

			
			auto & ddpv_queue = m_player_events.get<DebugDecreasePlayerVelocityEv>();
			while (!ddpv_queue.empty())
			{
				auto const new_vel = m_player_body.velocity() / 1.71f;
				if (new_vel > 0.f)
				{
					m_player_body.set_velocity(new_vel);
				}

				ddpv_queue.pop();
			}
			

			auto & dipv_queue = m_player_events.get<DebugIncreasePlayerVelocityEv>();
			while (!dipv_queue.empty())
			{
				auto const new_vel = m_player_body.velocity() * 2.f;
				if (new_vel < 20.f)
				{
					m_player_body.set_velocity(new_vel);
				}

				dipv_queue.pop();
			}
		}

		////
		//	@return: Tile occupied by the center of the player body volume (in tiles - map reference system).
		////
		auto debug_getPlayerPosition_inTiles() const -> Vector3i
		{
			auto center = m_player_body.volume().center();

			return { GSet::units_to_tiles(center.x), 
					 GSet::units_to_tiles(center.y),
					 GSet::units_to_tiles(center.z) };
		}

		////
		//	@return: Center of the player body volume (in units - map reference system).
		////
		auto debug_getPlayerPosition_inUnits() const -> Vector3f
		{
			return m_player_body.volume().center();
		}

		////
		//	@return: Player body volume (in units - map reference system).
		////
		auto debug_getPlayerVolume() const -> FloatParallelepiped
		{
			return m_player_body.volume();
		}

		auto debug_getPlayerVelocity() const { return m_player_body.velocity(); }


		////
		//	@feet_pos, @feet_dim: Map reference system (in units)
		////
		void debug_setPlayerFeetSquare(Vector2f const feet_pos, float const feet_dim)
		{
			m_player_body.set_feetPosition(feet_pos.x, feet_pos.y);
			m_player_body.debug_setFeetDim(feet_dim);
		}
		void debug_setPlayerVelocity(float const v)
		{
			m_player_body.set_velocity(v);
		}

	private:
		PlayerEventQueues & m_player_events;
		MobileBody & m_player_body;
};

#endif //GM_PLAYER_MANAGER_H