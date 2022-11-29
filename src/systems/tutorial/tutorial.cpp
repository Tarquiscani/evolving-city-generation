#include "tutorial.hh"


namespace tgm
{

	

auto Tutorial::get_current_step() const -> std::optional<TutorialStep>
{	
	std::scoped_lock(m_mutex);

	// Return a copy in order to achieve thread-safety
	return !is_over() ? std::optional<TutorialStep>{m_steps[m_current_step_idx]} : std::nullopt;
}
	
void Tutorial::add_step(TutorialStep const& tutorial_step) 
{ 
	std::scoped_lock(m_mutex);

	m_steps.push_back(tutorial_step); 
}

void Tutorial::update()
{
	std::scoped_lock(m_mutex);

	auto & tutorial_queues = events[m_name];

	auto & tt_queue = tutorial_queues.get<TutorialTriggerEv>();
	if (!tt_queue.empty())
	{
		// Ignore all trigger events other than the first one
		while (tt_queue.size() > 1)
		{
			tt_queue.pop();
		}

		auto & e = tt_queue.front();

		handle_trigger(e.tutorial_step_id);

		tt_queue.pop();
	}

	auto & gb_queue = tutorial_queues.get<TutorialGoBackEv>();
	if (!gb_queue.empty())
	{
		// Ignore all trigger events other than the first one
		while (gb_queue.size() > 1)
		{
			gb_queue.pop();
		}

		go_back();

		gb_queue.pop();
	}


	auto & ga_queue = tutorial_queues.get<TutorialGoAheadEv>();
	if (!ga_queue.empty())
	{
		// Ignore all trigger events other than the first one
		while (ga_queue.size() > 1)
		{
			ga_queue.pop();
		}

		go_ahead();

		ga_queue.pop();
	}
}

void Tutorial::go_back() noexcept
{
	if (m_current_step_idx > 0)
	{
		--m_current_step_idx;
	}
}

void Tutorial::go_ahead() noexcept
{
	if (!is_over())
	{
		++m_current_step_idx;
	}
}

void Tutorial::handle_trigger(std::string const& trigger_step_id)
{
	if (!is_over())
	{
		if (trigger_step_id == m_steps[m_current_step_idx].id)
		{
			++m_current_step_idx;
		}
	}
	else
	{
		// Do nothing
	}
}



} // namespace tgm