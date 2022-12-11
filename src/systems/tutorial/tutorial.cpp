#include "tutorial.hh"


namespace tgm
{

    

auto Tutorial::get_current_step() const -> std::optional<TutorialStep>
{	
    auto lock = std::scoped_lock(m_mutex);

    // Return a copy in order to achieve thread-safety
    return !is_over() ? std::optional<TutorialStep>{m_steps[m_current_step_idx]} : std::nullopt;
}
    
void Tutorial::add_step(TutorialStep const& tutorial_step) 
{ 
    auto lock = std::scoped_lock(m_mutex);

    m_steps.push_back(tutorial_step); 
}

void Tutorial::update()
{
    auto lock = std::scoped_lock(m_mutex);

    auto & tutorial_queues = events[m_name];

    auto & tt_queue = tutorial_queues.get<TutorialTriggerEv>();
    if (!tt_queue.empty())
    {
        auto & e = tt_queue.front();

        handle_trigger(e.tutorial_step_id);

        tt_queue.pop();
    }

    auto & gb_queue = tutorial_queues.get<TutorialGoBackEv>();
    if (!gb_queue.empty())
    {
        go_back();

        gb_queue.pop();
    }


    auto & ga_queue = tutorial_queues.get<TutorialGoAheadEv>();
    if (!ga_queue.empty())
    {
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
        internal_go_ahead();
    }
}

void Tutorial::handle_trigger(std::string const& trigger_step_id)
{
    if (!is_over())
    {
        if (trigger_step_id == m_steps[m_current_step_idx].id)
        {
            internal_go_ahead();
        }
    }
    else
    {
        // Do nothing
    }
}

void Tutorial::internal_go_ahead()
{
    assert(!is_over());

    ++m_current_step_idx;
    m_audio_manager.reproduce_sound("media/audio/success.wav");
}


} // namespace tgm