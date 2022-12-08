#include "window_manager.hh"


#include <thread>
#include <type_traits>

#include "window/glfw_wrapper.hh"				// Must be included before imgui_impl_*
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include "settings/graphics_settings.hh"

namespace tgm
{



auto Window::fbo_size() const -> Vector2i
{
	#if DYNAMIC_ASSERTS
		assert_open();
	#endif

	auto width = 0,
		 height = 0;

	glfwGetFramebufferSize(m_handler, &width, &height);

	return { width, height };
}

auto Window::window_size() const -> Vector2i
{
	#if DYNAMIC_ASSERTS
		assert_open();
	#endif

	auto width = 0,
		 height = 0;

	glfwGetWindowSize(m_handler, &width, &height);

	return { width, height };
}
		
bool Window::should_close() const
{
	return glfwWindowShouldClose(m_handler);
}

//TODO: PERFORMANCE: Maybe this function could be inlined.
void Window::open(WindowOptions const& opt)
{
	windows_manager().open_window(m_id, opt);
}

//TODO: PERFORMANCE: Maybe this function could be inlined.
void Window::activate()
{
	windows_manager().activate_window(m_id);
}

//TODO: PERFORMANCE: Maybe this function could be inlined.
void Window::poll_events()
{
	glfwPollEvents();

	process_keys();
}

//TODO: PERFORMANCE: Maybe this function could be inlined.
void Window::wait_events()
{
	glfwWaitEvents();

	process_keys();
}

void Window::process_keys()
{
	// Update key status
	for (auto key : m_pressed_keys)
	{
		m_key_status.emplace(key, Clock{});
	}
	for (auto key : m_released_keys)
	{
		m_key_status.erase(key);
	}

	// Invoke user-defined callback for pressed keys
	if (m_pressedKey_callback)
	{
		for (auto key : m_pressed_keys)
		{
			m_pressedKey_callback(*this, key);
		}
	}

	// Invoke user-defined callback for released keys
	if (m_releasedKey_callback)
	{
		for (auto key : m_released_keys)
		{
			m_releasedKey_callback(*this, key);
		}
	}


	// Invoke user-defined callback for held keys
	if(m_heldKey_callback)
	{
		//Invoke if it has been just pressed
		for (auto key : m_pressed_keys)
		{
			m_heldKey_callback(*this, key);
		}

		//Invoke if it has been held for more than 0.01 seconds
		for (auto [key, clock] : m_key_status)
		{
			auto pressure_time = clock.getElapsedTime().asSeconds();

			if (pressure_time > 0.01f)
			{
				m_heldKey_callback(*this, key);
			}
		}
	}

	m_pressed_keys.clear();
	m_released_keys.clear();
}

void Window::release_all_keys()
{	
	if (m_releasedKey_callback)
	{
		for (auto & [key, clock] : m_key_status)
		{
			m_releasedKey_callback(*this, key);
		}
	}

	m_key_status.clear();
}


//TODO: PERFORMANCE: Maybe this function could be inlined.
void Window::close()
{
	windows_manager().close_window(m_id);


	m_pressedKey_callback = nullptr;
	m_heldKey_callback = nullptr;
	m_releasedKey_callback = nullptr;
	m_cursorPos_callback = nullptr;
	m_mouseButton_callback = nullptr;
	m_mouseScroll_callback = nullptr;
	m_char_callback = nullptr;
	m_framebufferSize_callback = nullptr;
	m_windowSize_callback = nullptr;

	m_pressed_keys.clear();
	m_released_keys.clear();
	m_key_status.clear();

	m_user_pointer.reset();
}

void Window::set_shouldClose()
{
	#if DYNAMIC_ASSERTS
		assert_open();
	#endif

	glfwSetWindowShouldClose(m_handler, GLFW_TRUE);
}

void Window::set_size(Vector2i const new_size)
{
	#if DYNAMIC_ASSERTS
		assert_open();
	#endif

	glfwSetWindowSize(m_handler, new_size.x, new_size.y);
}

//TODO: PERFORMANCE: Maybe this function could be inlined.
void Window::set_callabacks(KeyCallback pressed_key, KeyCallback held_key, KeyCallback released_key,
						    CursorPosCallback cursor_pos, MouseButtonCallback mouse_button, MouseScrollCallback scroll,
							CharCallback char_callback,
						    FramebufferSizeCallback framebuffer_size, WindowSizeCallback window_size)
{
	windows_manager().set_callbacks(m_id, pressed_key, held_key, released_key, cursor_pos, mouse_button, scroll, char_callback, framebuffer_size, window_size);
}


void Window::activate_imguiCanvas()
{
	#if DYNAMIC_ASSERTS
		if(!m_has_imguiContext)
		{
			throw std::runtime_error("This command cannot be used if the Window has no associated ImGui context.");
		}

		if (m_is_imguiCanvas_active)
		{
			throw std::runtime_error("Unexpected state. The canvas is already active.");
		}
	#endif

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_is_imguiCanvas_active = true;
}

void Window::display(MainLoopData & mainLoop_data)
{
	#if DYNAMIC_ASSERTS
		assert_open();
		assert_active();
	#endif

	if (m_has_imguiContext && m_is_imguiCanvas_active)
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 

		m_is_imguiCanvas_active = false;
	}

