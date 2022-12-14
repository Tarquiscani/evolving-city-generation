#include "window_manager.hh"


#include <thread>
#include <type_traits>

#include "window/glfw_wrapper.hh"				// Must be included before imgui_impl_*
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include "debug/asserts.hh"
#include "settings/debug/debug_settings.hh"
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
    window_manager().open_window(m_id, opt);
}

//TODO: PERFORMANCE: Maybe this function could be inlined.
void Window::activate()
{
    window_manager().activate_window(m_id);
}

//TODO: PERFORMANCE: Maybe this function could be inlined.
void Window::poll_events()
{
    exec_check(assert_active());

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
    window_manager().close_window(m_id);


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
    window_manager().set_callbacks(m_id, pressed_key, held_key, released_key, cursor_pos, mouse_button, scroll, char_callback, framebuffer_size, window_size);
}

#if ENABLE_IMGUI
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
#endif //ENABLE_IMGUI

void Window::display(MainLoopData & mainLoop_data)
{
    #if DYNAMIC_ASSERTS
        assert_open();
        assert_active();
    #endif

#if ENABLE_IMGUI
    if (m_has_imguiContext && m_is_imguiCanvas_active)
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 

        m_is_imguiCanvas_active = false;
    }
#endif //ENABLE_IMGUI

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
    if (m_id != window_manager().activeWindow_id())
    {
        if(window_manager().activeWindow_id())
            g_log << "The active window was " << window_manager().activeWindow_id().value() << std::endl;
        else
            g_log << "There was no window bound." << std::endl;

        throw std::runtime_error("Unexpected state. This Window should be the active one.");
    }
}




WindowManager::WindowManager()
{
    // Create and open the main window
    
    create_window();
    
    exec_check( if (m_windows.size() - 1 != mainWindow_id) { throw std::runtime_error("The main window must be the first element of m_windows."); } );


    WindowOptions options{};

    options.title = "Evolving City Generation";
    //options.max_fps = 60;

    if (GSet::game_video_mode.fullscreen())
    {
        options.fullscreen = true;
        options.width = GSet::game_video_mode.width();
        options.height = GSet::game_video_mode.height();
    }
    else // Window mode
    {
        options.fullscreen = false;

        auto const current_mode = g_glfw.video_mode();
        options.width = GSet::game_video_mode.width() / 10 * 7;
        options.height = GSet::game_video_mode.height() / 10 * 8;
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
    g_log << "old m_windows size: " << m_windows.size() << std::endl;
    m_windows.emplace_back();
    m_windows.back().m_id = m_windows.size() - 1;

    g_log << "All windows: " << std::endl;
    for (auto & window : m_windows)
    {
        g_log << window << std::endl;
    }

    return m_windows.back();
}

auto WindowManager::open_window(WindowId const wid, WindowOptions const& opt) -> Window &
{
    g_log << "Opening window -- wid: " << wid << " -- Title: " << opt.title << std::endl;

    exec_check( assert_wid(wid); assert_closed(wid); );



    auto & window = m_windows[wid];


    GLFWmonitor * monitor = nullptr;
    if (opt.fullscreen)
    {
        monitor = glfwGetPrimaryMonitor();
    }

    GLFWwindow * shared_context = nullptr;
    if (opt.shared_context)
    {
        shared_context = m_windows[opt.shared_context.value()].m_handler;
    }


    auto const should_enable_vsync = opt.vsync && opt.max_fps == 0;

    set_contextCreationHints();
    
    g_log << "Creating a new window - WxH: " << opt.width << "x" << opt.height 
          << " - RGB depth: " << "(" << GSet::game_video_mode.red_bits() << ", " << GSet::game_video_mode.green_bits() << ", " << GSet::game_video_mode.blue_bits() << ") - "
          << "refresh_rate: " << GSet::game_video_mode.refresh_rate() << " - vsync: " << std::boolalpha << should_enable_vsync << std::endl;
    
    glfwWindowHint(GLFW_RESIZABLE, opt.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);

    // Better to enforce this hints, because in certain circumstances the default values chosen by GLFW aren't supported by the monitor.
    glfwWindowHint(GLFW_RED_BITS, GSet::game_video_mode.red_bits());
    glfwWindowHint(GLFW_GREEN_BITS, GSet::game_video_mode.green_bits());
    glfwWindowHint(GLFW_BLUE_BITS, GSet::game_video_mode.blue_bits());
    glfwWindowHint(GLFW_REFRESH_RATE, GSet::game_video_mode.refresh_rate());

    auto handler = glfwCreateWindow(opt.width, opt.height, opt.title.c_str(), monitor, shared_context);
    if (!handler) {	throw std::runtime_error("Window or OpenGL context creation failed"); }

    glfwSetWindowPos(handler, opt.starting_pos.x, opt.starting_pos.y);


    window.m_handler = handler;



    glfwMakeContextCurrent(handler);
    m_activeWindow_id = wid;

    window.m_max_fps = opt.max_fps;
    glfwSwapInterval(should_enable_vsync ? 1 : 0); // Enables VSync
    
    
    // Clear held keys of all other windows (otherwise they would never receive the released_key event)
    for (auto& w : m_windows)
    {
        if (w.id() != wid)
        {
            w.release_all_keys();
        }
    }


#if ENABLE_IMGUI
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
#endif // ENABLE_IMGUI

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
    //		 2) This hint just affects the default FBO. And when postprocessing is enabled the default FBO multisampling
    //			doesn't affect the actual scene rendering.
    #if GSET_ALPHA_TO_COVERAGE && !GSET_POSTPROCESSING
        g_log << "Default framebuffer MSAA enabled" << std::endl;
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
        
#if ENABLE_IMGUI
        if (new_window.m_has_imguiContext)
        {
            ImGui::SetCurrentContext(new_window.m_imgui_context);
        }
#endif //ENABLE_IMGUI

        m_activeWindow_id = wid;
    }

    //g_log << "Activated window " << wid << std::endl;

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

#if ENABLE_IMGUI
    // Destroy the associated ImGuiContext and the contexts of the related implementations.
    if (window.m_has_imguiContext)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        window.m_imgui_context = nullptr;
    }
