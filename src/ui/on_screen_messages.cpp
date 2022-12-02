#include "on_screen_messages.hh"


namespace tgm
{



void OnScreenMessages::push_new_message(std::string const& new_message)
{
	m_current_message = new_message;
	m_message_push_time = std::chrono::system_clock::now();
}

void OnScreenMessages::update()
{
	if (!m_current_message.empty())
	{
		auto const elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_message_push_time);

		if (elapsed_time > message_lifespan)
		{
			m_current_message.clear();
		}
		else
		{
			m_message_remaining_life = ((message_lifespan - elapsed_time).count() * 1.f / message_lifespan.count());
		}
	}
}



} // namespace tgm