	mainLoop_data.rendering_end();

	mainLoop_data.swap_begin();

	if (m_max_fps != 0)
	{
		auto const frame_duration = std::chrono::nanoseconds{ 1000000000ll / m_max_fps }; //in nanoseconds
		auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - m_previous_display_time);
		while (elapsed_time <= frame_duration)
		{
			//std::this_thread::sleep_for(frame_duration - elapsed_time);
			elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - m_previous_display_time);
		}
		m_previous_display_time = std::chrono::system_clock::now();
	}
	glfwSwapBuffers(m_handler);

	mainLoop_data.swap_end();
}

void Window::assert_active() const
{
	if (m_id != windows_manager().activeWindow_id())
	{
		if(windows_manager().activeWindow_id())
			std::cout << "The active window was " << windows_manager().activeWindow_id().value() << std::endl;
		else
			std::cout << "There was no window bound." << std::endl;

		throw std::runtime_error("Unexpected state. This Window should be the active one.");
	}
}




WindowManager::WindowManager()
{
	// Create and open the main window

	create_window();
	if (m_windows.size() - 1 != mainWindow_id)
		throw std::runtime_error("The main window must be the first element of m_windows.");


	WindowOptions options{};

	options.title = "Evolving City Generation";
	//options.max_fps = 60;

	if (GSet::video_mode.fullscreen())
	{
		options.fullscreen = true;
		options.width = GSet::video_mode.width();
		options.height = GSet::video_mode.height();
	}
	else //window-mode
	{
		options.fullscreen = false;

		auto const current_mode = g_glfw.video_mode();
		options.width = current_mode->width / 10 * 7;
		options.height = current_mode->height / 10 * 8;
	}

	options.create_imguiContext = true;

	open_window(mainWindow_id, options);
}

WindowManager::~WindowManager()
{
	close_allWindows();
}


auto WindowManager::create_window() -> Window &
{
	std::cout << "old m_windows size: " << m_windows.size() << std::endl;
	m_windows.emplace_back();
	m_windows.back().m_id = m_windows.size() - 1;

	std::cout << "All windows: " << std::endl;
	for (auto & window : m_windows)
	{
		std::cout << window << std::endl;
	}

	return m_windows.back();
}

