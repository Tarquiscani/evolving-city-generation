#ifndef GM_BASE_GUI_HH
#define GM_BASE_GUI_HH


#include <string>
#include <unordered_map>

#include <glad/glad.h>

#include "window/imgui_wrapper.h"


namespace tgm
{



struct GuiImage
{
	GLuint id;
	int width;
	int height;
};


class BaseGui
{
	public:
		bool is_open() const noexcept { return m_open; }
		void switch_state() noexcept { m_open = !m_open; }

	protected:
		bool m_open = false;

		static inline std::unordered_map<std::string, GuiImage> m_images;
		
		auto load_image(std::string const& img_name) -> GuiImage;

	private:
		////
		//	Load an image in the VRAM. Must be be called only once per image.
		////
		auto load_image(char const* low_def_path, char const* hd_path, char const* ultra_hd_path) -> GuiImage;
};



}


#endif //GM_BASE_GUI_HH
