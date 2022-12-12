#ifndef GM_GAME_VIDEO_MODE_HH
#define GM_GAME_VIDEO_MODE_HH


#include "glfw_wrapper.hh"


namespace tgm
{



////
//	Actual VideoMode in which the game run. 
////
class GameVideoMode : public VideoMode
{
    public:
        ////
        //	@fullscreen: Indicate whether the application have to be run in fullscreen mode or in window mode.
        //	@width, @height: Ignored in window mode. In fullscreen mode represent the chosen resolution.
        ////
        GameVideoMode(bool const fullscreen, int const width = 0, int const height = 0);

        bool fullscreen() const noexcept { return m_fullscreen; }

    private:
        bool m_fullscreen = false;
};



} // namespace tgm


#endif //GM_GAME_VIDEO_MODE_HH