auto WindowManager::open_window(WindowId const wid, WindowOptions const& opt) -> Window &
{
	std::cout << "Open window id: " << wid << " -- Title: " << opt.title << std::endl;

	#if DYNAMIC_ASSERTS
		assert_wid(wid);
		assert_closed(wid);
	#endif

	auto & window = m_windows[wid];



	GLFWmonitor * monitor = nullptr;
	if (opt.fullscreen)
	{
		monitor = glfwGetPrimaryMonitor();
	}
	else
	{
		//glfwWindowHint(GLFW_MAXIMIZED , GL_TRUE);
	}

	GLFWwindow * shared_context = nullptr;
	if (opt.shared_context)
	{
		shared_context = m_windows[opt.shared_context.value()].m_handler;
	}

	set_contextCreationHints();
	glfwWindowHint(GLFW_RESIZABLE, opt.resizable);
	glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);

	auto handler = glfwCreateWindow(opt.width, opt.height, opt.title.c_str(), monitor, shared_context);
	if (!handler)
	{
		throw std::runtime_error("Window or OpenGL context creation failed");
	}
	glfwSetWindowPos(handler, opt.starting_pos.x, opt.starting_pos.y);


	window.m_handler = handler;



	glfwMakeContextCurrent(handler);
	m_activeWindow_id = wid;

	window.m_max_fps = opt.max_fps;
	auto const should_enable_vsync = opt.vsync && opt.max_fps == 0;
	glfwSwapInterval(should_enable_vsync); // Enables VSync


	// Clear held keys of all other windows (otherwise they would never receive the released_key event)
	for (auto& w : m_windows)
	{
		if (w.id() != wid)
		{
			w.release_all_keys();
		}
	}


	// Setup ImGui context
	if(opt.create_imguiContext)
	{
		if (!gladLoadGL()) //Necessary to setup imgui
		{
			throw std::runtime_error("Failed to initialize GLAD");
		}

		// Decide GL+GLSL versions
		#if __APPLE__
			// GL 3.2 + GLSL 150
			const char* glsl_version = "#version 150";
		#else
			// GL 3.0 + GLSL 130
			const char* glsl_version = "#version 130";
		#endif

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		window.m_imgui_context = ImGui::CreateContext();
		ImGui::SetCurrentContext(window.m_imgui_context);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window.m_handler, false);
		ImGui_ImplOpenGL3_Init(glsl_version);


		window.m_has_imguiContext = true;
	}

	return window;
}

void WindowManager::set_contextCreationHints()
{
	// Use OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	/*
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);*/


	// Note: GLFW MSAA hint comes with multiple issues:
	//		 1) NVIDIA control panel (and graphics driver in general) may override this settings to an arbitrary value
	//		 2) This hint affect only the default FBO. When postprocessing is enabled then the default FBO multisampling
	//			doesn't affect the actual scene rendering.
	#if ALPHA_TO_COVERAGE && !POSTPROCESSING
		glfwWindowHint(GLFW_SAMPLES, GSet::samples);
	#endif

	glfwWindowHint(GLFW_DEPTH_BITS, GSet::depthBuffer_bits);
}

auto WindowManager::get_window(WindowId const wid) -> Window &
{
	#if DYNAMIC_ASSERTS
		assert_wid(wid);
	#endif

	return m_windows[wid];
}

auto WindowManager::activate_window(WindowId const wid) -> Window &
{
	#if DYNAMIC_ASSERTS
		assert_wid(wid);
		assert_open(wid);
	#endif


	if (m_activeWindow_id != wid)
	{
		auto & new_window = m_windows[wid];
		glfwMakeContextCurrent(new_window.m_handler);

		if (new_window.m_has_imguiContext)
		{
			ImGui::SetCurrentContext(new_window.m_imgui_context);
		}

		m_activeWindow_id = wid;
	}

	//std::cout << "Activated window " << wid << std::endl;

	return m_windows[m_activeWindow_id.value()];
}


void WindowManager::close_window(WindowId const wid)
{
	#if DYNAMIC_ASSERTS
		assert_wid(wid);
		assert_open(wid);
	#endif	
		
	activate_window(wid);

	auto & window = m_windows[wid];

	// Destroy the associated ImGuiContext and the contexts of the related implementations.
	if (window.m_has_imguiContext)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		window.m_imgui_context = nullptr;
	}



	glfwDestroyWindow(window.m_handler);
	window.m_handler = nullptr;

	m_activeWindow_id = std::nullopt;
}

void WindowManager::close_allWindows()
{
	for (auto & window : m_windows)
	{
		if (window.is_open())
		{
			window.close();
		}
	}
}


