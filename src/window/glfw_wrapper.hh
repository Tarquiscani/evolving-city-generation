#ifndef GM_GLFW_WRAPPER_HH
#define GM_GLFW_WRAPPER_HH


#include <exception>
#include <iostream>

// They must be included in the following order. 'glfw3.h' can't be included separately.
#include <glad/glad.h>

//glfw options macro
//#define GLFW_DLL		// Definition required when using dll version of GLFW library
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


namespace tgm
{


struct VideoMode
{
	public:
		VideoMode() = default;

		VideoMode(int const width, int const height, int const red_bits, int const green_bits, int const blue_bits, int const refresh_rate)
			: m_width{ width }, m_height{ height }, m_red_bits{ red_bits }, m_green_bits{ green_bits }, m_blue_bits{ blue_bits }, m_refresh_rate{ refresh_rate } {}

		////
		//	@return: Screen resolution (in pixels).
		////
		auto width() const noexcept -> int { return m_width; }

		////
		//	@return: Screen height (in pixels).
		////
		auto height() const noexcept -> int { return m_height; }

		auto red_bits() const noexcept -> int { return m_red_bits; }
		auto green_bits() const noexcept -> int { return m_green_bits; }
		auto blue_bits() const noexcept -> int { return m_blue_bits; }
		auto refresh_rate() const noexcept -> int { return m_refresh_rate; }

	protected:
		int m_width = 0;
		int m_height = 0;
		int m_red_bits = 0;
		int m_green_bits = 0;
		int m_blue_bits = 0;
		int m_refresh_rate = 0;
};


class GLFW
{
	public:
		////
		//	Initialize both GLFW and GLAD.
		////
		GLFW();
		GLFW(GLFW const&) = delete;
		GLFW& operator=(GLFW const&) = delete;

		~GLFW();

		auto video_mode() -> VideoMode;

	private:
		bool m_is_init = false;
		
		void print_monitor_info();
		auto print_monitor(std::ostream & os, GLFWmonitor * monitor) -> std::ostream &;
		auto print_mode(std::ostream & os, GLFWvidmode const& mode) -> std::ostream &;
};


inline GLFW g_glfw{};



} //namespace tgm


#endif //GM_GLFW_WRAPPER_HH