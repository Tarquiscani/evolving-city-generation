#include "visual_debug.hh"

#if VISUALDEBUG

#include <iomanip>
#include <sstream>
#include <algorithm>
#include <utility>
#include <filesystem>

#include "map/direction.h"
#include "map/tiles/tile.hh"
#include "map/gamemap.h"


namespace tgm
{



auto operator<<(Logger & lgr, VisualDebugChange const& vdc) -> Logger &
{
    lgr << Logger::addt
        << Logger::nltb << "step_id: " << vdc.m_step_id
        << Logger::nltb << "old_frame: " << vdc.m_old_frame
        << Logger::nltb << "new_frame: " << vdc.m_new_frame
        << Logger::nltb << "old_description: " << vdc.m_old_description
        << Logger::nltb << "new_description: " << vdc.m_new_description
        << Logger::nltb << "unhighlighted: " << vdc.m_unhighlighted
        << Logger::nltb << "highlighted: " << vdc.m_highlighted
        << Logger::remt;

    return lgr;
}


VisualDebug::VisualDebug():	
    m_window{ window_manager().create_window() }
{ 
    g_log << "Debug window after construction: " << m_window << std::endl;

}

void VisualDebug::base_start(int const map_length, int const map_width, int const map_height)
{
    if (m_running)
        throw std::runtime_error("Trying to start an already started VisualDebug.");

    m_running = true;

    m_map_length = map_length;
    m_map_width = map_width;
    m_map_height = map_height;

    m_original_state = VisualDebugState(m_map_length, m_map_width, m_map_height);

    set_frame(IntParallelepiped(0, 0, 0, m_map_length, m_map_width, m_map_height));
    

    m_current_state = 0;
    m_description = "Start of recording";

    init_chapterAlias();
}

void VisualDebug::stop()
{
    if (!m_running)
        throw std::runtime_error("Trying to stop a non-started VisualDebug.");

    m_cursor_pos = { 0, 0, 0 };
    m_highlighted.clear();
    m_changes.clear();
    m_chapter_bookmarks.clear();

    custom_stop();

    m_running = false;
}

int VisualDebug::last_state() const
{
    if (!m_running)
        throw std::runtime_error("Cannot compute the last state of a non-started VisualDebug.");

    if (m_changing)
    {
        #pragma warning (suppress: 4267)
        return m_changes.size() - 1;
    }
    else
    {
        if (m_changes.size() > 0)
        {
            #pragma warning (suppress: 4267)
            return m_changes.size();
        }
        else
        {
            return 0;
        }
    }
}


void VisualDebug::begin_chapter(std::string const& descr, Vector3i const starting_cursorPos)
{
    if (!m_running)
        throw std::runtime_error("Cannot begin a chapter in a non-started VisualDebug.");
    if (m_current_state != last_state())
        throw std::runtime_error("VisualDebug: When beginning a chapter it's expected that the state currently represented is the last one.");
    if (m_has_chapter_begun)
        throw std::runtime_error("VisualDebug: Cannot begin a new chapter when the last one hasn't been ended yet.");
    if (m_changing)
        throw std::runtime_error("VisualDebug: It is expected that the state isn't changing when a new chapter begins.");
    
    
    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::nltb << "Beginning of chapter"
              << Logger::addt;
    #endif

    m_has_chapter_begun = true;
    ++m_current_chapter;

    m_cursor_pos = starting_cursorPos;

    std::ostringstream oss;
    oss << "Beginning of " << m_chapter_alias << " " << m_current_chapter << " --- " << descr;
    begin_change(oss.str());
    

    // Remove all the highlightings of the previous chapter
    auto it = m_highlighted.cbegin();
    while (it != m_highlighted.cend())
    {
        const auto&[hid, vdh] = *it++;

        m_changes.back().record_unhighlighting(hid, *vdh);

        m_highlighted.erase(hid);
    }

    m_is_recording_enabled = true; // The actions recorded at the beginning of the chapter are always important.
}

void VisualDebug::end_chapter()
{
    if (!m_running)
        throw std::runtime_error("Cannot end a chapter in a non-started VisualDebug.");
    if (!is_last_state())
        throw std::runtime_error("VisualDebug: When ending a chapter it's expected that the state currently represented is the last one.");
    if (!m_changing)
        throw std::runtime_error("VisualDebug: It is expected that a change has already begun when a chapter ends.");
    if (!m_has_chapter_begun)
        throw std::runtime_error("VisualDebug: Cannot end a non-begun chapter.");

    end_change();
    debug_loop();

    if (m_window.is_open())
    {
        close_window();
    }

    m_has_chapter_begun = false;
    
    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::remt
              << Logger::nltb << "End chapter";
    #endif
        
    m_skip_currentChapter = false;
}

void VisualDebug::begin_change(std::string const& next_descr)
{
    if (!m_running)
        throw std::runtime_error("Cannot initialize a new step in a non-started VisualDebug.");
    if (m_changing)
        throw std::runtime_error("VisualDebug: Cannot begin a change when a change is already begun.");
    if (m_current_state != last_state())
        throw std::runtime_error("Cannot begin a change when the the state currently represented is not the last one.");

    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::nltb << "Begin change #" << m_current_state << " (" << next_descr << ")"
              << Logger::addt;
    #endif

    m_changing = true;

    //make a new Change
    m_changes.emplace_back(m_current_state, m_description, next_descr);
    init_newChange(m_current_state);

    m_description = next_descr;
    

    // Delete non-permanent highlightings
    auto it = m_highlighted.cbegin();
    while (it != m_highlighted.cend())
    {
        const auto&[hid, vdh] = *it++;

        if (!vdh)
            throw std::runtime_error("VisualDebug: Unexpected null highlightings.");

        if (!vdh->is_permanent())
        {
            m_changes.back().record_unhighlighting(hid, *vdh);

            m_highlighted.erase(hid);
        }
    }
}

void VisualDebug::end_change()
{
    if (!m_running)
        throw std::runtime_error("Cannot initialize a new step in a non-started VisualDebug.");
    if (!m_changing)
        throw std::runtime_error("VisualDebug: Cannot end a non-begun change.");

    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::remt
              << Logger::nltb << "End change #" << m_current_state << " (" << m_description << ")";
    #endif

    m_changing = false;

    ++m_current_state;

    // If the chapter bookmark hasn't been already registered
    auto it = m_chapter_bookmarks.crbegin();
    if (it == m_chapter_bookmarks.crend() || it->second != m_current_chapter)
    {
        m_chapter_bookmarks.insert({ m_current_state, m_current_chapter });
    }
}

//TODO: Make use of variadic templates to allow descr being whatever streamable arguments.
void VisualDebug::new_step(std::string const& descr, int const depth)
{
    if (!m_running)
        throw std::runtime_error("Cannot make a new step in a non-started VisualDebug.");
    if (!m_changing)
        throw std::runtime_error("VisualDebug: It is expected that a change has already begun when new_step() is called.");
    if (!m_has_chapter_begun)
        throw std::runtime_error("VisualDebug: cannot make a new step outside a chapter.");
    if (!is_last_state())
        throw std::runtime_error("VisualDebug: When creating a new step it's expected that the state currently represented is the last one.");

    
    if (depth > visualDebug_runtime_maxRecordableDepth)
    {
        m_is_recording_enabled = false;
    }
    else
    {
        m_is_recording_enabled = true;

        end_change();
        debug_loop();
        begin_change(descr);
    }
}

void VisualDebug::validate_inChapterAction() const
{
    if (!is_running())
        throw std::runtime_error("Cannot execute an action in a non-started VisualDebug.");
    if (!is_last_state())
        throw std::runtime_error("VisualDebug: Cannot execute an action in an old state.");
    if (!is_changing())
        throw std::runtime_error("VisualDebug: It is expected that a change has already begun when executing an in-chapter action.");
    if (!m_has_chapter_begun)
        throw std::runtime_error("VisualDebug: It is expected that a chapter has already begun when executing an in-chapter action.");
}

void VisualDebug::validate_inStepAction() const
{
    validate_inChapterAction();
}

void VisualDebug::validate_navigation() const 
{
    if (!m_running)
        throw std::runtime_error("Cannot go to another state in a non-started VisualDebug.");
    if (m_changing)
        throw std::runtime_error("VisualDebug: It is expected that the state isn't when going to another state.");
}

void VisualDebug::change_frame(IntParallelepiped const& newp) noexcept
{
    validate_inStepAction();
    if (!m_is_recording_enabled) { return; }


    auto old_frame = m_frame;
    set_frame(newp);

    m_changes.back().record_frameChange(old_frame, m_frame);
}

void VisualDebug::set_frame(IntParallelepiped const& p)
{
    if (p.length < 1 || p.width < 1 || p.height < 1)
        throw std::runtime_error("Frames with zero or negative dimensions are not allowed.");
    
    #if DEBUGLOG_VISUALDEBUG_FRAMEINFOS
        VDlog << Logger::nltb << "Set Frame"
              << Logger::addt;
    #endif

    m_frame = p;
    m_ppt = compute_bestTileDimension();

    //g_log << "Debug Window before resizing: " << m_window << std::endl;

    // Adjust fbo dimension according to the new frame.
    if (m_window.is_open() && (m_window.fbo_size() != Vector2i{ window_width(), window_height() }))
    {
        //TODO: I should set FBO size, not window size (in GLFW they are different), but it's not possible to set FBO size.
        //		and in Windows OS they are the same thing and I'll keep with this partial solution. In the future I should
        //		find a more portable solution.
        m_window.set_size(Vector2i{ window_width(), window_height() });

        #if DEBUGLOG_VISUALDEBUG_FRAMEINFOS
            VDlog << Logger::nltb << "Window new size: " << Vector2i{ window_width(), window_height()};
        #endif
    }


    #if DEBUGLOG_VISUALDEBUG_FRAMEINFOS
        auto old_cursorPos = m_cursor_pos;
    #endif

    if (m_cursor_pos.x < m_frame.behind)
        m_cursor_pos.x = m_frame.behind;
    else if (m_cursor_pos.x > m_frame.front())
        m_cursor_pos.x = m_frame.front();
    
    if (m_cursor_pos.y < m_frame.left)
        m_cursor_pos.y = m_frame.left;
    else if (m_cursor_pos.y > m_frame.right())
        m_cursor_pos.y = m_frame.right();

    if (m_cursor_pos.z < m_frame.down)
        m_cursor_pos.z = m_frame.down;
    else if (m_cursor_pos.z > m_frame.up())
        m_cursor_pos.z = m_frame.up();

    #if DEBUGLOG_VISUALDEBUG_FRAMEINFOS
        VDlog << Logger::nltb << "frame: " << m_frame
              << Logger::nltb << "tile_dim: " << m_ppt
              << Logger::nltb << "old_cursorPos: " << old_cursorPos
              << Logger::nltb << "new_cursorPos: " << m_cursor_pos
              << Logger::remt;
    #endif
}

auto VisualDebug::compute_bestTileDimension() -> int
{
    if (!m_running)
        throw std::runtime_error("Cannot compute best tile dimension for a non-started VisualDebug.");

    auto const rectified_screenWidth = GSet::game_video_mode.width() / 10 * 9;
    auto const rectified_screenHeight = GSet::game_video_mode.height() / 10 * 9;


    auto const tileMaxDim_in_screenWidth = rectified_screenWidth / m_frame.width;
    auto const tileMaxDim_in_screenHeight = rectified_screenHeight / m_frame.length;


    #if DEBUGLOG_VISUALDEBUG_FRAMEINFOS
        VDlog << Logger::nltb << "rectified_screenWidth: " << rectified_screenWidth
              << Logger::nltb << "rectified_screenHeight: " << rectified_screenHeight
              << Logger::nltb << "frame_width: " << m_frame.width
              << Logger::nltb << "frame_length: " << m_frame.length
              << Logger::nltb << "tileMaxDim_in_screenWidth: " << tileMaxDim_in_screenWidth
              << Logger::nltb << "tileMaxDim_in_screenHeight: " << tileMaxDim_in_screenHeight;
    #endif

    auto ret = std::min(tileMaxDim_in_screenWidth, tileMaxDim_in_screenHeight);
        
    // In case we have a m_frame that is larger than the screen, then we might get '0' as tile dimension. But we can't render a visual debug window
    // with a "ppt" equal to zero. The minimum is '1'.
    return std::max(ret, 1); 
}



//////////////////////////

//  In-chapter actions  //

///////////////////////////

void VisualDebug::highlight_tile(Vector3i const pos, Color const col, std::string const& info, bool const permanent)
{
    validate_inStepAction();
    if (!m_is_recording_enabled) { return; }

    //TODO: Add a check that prevents highlighting outside the map

    HighlightingId max_hid = m_highlighted.size() == 0 ? -1 : m_highlighted.crbegin()->first;

    HighlightingId new_hid = max_hid + 1;
    TilesHighlighting th(new_hid, pos, col, info, permanent);

    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::nltb << "highlight tile (hid: " << new_hid << ")";
    #endif

    m_highlighted.insert({ new_hid, th.clone() });

    m_changes.back().record_highlighting(new_hid, th);
}


void VisualDebug::highlight_rect(FloatRect const rect, int const z_level, Color const col, bool const permanent)
{
    validate_inStepAction();
    if (!m_is_recording_enabled) { return; }

    if (   !is_z_inside_map(z_level)
        /*|| !is_inside_map(Vector2f{ rect.top, rect.left })			// For the PlayerMovementVisualDebug it's actually useful to
        || !is_inside_map(Vector2f{ rect.bottom(), rect.right() })*/)	// highlight rectangles outside of the map. 
    {
        throw std::runtime_error("Cannot highlight a rectangle that lies outside of the map.");
    }

    HighlightingId const max_hid = m_highlighted.size() == 0 ? -1 : m_highlighted.crbegin()->first;

    HighlightingId const new_hid = max_hid + 1;
    RectHighlighting const rh(new_hid, rect, z_level, col, permanent);

    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::nltb << "highlight rect (hid: " << new_hid << ")";
    #endif

    m_highlighted.insert({ new_hid, rh.clone() });

    m_changes.back().record_highlighting(new_hid, rh);
}

void VisualDebug::highlight_tilesRect(IntRect const& rect, int const z, Color const col, bool const permanent)
{
    if (!m_is_recording_enabled) { return; }

    std::vector<Vector3i> poss;

    for (int x = rect.top; x <= rect.bottom(); ++x)
        for (int y = rect.left; y <= rect.right(); ++y)
            poss.push_back({ x,y,z });

    highlight_tiles(poss.begin(), poss.end(), col, permanent);
}

void VisualDebug::highlight_polygon(std::vector<Vector2f> const& vertices, int const z_level, Color const col, bool const permanent)
{
    validate_inStepAction();
    if (!m_is_recording_enabled) { return; }


    if(!is_z_inside_map(z_level))
        throw std::runtime_error("Cannot highlight a polygon that lies outside of the map.");

    /*for (auto v : m_vertices)		Highlighting a polygon outside the map is actually useful for PlayerMovementVisualDebug
    {
        if(!is_inside_map(v))
            throw std::runtime_error("Cannot highlight a polygon with a vertex outside of the map.");
    }*/



    HighlightingId const max_hid = m_highlighted.size() == 0 ? -1 : m_highlighted.crbegin()->first;

    HighlightingId const new_hid = max_hid + 1;
    PolygonHighlighting const ph(new_hid, vertices, z_level, col, permanent);

    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::nltb << "highlight polygon (hid: " << new_hid << ")";
    #endif

    m_highlighted.insert({ new_hid, ph.clone() });

    m_changes.back().record_highlighting(new_hid, ph);
}



void VisualDebug::open_window()
{
    if (!m_running)
        throw std::runtime_error("Cannot open_window a window for a non-started VisualDebug.");
    if (!has_been_activated())
        throw std::runtime_error("Cannot open_window a window for VisualDebug when the user hasn't asked for it.");
    if (m_window.is_open())
        throw std::runtime_error("Reopening an already opened VisualDebug");
    if (m_skip_currentChapter)
        throw std::runtime_error("Cannot open the window when skip_currentChapter is active.");



    m_previousWindow_id = window_manager().activeWindow_id();

    WindowOptions opt;
    opt.title = window_title();
    opt.width = window_width();
    opt.height = window_height();
    opt.resizable = false;
    opt.starting_pos = { 200, 100 };
    opt.create_imguiContext = true;

    m_window.open(opt);

    m_window.set_callabacks(pressedKey_callback, heldKey_callback, nullptr, 
                                  nullptr, mouseButton_callback, mouseScroll_callback, 
                                  nullptr, 
                                  framebufferSize_callback, windowSize_callback);
    m_window.set_userPointer(this);


    m_graphics_mgr.init(m_window.fbo_size(), m_window.window_size());
}

void VisualDebug::close_window()
{
    if (!m_running)
        throw std::runtime_error("Trying to close_window a window of a non-started VisualDebug.");
    if (!m_window.is_open())
        throw std::runtime_error("Trying to close_window an already closed window for VisualDebug.");
    
    m_graphics_mgr.shutdown();
    m_window.close();

    if (m_previousWindow_id)
    {
        window_manager().activate_window(m_previousWindow_id.value());
    }
}

void VisualDebug::move_cursor(Vector2f const glfw_cursorPos)
{
    // Relative position of the tile in the frame (in pixels)
    auto const frame_pos = m_graphics_mgr.glfwScreenPixel_to_framePos(glfw_cursorPos, VDUtil::frame_to_pixels(m_frame, m_ppt));

    // Relative position of the tile in the frame (in tiles)
    auto const fx = static_cast<int>(frame_pos.x / m_ppt),
               fy = static_cast<int>(frame_pos.y / m_ppt);

    // Absolute position of the tile
    auto const map_x = fx + m_frame.behind,
               map_y = fy + m_frame.left;

    m_cursor_pos.x = map_x;
    m_cursor_pos.y = map_y;
}

void VisualDebug::move_cursor(Vector3i const drc)
{
    if (!m_running) { throw std::runtime_error("Cannot move the cursor of a non-started VisualDebug."); }
    if (!m_window.is_open()) { throw std::runtime_error("Cannot move the cursor of VisualDebug with a close_window window."); }

    auto const new_pos = m_cursor_pos + drc;

    if (is_inside_frame(new_pos)) { m_cursor_pos = new_pos; }
}

void VisualDebug::go_to_state(int const state_id)
{
    validate_navigation();

    if (state_id < 0)
        throw std::runtime_error("VisualDebug: Cannot go to a state lower than zero.");
    if (state_id > last_state())
        throw std::runtime_error("VisualDebug: Cannot go to a state beyond the last one.");

    if (m_current_state < state_id)
    {
        while (m_current_state != state_id)
            go_to_nextStep();
    }
    else if (m_current_state > state_id)
    {
        while (m_current_state != state_id)
            go_to_previousStep();
    }
}

void VisualDebug::go_to_previousStep()
{
    validate_navigation();

    if (m_current_state == 0)
        return; //a previous state doesn't exist

    #if VISUALDEBUG_DEBUGLOG
        VDlog << "\n\n"
              << Logger::nltb << "Going to previous step (#" << m_current_state - 1 << ')'
              << Logger::addt;
    #endif

    #pragma warning (suppress: 26451) //the operation below never overflows
    auto const& ch = m_changes[m_current_state - 1];

    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::nltb << "Base Change #" << m_current_state - 1 << ": " << ch;
    #endif

    auto const old_frame = ch.old_frame();
    if (!old_frame.is_null()) { set_frame(old_frame); }

    m_description = ch.old_description();

    for (auto const& [hid, vdh] : ch.highlighted())
    {
        if (!vdh) { throw std::runtime_error("VisualDebug: Unexpected null highlighting."); }

        if (m_highlighted.erase(hid) < 1) { throw std::runtime_error("VisualDebug: Cannot remove an highlighting when going to the previous step."); }
    }

    for (auto const& [hid, vdh] : ch.unhighlighted())
    {
        if (!vdh) { throw std::runtime_error("VisualDebug: Unexpected null highlighting."); }

        auto const [it, success] = m_highlighted.insert({ hid, vdh->clone() });
        if (!success) { throw std::runtime_error("VisualDebug: Cannot readd an unhighlighting tile when going to the previous step."); }
    }

    custom_goToPreviousStep(m_current_state - 1);

    --m_current_state;
    m_current_chapter = compute_currentChapter();

    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::nltb << "Computed current chapter: " << m_current_chapter
              << Logger::remt;
    #endif
}

void VisualDebug::go_to_nextStep()
{
    validate_navigation();

    if (m_current_state == last_state())
        throw std::runtime_error("VisualDebug: Cannot go to the next step if the current step is the last one.");

    #if VISUALDEBUG_DEBUGLOG
        VDlog << "\n\n"
              << Logger::nltb << "Going to next step (#" << m_current_state + 1 << ')'
              << Logger::addt;
    #endif
    auto const& ch = m_changes[m_current_state];

    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::nltb << "Base Change #" << m_current_state + 1 << ": " << ch;
    #endif

    auto const new_frame = ch.new_frame();
    if (!new_frame.is_null()) { set_frame(new_frame); }

    m_description = ch.new_description();

    //remove the highlightings removed during the change.
    for (auto const& [hid, vdh] : ch.unhighlighted())
    {
        if (!vdh) { throw std::runtime_error("VisualDebug: unexpected null highlighting."); }

        if (m_highlighted.erase(hid) < 1) { throw std::runtime_error("VisualDebug: Cannot remove a highlighting when going to the next step."); }
    }

    //add the highlightings added during the change.
    for (auto const& [hid, vdh] : ch.highlighted())
    {
        if (!vdh) { throw std::runtime_error("VisualDebug: unexpected null highlighting."); }

        auto const [it, success] = m_highlighted.insert({ hid, vdh->clone() });
        if (!success) { throw std::runtime_error("VisualDebug: Cannot add an highlighting when going to the next step."); }
    }

    custom_goToNextStep(m_current_state);

    ++m_current_state;
    m_current_chapter = compute_currentChapter();

    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::remt
              << Logger::nltb << "Computed current chapter: " << m_current_chapter << std::endl;
    #endif
}

int VisualDebug::compute_currentChapter() const
{
    if (!m_running)
        throw std::runtime_error("Cannot compute current chapter in a non-started VisualDebug.");
    if (m_changing)
        throw std::runtime_error("VisualDebug: It is expected that the state isn't changing when computing current chapter.");
    if (m_chapter_bookmarks.empty())
        throw std::runtime_error("VisualDebug: It's expected that chapter_bookmarks contains at least one value when this function is called.");


    auto it = m_chapter_bookmarks.lower_bound(m_current_state);

    if (it == m_chapter_bookmarks.cend())
    {
        --it; //it's fine since we already checked that m_chapter_bookmarks isn't empty
    }

    return it->second;
}

void VisualDebug::go_to_previousChapter()
{
    validate_navigation();


    auto it = std::find_if(m_chapter_bookmarks.crbegin(), m_chapter_bookmarks.crend(),
                           [this](const auto& p) { return p.first < m_current_state; });

    if(it != m_chapter_bookmarks.crend())
    {
        auto const chapter_firstState = it->first;
        if (chapter_firstState < 0)
            throw std::runtime_error("VisualDebug: It's not expected that the bookmark of a chapter is ahead of 0.");
        
        go_to_state(chapter_firstState);
    }
}

void VisualDebug::go_to_nextChapter()
{
    validate_navigation();

    
    auto it = std::find_if(m_chapter_bookmarks.cbegin(), m_chapter_bookmarks.cend(),
                           [this](const auto& p) { return p.first > m_current_state; });


    if (it != m_chapter_bookmarks.cend())
    {
        int chapter_firstState = it->first;
        if (chapter_firstState > last_state())
            throw std::runtime_error("VisualDebug: It's not expected that the bookmark of a chapter is beyond the last state.");

        go_to_state(chapter_firstState);
    }
    else
    {
        go_to_state(last_state());
    }
}

void VisualDebug::debug_loop()
{
    if (!m_running)
        throw std::runtime_error("Cannot start the debug_loop for a non-started VisualDebug.");
    if (m_changing)
        throw std::runtime_error("VisualDebug: It is expected that the state isn't changing when entering in debug_loop.");

    if (!has_been_activated()) //if the visualDebug_runtime_openWindow is deactivated exit the debug_loop.
        return;

    if (m_skip_currentChapter) //if the user have explicitly asked to skip the view of this chapter
        return;



    if (!m_window.is_open())
        open_window();

    #if VISUALDEBUG_DEBUGLOG
        if (m_changes.size() > 0)
        {
            VDlog << Logger::addt
                  << Logger::nltb << "What changed (only base change): " << m_changes.back()
                  << Logger::remt;
        }
    #endif


    while (m_debugLoop_exitType == DebugLoopExitType::None)	// m_should_resumeMainFlow may also be modified by callbacks
    {
        display_currentState();

        m_window.poll_events();
    }


    // Additional actions specific for the exit type
    if (m_debugLoop_exitType == DebugLoopExitType::CloseWindow)
    {
        close_window();
        go_to_state(last_state());
    }
    // else m_debugLoop_exitType == DebugLoopExitType::ResumeMainFlow

    m_debugLoop_exitType = DebugLoopExitType::None; //reset, it has already served its purpose
}

void VisualDebug::display_currentState()
{
    if (!m_running) { throw std::runtime_error("Cannot display the map frame for a non-started VisualDebug."); }
    if (!m_window.is_open()) { throw std::runtime_error("Cannot display the map frame for a VisualDebug with a closed window."); }
    if (m_ppt == 0) { throw std::runtime_error("VisualDebug: the derived class should have set tile_dim at this point."); }

        
    #if VISUALDEBUG_DEBUGLOG
        VDlog << "\n\n"
              << Logger::nltb << "Displaying state: " << m_current_state
              << Logger::addt;
    #endif



    if (m_is_display_changed)
    {
        m_is_display_changed = false;

        m_vertices.reset( m_ppt, m_cursor_pos, m_frame,
                          m_frame.length * m_frame.width * 3 * 2		// 3 vertices per triangle, 2 triangle per tile
                          /*+*/ //TODO: Find a way to estimate how many vertices there will be in the final vector
                          + 4 * 2 );									//cursor rectangles

        push_terrain(m_vertices);
        custom_pushVertices(m_vertices);
        push_highlightings(m_vertices);
        push_cursor(m_vertices);
    }

    m_graphics_mgr.draw();

    m_window.activate_imguiCanvas();
    generate_UI();
        

    m_window.display();


    #if VISUALDEBUG_DEBUGLOG
        VDlog << Logger::remt;
    #endif
}

void VisualDebug::push_terrain(DebugVertices & vertices) const
{
    auto const map_rect = FloatRect{ 0.f, 0.f, GSet::tiles_to_units(m_map_length), GSet::tiles_to_units(m_map_width) };
    vertices.push_rectangle(map_rect, m_cursor_pos.z, 0, Color::White);

    //TODO: Use this chunk of code to push particular kind of terrains (e.g. water)
    ////Push the visisble terrain tiles
    //for (int fx = 0; fx < m_frame.length; ++fx)		//fx and fy are the relative coordinates of the tile in the frame.
    //{
    //	for (int fy = 0; fy < m_frame.width; ++fy)
    //	{
    //		int const x = m_frame.behind + fx,
    //				  y = m_frame.left + fy;

    //		Color tile_color;
    //		if (is_inside_map(Vector2i{ x, y }))
    //			tile_color = Color::White;
    //		else
    //			tile_color = Color::Black;

    //		vertices.push_tile(x, y, 0.f, tile_color);
    //	}
    //}
}

void VisualDebug::push_highlightings(DebugVertices & vertices) const
{
    // Highlight pieces of map (tiles, rectangles, polygons and more)
    for (auto const& [hid, ht] : m_highlighted)
        ht->push_vertices(vertices);
}

void VisualDebug::push_cursor(DebugVertices & vertices) const
{
    auto const color = Color::Red;

    vertices.push_tileOutline(m_cursor_pos.x, m_cursor_pos.y, 5.f, color);
}

void VisualDebug::generate_UI()
{
    static Clock fps_timer;
    std::ostringstream fps_oss; fps_oss << std::setprecision(0) << std::fixed << std::round(1 / fps_timer.getElapsedTime().asSeconds());
    fps_timer.restart();

    std::ostringstream pos_oss; pos_oss << m_cursor_pos;
    std::ostringstream step_oss; step_oss << m_current_state;
    std::ostringstream chp_oss; chp_oss << m_current_chapter;

    ImGui::SetNextWindowPos({ 10.f, 10.f }, ImGuiCond_Once);
    ImGui::Begin("Step informations", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowFontScale(GSet::imgui_scale());

    ImGui::Text("Fps:");		ImGui::SameLine(80.f * GSet::imgui_scale());	ImGui::Text("%s", fps_oss.str().data());
    ImGui::Text("Position:");	ImGui::SameLine(80.f * GSet::imgui_scale());	ImGui::Text("%s", pos_oss.str().data());
    ImGui::Text("Step:");		ImGui::SameLine(80.f * GSet::imgui_scale());	ImGui::Text("%s", step_oss.str().data());
    ImGui::Text("Chapter:");	ImGui::SameLine(80.f * GSet::imgui_scale());	ImGui::Text("%s", chp_oss.str().data());
    if (m_description.length() != 0)
    {
        ImGui::Text("Descr:");	ImGui::SameLine(80.f * GSet::imgui_scale());	ImGui::TextWrapped("%s", m_description.data());
    }

    ImGui::End();



    //Draw the infos associated to the selected tile
    std::vector<std::string> infos;
    for (const auto& [hid, ht] : m_highlighted)
    {
        if (ht->has_info(m_cursor_pos))
        {
            infos.push_back(ht->get_info(m_cursor_pos));
        }
    }

    if (!infos.empty())
    {
        std::ostringstream descr_oss;
        if (m_description.length() != 0)
        {
            descr_oss << m_description;
        }

        ImGui::SetNextWindowPos({ 10.f, window_height() - 300.f }, ImGuiCond_Once);
        ImGui::Begin("Tile informations", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        ImGui::SetWindowFontScale(GSet::imgui_scale());

        for (auto & info : infos)
        {
            ImGui::Text("%s", info.data());
        }

        ImGui::End();
    }
}

//TODO: Complete this function
void VisualDebug::save(std::string message)
{
    if (!m_running)
        throw std::runtime_error("Cannot save a non-started VisualDebug.");

    if (!std::filesystem::exists("_debug"))
        std::filesystem::create_directory("_debug");
    if (!std::filesystem::exists("_debug/screenshots"))
        std::filesystem::create_directory("_debug/screenshots");

    std::string filename;

    for (int i = 0;  ;++i)
    {
        std::ostringstream oss;
        oss << "_debug/screenshots/"<<i<<".screen";

        if (!std::filesystem::exists(oss.str()))
        {
            filename = oss.str();
            break;
        }
    }

    //g_log << filename << std::endl;
    std::ofstream file(filename);
    if (!file)
        throw std::runtime_error("Can't create a .screen file.");

    file << "prova" << std::endl;
}


void VisualDebug::pressedKey_callback(Window & window, int const key)
{
    auto & vd = *std::any_cast<VisualDebug *>(window.user_pointer()); //retrieve the VisualDebug object that triggered this callback

    auto const drc = DirectionUtil::key_to_unitVector(key);

    if (drc != Vector3i{0, 0, 0})
    {
        vd.move_cursor(drc);
        vd.m_is_display_changed = true;
    }
    else
    {
        switch (key)
        {
            case GLFW_KEY_B:
            {
                vd.handle_go_to_previous_step_input();
                break;
            }

            case GLFW_KEY_N:				//navigate to the next step or resume the flow of the program (without closing the window)
            {
                vd.handle_go_to_next_step_input();
                break;
            }
            case GLFW_KEY_V: 
            {
                vd.go_to_previousChapter();
                vd.m_is_display_changed = true;
                break;
            }

            case GLFW_KEY_M:				//navigate to the next chapter or skip the current chapter
            {
                vd.go_to_nextChapter();
                vd.m_is_display_changed = true;
                break;
            }

            case GLFW_KEY_Y:				//increase the visual debug depth
            {
                ++visualDebug_runtime_maxRecordableDepth;
                if (visualDebug_runtime_maxRecordableDepth > visualDebug_maxStepDepth) { visualDebug_runtime_maxRecordableDepth = 0; }

                g_log << "VisualDebug max recordable depth: " << visualDebug_runtime_maxRecordableDepth << std::endl;

                break;
            }

            case GLFW_KEY_ESCAPE:		//close the window and disable the debug mode
            {
                vd.custom_deactivate();

                vd.m_debugLoop_exitType = DebugLoopExitType::CloseWindow;
                vd.m_is_display_changed = true;

                break;
            }

            case GLFW_KEY_U:			//close the window and skip the current chapter
            {
                vd.m_skip_currentChapter = true;

                vd.m_debugLoop_exitType = DebugLoopExitType::CloseWindow;
                vd.m_is_display_changed = true;

                break;
            }
        }
    }
}


void VisualDebug::heldKey_callback(Window & window, int const key)
{
    auto & vd = *std::any_cast<VisualDebug *>(window.user_pointer()); //retrieve the VisualDebug object that triggered this callback

    static auto timer = Clock{};

    if (timer.getElapsedTime().asSeconds() > 0.04f)
    {
        timer.restart();

        switch (key)
        {
            case GLFW_KEY_H:
            {
                vd.handle_go_to_previous_step_input();
                break;
            }

            case GLFW_KEY_J:				//navigate to the next step or resume the flow of the program (without closing the window)
            {
                vd.handle_go_to_next_step_input();
                break;
            }
        }
    }
}

void VisualDebug::handle_go_to_previous_step_input()
{
    go_to_previousStep();
    m_is_display_changed = true;
}

void VisualDebug::handle_go_to_next_step_input()
{
    if (m_current_state == last_state())
    {
        m_debugLoop_exitType = DebugLoopExitType::ResumeMainFlow;
        m_is_display_changed = true;
    }
    else
    {
        go_to_nextStep();
        m_is_display_changed = true;
    }
}


void VisualDebug::mouseButton_callback(Window & window, Vector2f const pos, int const button, int const action, int const)
{
    auto & vd = *std::any_cast<VisualDebug *>(window.user_pointer()); //retrieve the VisualDebug object that triggered this callback

    if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
    {
        vd.move_cursor(pos);
        vd.m_is_display_changed = true;
    }
}

void VisualDebug::mouseScroll_callback(Window& window, float const, float const y_offset)
{
    auto& vd = *std::any_cast<VisualDebug*>(window.user_pointer()); //retrieve the VisualDebug object that triggered this callback

    if (y_offset < 0)
    {
        vd.go_to_previousStep();
        vd.m_is_display_changed = true;
    }
    else							//navigate to the next step or resume the flow of the program (without closing the window)
    {
            
        if (vd.m_current_state == vd.last_state())
        {
            vd.m_debugLoop_exitType = DebugLoopExitType::ResumeMainFlow;
                vd.m_is_display_changed = true;
        }
        else
        {
            vd.go_to_nextStep();
            vd.m_is_display_changed = true;
        }
    }
}

void VisualDebug::framebufferSize_callback(Window & window, Vector2i const new_fbo_size)
{
    auto & vd = *std::any_cast<VisualDebug *>(window.user_pointer()); //retrieve the VisualDebug object that triggered this callback

    vd.m_graphics_mgr.resize_fbo(new_fbo_size);
}

void VisualDebug::windowSize_callback(Window & window, Vector2i const new_fbo_size)
{
    auto & vd = *std::any_cast<VisualDebug *>(window.user_pointer()); //retrieve the VisualDebug object that triggered this callback

    vd.m_graphics_mgr.resize_window(new_fbo_size);
}



} //namespace tgm


#endif //VISUALDEBUG