void WindowManager::set_callbacks(WindowId const wid,
								  Window::KeyCallback pressed_key, Window::KeyCallback held_key, Window::KeyCallback released_key,
								  Window::CursorPosCallback cursor_pos, Window::MouseButtonCallback mouse_button, Window::MouseScrollCallback scroll,
								  Window::CharCallback char_clbk,
								  Window::FramebufferSizeCallback framebuffer_size, Window::WindowSizeCallback window_size)
{
	#if DYNAMIC_ASSERTS
		assert_wid(wid);
		assert_open(wid);
	#endif

	auto & w = m_windows[wid];


	// Set internal callbacks that will call the user-defined callbacks. They are set only if there's a real need, i.e.
	// if the user-defined callbacks aren't null or if there is an active ImGui context that need to capture those callbacks.

	if (pressed_key || held_key || released_key || w.m_has_imguiContext) 
	{
		glfwSetKeyCallback(w.m_handler, internal_keyCallback);
	}
	else
	{
		glfwSetKeyCallback(w.m_handler, nullptr);
	}

	if (cursor_pos) //ImGui doesn't use this callback
	{
		std::cout << "Window " << w.m_handler << " has installed cursorPos_callback" << std::endl;
		glfwSetCursorPosCallback(w.m_handler, internal_cursorPosCallback);
	}
	else
	{
		glfwSetCursorPosCallback(w.m_handler, nullptr);
	}

	if (mouse_button || w.m_has_imguiContext)
	{
		glfwSetMouseButtonCallback(w.m_handler, internal_mouseButtonCallback);
	}
	else
	{
		glfwSetMouseButtonCallback(w.m_handler, nullptr);
	}

	if (scroll || w.m_has_imguiContext)
	{
		glfwSetScrollCallback(w.m_handler, internal_mouseScrollCallback);
	}
	else
	{
		glfwSetScrollCallback(w.m_handler, nullptr);
	}

	if (char_clbk || w.m_has_imguiContext)
	{
		glfwSetCharCallback(w.m_handler, internal_charCallback);
	}
	else
	{
		glfwSetCharCallback(w.m_handler, nullptr);
	}

	if (framebuffer_size) //ImGui doesn't use this callback
	{
		glfwSetFramebufferSizeCallback(w.m_handler, internal_framebufferSizeCallback);
	}
	else
	{
		glfwSetFramebufferSizeCallback(w.m_handler, nullptr);
	}

	if (window_size) //ImGui doesn't use this callback
	{
		glfwSetWindowSizeCallback(w.m_handler, internal_windowSizeCallback);
	}
	else
	{
		glfwSetWindowSizeCallback(w.m_handler, nullptr);
	}

	// Set user-defined callbacks
	w.m_pressedKey_callback		 = pressed_key;
	w.m_heldKey_callback		 = held_key;
	w.m_releasedKey_callback	 = released_key;
	w.m_cursorPos_callback		 = cursor_pos;
	w.m_mouseButton_callback	 = mouse_button;
	w.m_mouseScroll_callback	 = scroll;
	w.m_char_callback			 = char_clbk;
	w.m_framebufferSize_callback = framebuffer_size;
	w.m_windowSize_callback		 = window_size;
}

void WindowManager::internal_keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	auto & w = windows_manager().get_window_byHandler(window);

	// Process input callbacks only if this is the active window
	if (w.m_id == windows_manager().m_activeWindow_id)
	{
		// Being this the active window, the correct ImGui context should be already set

		if (w.m_has_imguiContext)
		{
			ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
		}


		auto& imgui_io = ImGui::GetIO();

		if (action == GLFW_PRESS && !imgui_io.WantCaptureKeyboard)
		{
			w.m_pressed_keys.insert(key);
		}
		else if (action == GLFW_RELEASE && !imgui_io.WantCaptureKeyboard)
		{
			w.m_released_keys.insert(key);
		}
	}
}

void WindowManager::internal_cursorPosCallback(GLFWwindow* window, double x_pos, double y_pos)
{
	auto & w = windows_manager().get_window_byHandler(window);

	#if DYNAMIC_ASSERTS
		if(!w.m_cursorPos_callback) { throw std::runtime_error("Unexpected state. This callback should be invoked only when an user-defined CursorPosCallback is defined."); }
	#endif

	// Process input callbacks only if this is the active window
	if (w.m_id == windows_manager().m_activeWindow_id)
	{
		// Being this the active window, the correct ImGui context should be already set		

		auto & imgui_io = ImGui::GetIO();

		if (!imgui_io.WantCaptureMouse)
		{
			w.m_cursorPos_callback(w, { static_cast<float>(x_pos), static_cast<float>(y_pos) });
		}
	}
}

