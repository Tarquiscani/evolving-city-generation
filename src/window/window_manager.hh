#ifndef GM_WINDOWS_MANAGER_HH
#define GM_WINDOWS_MANAGER_HH


#include <iostream>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <any>

#include <imgui_impl_custom/imgui_wrapper.h>

#include "system/vector2.hh"
#include "system/clock.hh"
#include "data_strctures/data_array.hh"
#include "window/glfw_wrapper.hh"
#include "utilities/main_loop_data.hh"

namespace tgm
{



class Window;

using WindowId = std::vector<Window>::size_type;



struct WindowOptions
{
	std::string title = "New Window";

	int width = 800;
	int height = 600;
	bool fullscreen = false;
	bool resizable = true;

	std::optional<WindowId> shared_context; //this window will share the same context of share_window
	bool vsync = true;

	Vector2i starting_pos = { 50, 50 };

	bool create_imguiContext = false;
};


////
//
//	Object oriented syntactic sugar over WindowManager.
//
////
class Window
{
	public:
		using KeyCallback = std::function<void(Window & window, int const key)>;
		using CursorPosCallback = std::function<void(Window & window, Vector2f const pos)>;
		using MouseButtonCallback = std::function<void(Window & window, Vector2f const pos, int const button, int const action, int const mods)>;
		using MouseScrollCallback = std::function<void(Window & window, float const x_offset, float const y_offset)>;
		using CharCallback = std::function<void(Window & window, unsigned int c)>;
		using FramebufferSizeCallback = std::function<void(Window & window, Vector2i const new_size)>;
		using WindowSizeCallback = std::function<void(Window & window, Vector2i const new_size)>;


		Window() = default;
		Window(Window const&) = delete;
		auto operator=(Window const&) -> Window & = delete;


		auto id() const noexcept -> WindowId { return m_id; }
		bool is_open() const noexcept { return m_handler; }
		bool should_close() const;

		////
		//	@return: The dimensions of the window framebuffer (in pixels -- OpenGL framebuffer reference system).
		////
		auto fbo_size() const -> Vector2i;

		////
		//	@return: The dimensions of the window (GLFW screen reference system).
		////
		auto window_size() const -> Vector2i;

		void open(WindowOptions const& opt);
		void activate();
		void close();


		void set_shouldClose();
		////
		//	Set the dimensions of the window.
		//	@new_size: (GLFW screen reference system)
		////
		void set_size(Vector2i const new_size);
		void set_callabacks(KeyCallback pressed_key, KeyCallback held_key, KeyCallback released_key,
						    CursorPosCallback cursor_pos, MouseButtonCallback mouse_button, MouseScrollCallback scroll,
							CharCallback char_callback,
						    FramebufferSizeCallback framebuffer_size, WindowSizeCallback window_size);

		////
		//	Associate to the window a generic chunk of data, whose type is ignored both by Window and WindowManager and known only to the input callbacks.
		////
		template <typename T>
		void set_userPointer(T user_pointer)
		{
			static_assert(std::is_pointer_v<T>, "'user_pointer' must be a pointer");

			m_user_pointer = user_pointer;
		}


		////
		//	Associate to the window a generic chunk of data, whose type is ignored both by Window and WindowManager and known only to the input callbacks.
		////
		auto user_pointer() const -> std::any {	return m_user_pointer; }

		void poll_events();
		////
		//	Note: wait_events() doesn't work fine with ImGui.
		////
		void wait_events();

		void activate_imguiCanvas();
		
		//TODO: 99: E' poco elegante passare questo parametro trova alternative
		static inline auto fake_mainLoopData = MainLoopData{};
		void display(MainLoopData & mainLoop_data = fake_mainLoopData);

		// Forcibly release all keys
		void release_all_keys();


	private:
		WindowId m_id = 0;
		GLFWwindow * m_handler = nullptr;

		bool m_has_imguiContext = false;					//indicate whether this window has an associated ImGui context
		ImGuiContext * m_imgui_context = nullptr;
		bool m_is_imguiCanvas_active = false;				//indicate whether the ImGui "canvas" has been activated for the current frame

		KeyCallback m_pressedKey_callback = nullptr;
		KeyCallback m_heldKey_callback = nullptr;
		KeyCallback m_releasedKey_callback = nullptr; 
		CursorPosCallback m_cursorPos_callback = nullptr;
		MouseButtonCallback m_mouseButton_callback = nullptr;
		MouseScrollCallback m_mouseScroll_callback = nullptr;
		CharCallback m_char_callback = nullptr;
		FramebufferSizeCallback m_framebufferSize_callback = nullptr;
		WindowSizeCallback m_windowSize_callback = nullptr;

