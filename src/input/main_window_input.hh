#ifndef GM_MAIN_WINDOW_INPUT_HH
#define GM_MAIN_WINDOW_INPUT_HH


#include <vector>

#include "input/main_window_objects.hh"


namespace tgm
{

inline TimedCounter inputCounter; //TODO: NOW: Rimuovi dopo i test

namespace MainWindow
{
	void pressedKey_callback(Window & window, int const key);

	void heldKey_callback(Window & window, int const key);

	void releasedKey_callback(Window & window, int const key);

	void mouseButton_callback(Window & window, Vector2f const pos, int const button, int const action, int const mods);

	void mouseScroll_callback(Window & window, float const x_offset, float const y_offset);

	void framebufferSize_callback(Window & window, Vector2i const new_size);

	void windowSize_callback(Window & window, Vector2i const new_size);
}



} //namespace tgm
using namespace tgm;


#endif //GM_MAIN_WINDOW_INPUT_HH