void WindowManager::internal_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto & w = windows_manager().get_window_byHandler(window);

	// Process input callbacks only if this is the active window
	if (w.m_id == windows_manager().m_activeWindow_id)
	{
		// Being this the active window, the correct ImGui context should be already set

		if (w.m_has_imguiContext)
		{
			ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
		}


		auto & imgui_io = ImGui::GetIO();

		if (w.m_mouseButton_callback && !imgui_io.WantCaptureMouse)
		{
			auto cx = 0.,
				 cy = 0.;
			glfwGetCursorPos(window, &cx, &cy);


			w.m_mouseButton_callback(w, { static_cast<float>(cx), static_cast<float>(cy) }, button, action, mods);
		}
	}
}

void WindowManager::internal_mouseScrollCallback(GLFWwindow * window, double x_offset, double y_offset)
{
	auto & w = windows_manager().get_window_byHandler(window);

	// Process input callbacks only if this is the active window
	if (w.m_id == windows_manager().m_activeWindow_id)
	{
		// Being this the active window, the correct ImGui context should be already set

		if (w.m_has_imguiContext)
		{
			ImGui_ImplGlfw_ScrollCallback(window, x_offset, y_offset);
		}


		auto & imgui_io = ImGui::GetIO();

		if (w.m_mouseScroll_callback && !imgui_io.WantCaptureMouse)
		{
			w.m_mouseScroll_callback(w, static_cast<float>(x_offset), static_cast<float>(y_offset));
		}
	}
}

void WindowManager::internal_charCallback(GLFWwindow* window, unsigned int c)
{
	auto & w = windows_manager().get_window_byHandler(window);

	// Process input callbacks only if this is the active window
	if (w.m_id == windows_manager().m_activeWindow_id)
	{
		// Being this the active window, the correct ImGui context should be already set

		if (w.m_has_imguiContext)
		{
			ImGui_ImplGlfw_CharCallback(window, c);
		}


		auto & imgui_io = ImGui::GetIO();

		if (w.m_char_callback && !imgui_io.WantCaptureKeyboard)
		{
			w.m_char_callback(w, c);
		}
	}
}

void WindowManager::internal_framebufferSizeCallback(GLFWwindow * window, int new_width, int new_height)
{
	auto & w = windows_manager().get_window_byHandler(window);

	windows_manager().assert_active(w.m_id); //Often in the framebufferSizeCallback are executed OpenGL operations, so we must be sure that @window is the active window.
	
	#if DYNAMIC_ASSERTS
		if(!w.m_framebufferSize_callback)
			throw std::runtime_error("Unexpected state. This callback should be invoked only when an user-defined FramebufferSizeCallback is defined.");
	#endif

	if (w.m_user_pointer.has_value())
	{
		w.m_framebufferSize_callback(w, {new_width, new_height});
	}
}

void WindowManager::internal_windowSizeCallback(GLFWwindow * window, int new_width, int new_height)
{
	auto & w = windows_manager().get_window_byHandler(window);

	windows_manager().assert_active(w.m_id); //Often in the framebufferSizeCallback are executed OpenGL operations, so we must be sure that @window is the active window.

	#if DYNAMIC_ASSERTS
		if (!w.m_windowSize_callback)
			throw std::runtime_error("Unexpected state. This callback should be invoked only when an user-defined WindowSizeCallback is defined.");
	#endif

	if (w.m_user_pointer.has_value())
	{
		w.m_windowSize_callback(w, { new_width, new_height });
	}
}



auto windows_manager() -> WindowManager &
{
	static WindowManager wmgr;

	return wmgr;
}




auto operator<<(std::ostream & os, Window const& w) -> std::ostream &
{
	os << "Window{ id: " << w.m_id << ", m_handler: " << w.m_handler << " }";

	return os;
}



} //namespace tgm