		std::unordered_set<int> m_pressed_keys;
		std::unordered_set<int> m_released_keys;
		std::unordered_map<int, Clock> m_key_status;

		std::any m_user_pointer;

		void assert_open() const
		{
			if (!is_open())
				throw std::runtime_error("Unexpected state. The Window was expected to be open.");
		}

		void assert_closed() const
		{
			if (is_open())
				throw std::runtime_error("Unexpected state. The Window was expected to be closed.");
		}

		void assert_active() const;

		void assert_userPointer() const
		{
			if (!m_user_pointer.has_value())
			{
				throw std::runtime_error("No user pointer set.");
			}
		}

		////
		//	From the pressed and released keys during this tick, update their status and call the right 
		//	user-defined callback for each of them.
		////
		void process_keys();

	friend class WindowManager;
	friend auto operator<<(std::ostream & lgr, Window const& w) ->std::ostream &;
};



//////
//
//	It manages the windows, switch between one and another and switch between OpenGL and SFML context.
//	Note: The windows can be only created and not destroyed (but they can be closed).
//
//////
class WindowManager
{
	public:
		enum class RenderType
		{
			OpenGL,
			SFML,
		};

		static WindowId constexpr mainWindow_id = 0;

		WindowManager();
		~WindowManager();

		////
		//	Create a new closed window.
		////
		auto create_window() -> Window &;

		////
		//	Open an existing window.
		////
		auto open_window(WindowId const wid, WindowOptions const& opt) -> Window &;

		auto get_window(WindowId const wid) -> Window &;

		auto activeWindow_id() -> std::optional<WindowId> { return m_activeWindow_id; }

		auto activate_window(WindowId const wid) -> Window &;

		void close_window(WindowId const wid);

		void close_allWindows();

		void set_callbacks(WindowId const wid, 
						   Window::KeyCallback pressed_key, Window::KeyCallback held_key, Window::KeyCallback released_key,
						   Window::CursorPosCallback cursor_pos, Window::MouseButtonCallback mouse_button, Window::MouseScrollCallback scroll,
						   Window::CharCallback char_clbk,
						   Window::FramebufferSizeCallback framebuffer_size, Window::WindowSizeCallback window_size);

		auto main_window() -> Window & { return m_windows[mainWindow_id]; }


	private:
		std::deque<Window> m_windows{}; //std::deque doesn't invalidate references

		std::optional<WindowId> m_activeWindow_id{};



		auto get_window_byHandler(GLFWwindow * handler) -> Window &
		{
			//TODO: PERFORMANCE: Maybe std::find_if with a lambda it's better, but the lambda can't access m_handler because it's not friend of Window class
			for (auto & w : m_windows)
			{
				if (w.m_handler == handler)
				{
					return w;
				}
			}

			throw std::runtime_error("WindowManager received a callback for a window, but it doesn't contain any window with that callback.");
		}


		void assert_wid(WindowId const wid) const
		{
			if (wid >= m_windows.size())
				throw std::runtime_error("Invalid WindowId");
		}

		void assert_closed(WindowId const wid) const
		{
			if (m_windows[wid].is_open())
				throw std::runtime_error("Unexpected state. The window should have been closed");
		}

		void assert_open(WindowId const wid) const
		{
			if (!m_windows[wid].is_open())
				throw std::runtime_error("Unexpected state. The window should have been open.");
		}

		void assert_active(WindowId const wid) const
		{
			if (wid != m_activeWindow_id)
				throw std::runtime_error("Unexpected state. It's not the current active window");
		}

		static void set_contextCreationHints();

		////
		//	Internal callbacks associated to every window. They call user-defined callbacks.
		////
		static void internal_keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
		static void internal_cursorPosCallback(GLFWwindow* window, double x_pos, double y_pos);
		static void internal_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void internal_mouseScrollCallback(GLFWwindow * window, double x_offset, double y_offset);
		static void internal_charCallback(GLFWwindow* window, unsigned int c);
		static void internal_framebufferSizeCallback(GLFWwindow * window, int new_width, int new_height);
		static void internal_windowSizeCallback(GLFWwindow * window, int new_width, int new_height);
};


////
//	@return: A global object that manage all the application windows and their contexts.
//
//	NOTE: IT'S A GLOBAL OBJECT. It must never be used in the destructor of another global object, because of deinitialization order fiasco.
//		  If it's very necessary to use it in the destructor of a global object, then call it also in the constructor of that object. This
//		  way the windowsManager is forced to be constructed before that object and the C++ rules imply that it'll be also destroyed after 
//		  the destruction of that object.
////
auto windows_manager() -> WindowManager &;



} //namespace tgm
using namespace tgm;


#endif //GM_WINDOWS_MANAGER_HH