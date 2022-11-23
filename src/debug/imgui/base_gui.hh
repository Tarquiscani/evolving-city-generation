#ifndef GM_BASE_GUI_HH
#define GM_BASE_GUI_HH


#include <imgui.h>


namespace tgm
{



class BaseGui
{
	public:
		bool is_open() const noexcept { return m_open; }
		void switch_state() noexcept { m_open = !m_open; }

	protected:
		bool m_open = false;
};



}


#endif //GM_BASE_GUI_HH
