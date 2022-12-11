#ifndef GM_ON_SCREEN_MESSAGES_HH
#define GM_ON_SCREEN_MESSAGES_HH


#include <string>
#include <chrono>


namespace tgm
{



class OnScreenMessages
{
    public:
        bool is_visible() const { return !m_current_message.empty(); }
        auto message() const { return m_current_message; }
        auto remaining_life() const { return m_message_remaining_life; }

        void push_new_message(std::string const& new_message);

        void update();

    private:
        static auto constexpr message_lifespan = std::chrono::milliseconds{ 1500 };

        std::string m_current_message;
        std::chrono::system_clock::time_point m_message_push_time;		// Time in which m_current_message was pushed
        float m_message_remaining_life;									// Fraction: remaining life / total life
};


inline OnScreenMessages g_on_screen_messages;



} // namespace tgm


#endif //GM_ON_SCREEN_MESSAGES_HH