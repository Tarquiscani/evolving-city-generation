#include "player_manager.hh"


#include <iomanip>
#include <sstream>

#include "tutorial/tutorial.hh"
#include "ui/on_screen_messages.hh"


namespace tgm
{



void PlayerManager::update()
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
		if (new_vel > 0.00001f)
		{
			m_player_body.set_velocity(new_vel);

			auto oss = std::ostringstream{}; oss << std::setprecision(3) << "New velocity: " << new_vel;
			g_on_screen_messages.push_new_message(oss.str());
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
					
			auto oss = std::ostringstream{}; oss << std::setprecision(3) << "New velocity: " << new_vel;
			g_on_screen_messages.push_new_message(oss.str());
		}

		dipv_queue.pop();
	}
}



} // namespace tgm