#endif //ENABLE_IMGUI


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
        g_log << "Window " << w.m_handler << " has installed cursorPos_callback" << std::endl;
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
    auto & w = window_manager().get_window_byHandler(window);

    // Process input callbacks only if this is the active window
    if (w.m_id == window_manager().m_activeWindow_id)
    {
        // Being this the active window, the correct ImGui context should be already set
        
        auto imgui_want_capture_keyboard = false;
#if ENABLE_IMGUI
        if (w.m_has_imguiContext)
        {
            ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
        }
        imgui_want_capture_keyboard = ImGui::GetIO().WantCaptureKeyboard;
#endif //ENABLE_IMGUI

        if (!imgui_want_capture_keyboard)
        {
            if (action == GLFW_PRESS)
            {
                w.m_pressed_keys.insert(key);
            }
            else if (action == GLFW_RELEASE)
            {
                w.m_released_keys.insert(key);
            }
        }
    }
}

void WindowManager::internal_cursorPosCallback(GLFWwindow* window, double x_pos, double y_pos)
{
    auto & w = window_manager().get_window_byHandler(window);

    #if DYNAMIC_ASSERTS
        if(!w.m_cursorPos_callback) { throw std::runtime_error("Unexpected state. This callback should be invoked only when an user-defined CursorPosCallback is defined."); }
    #endif

    // Process input callbacks only if this is the active window
    if (w.m_id == window_manager().m_activeWindow_id)
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
    auto & w = window_manager().get_window_byHandler(window);

    // Process input callbacks only if this is the active window
    if (w.m_id == window_manager().m_activeWindow_id)
    {
        // Being this the active window, the correct ImGui context should be already set
        
        auto imgui_want_capture_mouse = false;
#if ENABLE_IMGUI
        if (w.m_has_imguiContext)
        {
            ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        }
        imgui_want_capture_mouse = ImGui::GetIO().WantCaptureMouse;
#endif //ENABLE_IMGUI

        if (w.m_mouseButton_callback && !imgui_want_capture_mouse)
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
    auto & w = window_manager().get_window_byHandler(window);

    // Process input callbacks only if this is the active window
    if (w.m_id == window_manager().m_activeWindow_id)
    {
        // Being this the active window, the correct ImGui context should be already set
        
        auto imgui_want_capture_mouse = false;
#if ENABLE_IMGUI
        if (w.m_has_imguiContext)
        {
            ImGui_ImplGlfw_ScrollCallback(window, x_offset, y_offset);
        }
        imgui_want_capture_mouse = ImGui::GetIO().WantCaptureMouse;
#endif //ENABLE_IMGUI

        if (w.m_mouseScroll_callback && !imgui_want_capture_mouse)
        {
            w.m_mouseScroll_callback(w, static_cast<float>(x_offset), static_cast<float>(y_offset));
        }
    }
}

void WindowManager::internal_charCallback(GLFWwindow* window, unsigned int c)
{
    auto & w = window_manager().get_window_byHandler(window);

    // Process input callbacks only if this is the active window
    if (w.m_id == window_manager().m_activeWindow_id)
    {
        // Being this the active window, the correct ImGui context should be already set
        
        auto imgui_want_capture_keyboard = false;
#if ENABLE_IMGUI
        if (w.m_has_imguiContext)
        {
            ImGui_ImplGlfw_CharCallback(window, c);
        }
        imgui_want_capture_keyboard = ImGui::GetIO().WantCaptureKeyboard;
#endif //ENABLE_IMGUI

        if (w.m_char_callback && !imgui_want_capture_keyboard)
        {
            w.m_char_callback(w, c);
        }
    }
}

void WindowManager::internal_framebufferSizeCallback(GLFWwindow * window, int new_width, int new_height)
{
    // Often in the framebufferSizeCallback some OpenGL operations are executed, so we must ensure that the right window is activated before running any operation.
    run_in_right_window(window, 
                        [new_width, new_height](Window & w) {
                            #if DYNAMIC_ASSERTS
                                if (!w.m_framebufferSize_callback) { throw std::runtime_error("Unexpected state. This callback should be invoked only when there's a user-defined FramebufferSizeCallback."); }
                            #endif
    
                            if (w.m_user_pointer.has_value())
                            {
                                w.m_framebufferSize_callback(w, {new_width, new_height});
                            }
                        });
}

void WindowManager::internal_windowSizeCallback(GLFWwindow * window, int new_width, int new_height)
{
    // Often in the WindowSizeCallback some OpenGL operations are executed, so we must ensure that the right window is activated before running any operation.	
    run_in_right_window(window, 
                        [new_width, new_height](Window & w) {
                            #if DYNAMIC_ASSERTS
                            if (!w.m_windowSize_callback) { throw std::runtime_error("Unexpected state. This callback should be invoked only when there's user-defined WindowSizeCallback."); }
                            #endif

                            if (w.m_user_pointer.has_value())
                            {
                                w.m_windowSize_callback(w, { new_width, new_height });
                            }
                        });
}


void WindowManager::run_in_right_window(GLFWwindow * window, std::function<void(Window &)> body)
{
    auto & w = window_manager().get_window_byHandler(window);

    auto curr_active_window = window_manager().activeWindow_id();
    if (!curr_active_window) { throw std::runtime_error("Unexpected state. There should always be an active window."); }
    
    //Often in the framebufferSizeCallback are executed OpenGL operations, so we must be sure that @window is the active window.
    auto const should_replace_active_window = curr_active_window.value() != w.m_id;
    if (should_replace_active_window)
    {
        window_manager().activate_window(w.m_id);
        body(w);
        window_manager().activate_window(curr_active_window.value());
    }
    else
    {
        body(w);
    }
}



auto window_manager() -> WindowManager &
{
    static auto wmgr = WindowManager{};

    return wmgr;
}




auto operator<<(std::ostream & os, Window const& w) -> std::ostream &
{
    os << "Window{ id: " << w.m_id << ", m_handler: " << w.m_handler << " }";

    return os;
}



} //namespace tgm