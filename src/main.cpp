#include "game.hh"


int main()
{
    auto game = tgm::Game{};

    while (!game.should_shutdown())
    {
        game.tick();
    }

    return 0;
}