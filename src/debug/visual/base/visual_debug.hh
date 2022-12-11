#ifndef GM_VISUAL_DEBUG_HH
#define GM_VISUAL_DEBUG_HH


#include "settings/debug/debug_settings.hh"
#if VISUALDEBUG


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <iterator>
#include <optional>

#include "graphics/color.hh"
#include "map/direction.h"
#include "map/tiles/tile_set.hh"
#include "polygon_highlighting.hh"
#include "rect_highlighting.hh"
#include "tiles_highlighting.h"
#include "settings/graphics_settings.hh"
#include "system/parallelepiped.hh"
#include "visual_debug_graphics_manager.hh"
#include "visual_debug_highlighting.hh"
#include "visual_debug_utilities.hh"
#include "window/glfw_wrapper.hh"
#include "window/window_manager.hh"


namespace tgm
{



////
//	Stores the original state when VisualDebug is started
////
class VisualDebugState
{
    public:
        VisualDebugState() = default;

        VisualDebugState(int const length, int const width, int const height) :
            m_map_length(length), m_map_width(width), m_map_height(height) { }

    private:
        int m_map_length = 0;
        int m_map_width = 0;
        int m_map_height = 0;
};

////
//	Store the changes that occurs in VisualDebug from a previous state to the next.
////
class VisualDebugChange
{
    private:
        int m_step_id = 0;

        IntParallelepiped m_old_frame;
        IntParallelepiped m_new_frame;

        std::string m_old_description;
        std::string m_new_description;

        std::map<HighlightingId, std::unique_ptr<VisualDebugHighlighting>> m_unhighlighted;
        std::map<HighlightingId, std::unique_ptr<VisualDebugHighlighting>> m_highlighted;

    public:
        VisualDebugChange(int step_id_, const std::string& old_desc, const std::string& new_desc) noexcept:
            m_step_id(step_id_), m_old_description (old_desc), m_new_description(new_desc) { }
        VisualDebugChange(VisualDebugChange && moved) noexcept(    std::is_nothrow_move_constructible_v<decltype(m_old_description)>
                                                                && std::is_nothrow_move_constructible_v<decltype(m_unhighlighted)  >  ) 
                                                                = default;

        auto old_frame() const noexcept -> IntParallelepiped { return m_old_frame; }
        auto new_frame() const noexcept -> IntParallelepiped { return m_new_frame; }

        auto old_description() const noexcept -> std::string { return m_old_description; }
        auto new_description() const noexcept -> std::string { return m_new_description; }

        auto unhighlighted() const noexcept -> std::map<HighlightingId, std::unique_ptr<VisualDebugHighlighting>> const& { return m_unhighlighted; }
        auto highlighted() const noexcept -> std::map<HighlightingId, std::unique_ptr<VisualDebugHighlighting>> const& { return m_highlighted; }


        void record_frameChange(IntParallelepiped const& oldf, IntParallelepiped const& newf)
        {
            if (m_old_frame.is_null())
                m_old_frame = oldf;

            m_new_frame = newf;
        }

        void record_unhighlighting(HighlightingId const hid, VisualDebugHighlighting const& vdh)
        {
            debug_unhighlightings_count += vdh.count();

            auto[it, success] = m_unhighlighted.insert({ hid, vdh.clone() }); //clone() since the reference could also contain a derived class object

            if (!success)
                throw std::runtime_error("VisualDebugChange: An unhighlighting with the same id was already recorded.");
        }

        void record_highlighting(HighlightingId const hid, VisualDebugHighlighting const& vdh)
        {
            debug_highlightings_count += vdh.count();

            auto[it, success] = m_highlighted.insert({ hid, vdh.clone() }); //clone() since the reference could also contain a derived class object

            if (!success)
                throw std::runtime_error("VisualDebugChange: An highlighting with the same id was already recorded.");
        }


    friend auto operator<<(Logger & os, VisualDebugChange const& vdc) -> Logger &;
};

class GameMap;

class VisualDebug
{
    private:
        enum class DebugLoopExitType
        {
            None,
            CloseWindow,		//close the VisualDebug window and resume main flow
            ResumeMainFlow,		//resume the main flow without closing the window
        };

    public:
        VisualDebug();

        ////
        //	Stop the debug recording.
        ////
        void stop();
        
        ////
        //	Start a new step and bookmark a new chapter.
        ////
        void begin_chapter(std::string const& descr, Vector3i const starting_cursorPos = Vector3i{});
        ////
        //	Show the last step, reset all the non-permanent changes and start a new step. 
        ////
        void end_chapter();

        bool has_chapter_begun() { return m_has_chapter_begun; }

        ////
        //	Show the last step and start a new one.
        //	@depth: Lower values indicate that the step is fundamental; higher values indicate that a step is accessorial. If the depth is higher than a certain
        //			runtime-defined value, the step won't be recorded.
        //
        ////
        void new_step(std::string const& description = std::string(), int const depth = 0);
        


        ////
        //	Highlight one tile (with description).
        //	Note: You should prefer highlight_tiles() when highlighting multiple tiles of the same color.
        //	@info: A message associated to the tile, that will be shown when cursor moves over it.
        //	@permanent: Indicate whether the tiles should remain highlighted also in the next steps.
        ////
        void highlight_tile(Vector3i const pos, Color const col, std::string const& info, bool const permanent = false);

        ////
        //	Highlight one tile.
        //	Note: You should prefer highlight_tiles() when highlighting multiple tiles with the same color.
        //	@permanent: Indicate whether the tiles should remain highlighted also in the next steps.
        ////
        void highlight_tile(Vector2i const& coo, int const z, Color const col, bool const permanent = false) { highlight_tile({ coo.x, coo.y, z }, col, permanent); }
        
        ////
        //	Highlight one tile (with description).
        //	Note: You should prefer highlight_tiles() when highlighting multiple tiles with the same color.
        //	@info: A message associated to the tile, that will be shown when cursor moves over it.
        //	@permanent: Indicate whether the tiles should remain highlighted also in the next steps.
        ////
        void highlight_tile(Vector2i const& coo, int const z, Color const col, std::string const& info, bool const permanent = false) { highlight_tile({ coo.x, coo.y, z }, col, info, permanent); }


        void highlight_tile(Vector3i const pos, Color const col, bool const permanent = false) { highlight_tile(pos, col, "", permanent); }
               
        ////
        //	Highlight multiple tiles.
        //	@beg, @end: Iterators to a container whose underlying type is either 'Vector3i' or 'TileHighlightingWithInfo'.
        //	@permanent: Indicate whether the tiles should remain highlighted also in the next steps.
        ////
        template <typename InputIt>
        void highlight_tiles(InputIt beg, InputIt end, Color const col, bool const permanent = false)
        {
            static_assert(std::is_convertible_v<typename std::iterator_traits<InputIt>::iterator_category, std::input_iterator_tag>,
                          "This function does require at least an input iterator");

            validate_inStepAction();
            if (!m_is_recording_enabled) { return; }

            //TODO: Add a check that prevents highlighting outside the map

            HighlightingId const max_hid = m_highlighted.size() == 0 ? -1 : m_highlighted.crbegin()->first;
            HighlightingId const new_hid = max_hid + 1;
            
            #if VISUALDEBUG_DEBUGLOG
                VDlog << Logger::nltb << "highlight tiles (hid: " << new_hid << ")";
            #endif			


            auto const ths = create_tilesHighlighting(new_hid, beg, end, col, permanent);
            
            m_highlighted.insert({ new_hid, ths.clone() });

            m_changes.back().record_highlighting(new_hid, ths);
        }

        ////
        //	Highlight multiple tiles.
        //	@beg, @end: Iterators to a container whose underlying type is 'Vector2i'.
        //	@permanent: Indicate whether the tiles should remain highlighted also in the next steps.
        ////
        template <typename InputIt>
        void highlight_tiles(InputIt beg, InputIt end, int const z, Color const col, bool permanent = false)
        {
                        static_assert(std::is_convertible_v<typename std::iterator_traits<InputIt>::iterator_category, std::input_iterator_tag>,
                          "This function do require at least an input iterators");
            
                        static_assert(std::is_same_v<typename std::iterator_traits<InputIt>::value_type, Vector2i>,
                          "This function can be only used with Vector2i");

            validate_inStepAction();
            if (!m_is_recording_enabled) { return; }

            //TODO: Add a check that prevents highlighting outside the map

            HighlightingId max_hid = m_highlighted.size() == 0 ? -1 : m_highlighted.crbegin()->first;
            HighlightingId new_hid = max_hid + 1;

            #if VISUALDEBUG_DEBUGLOG
                VDlog << Logger::nltb << "highlight tiles (hid: " << new_hid << ")";
            #endif

            std::vector<Vector3i> valid_positions;
            //eliminate possible unexistent positions
            for (auto it = beg; it != end; ++it)
            {
                if (is_inside_frame(*it, z))
                    valid_positions.push_back({ it->x, it->y, z });
            }

            TilesHighlighting ths(new_hid, std::move(valid_positions), col, permanent);

            m_highlighted.insert({ new_hid, ths.clone() });

            m_changes.back().record_highlighting(new_hid, ths);
        }


        ////
        //	Highlights all the tiles inside @rect.
        //	@rect: (in tiles -- map reference system)
        ////
        void highlight_tilesRect(IntRect const& rect, int const z, Color const col, bool const permanent = false);


        ////
        //	Highlights an IntRect of map pixels.
        //  @rect: (in units -- map reference system)
        ////
        void highlight_rect(FloatRect const rect, int const z, Color const col, bool const permanent = false);
        

        ////
        //	Highlights a polygon whose vertices are pixels.
        //	@vertices: (in units -- map reference system)
        //	@z_level: Level on which the polygon will be drawn (in tiles -- map reference system).
        ////
        void highlight_polygon(std::vector<Vector2f> const& vertices, int const z_level, Color const col, bool const permanent = false);

        ////
        //	Highlights a polygon whose vertices are tiles.
        //	@beg, @end: A range denoting vertices coordinates (in tiles -- map reference system). The z-coordinate is ignored. The polygon will lie on a single plane.
        //	@z_level: Level on which the polygon will be drawn (in tiles -- map reference system).
        ////
        template <typename InputIt>
        void highlight_tilePolygon(InputIt beg, InputIt end, int const z_level, Color const col, bool const permanent = false)
        {
                        static_assert(std::is_convertible_v<typename std::iterator_traits<InputIt>::iterator_category, std::input_iterator_tag>,
                          "This function do require at least an input iterators");
            
                        static_assert(   std::is_same_v<typename std::iterator_traits<InputIt>::value_type, Vector2i>
                                                  || std::is_same_v<typename std::iterator_traits<InputIt>::value_type, Vector3i> ,
                          "This function can be only used with Vector2i or Vector3i");

            if (!m_is_recording_enabled) { return; }

            std::vector<Vector2f> float_vertices;

            for (auto it = beg; it != end; ++it)
                float_vertices.push_back({ GSet::tiles_to_units(it->x + 0.5f), GSet::tiles_to_units(it->y + 0.5f) }); //from tile position (in tiles -- map reference system) to the center of the tile (in units -- map reference system)

            highlight_polygon(float_vertices, z_level, col, permanent);
        }



        void save(std::string message);
        

        auto const& debug_getChanges() { return m_changes; }


    protected:

        std::string m_chapter_alias; // Custom name for "chapter"
        
        ////
        //	Start the debug recording behind the scenes.
        ////
        void base_start(int const map_length, int const map_width, int const map_height);

        bool is_running() const noexcept { return m_running; }
        bool is_changing() const noexcept { return m_changing; }
        bool is_recordingEnabled() const noexcept { return m_is_recording_enabled; }

        bool is_last_state() const noexcept { return m_current_state == last_state(); }
        void validate_inChapterAction() const;
        void validate_inStepAction() const;

        ////
        //	Indicate whether the user has explicitly activated visual debug and want to see its window.
        ////
        bool has_been_activated() const noexcept { return visualDebug_runtime_openWindow && custom_has_been_activated(); }

        auto map_length() const noexcept -> int { return m_map_length; }
        auto map_width() const noexcept -> int { return m_map_width; }
        auto map_height() const noexcept -> int { return m_map_height; }
        void change_frame(IntParallelepiped const& newp) noexcept;
        auto frame() const noexcept -> IntParallelepiped { return m_frame; }

        auto compute_bestTileDimension() -> int;
        auto ppt() const noexcept { return m_ppt; }


        auto cursor_pos() const noexcept -> Vector3i { return m_cursor_pos; }


    private:
        bool m_running = false;
        bool m_has_chapter_begun = false;
        bool m_changing = false;
        bool m_is_recording_enabled = false;			// Indicate whether the actions of the current step have to be recorded.

        std::optional<WindowId> m_previousWindow_id;	//the active window before the debug window has been opened
        Window & m_window;
        Vector2f m_glfw_cursorPos;
        DebugLoopExitType m_debugLoop_exitType = DebugLoopExitType::None;	// Indicate whether should be resumed the main flow of the program and in which way

        bool m_is_display_changed = true;
        DebugVertices m_vertices;
        VisualDebugGraphicsManager m_graphics_mgr{ m_vertices };


        int m_map_length = 0;
        int m_map_width = 0;
        int m_map_height = 0;

        IntParallelepiped m_frame;
        int m_ppt = 0; //pixels per tile of the debug window
        

        int m_current_state = 0; //the currently represented state


        VisualDebugState m_original_state;
        std::vector<VisualDebugChange> m_changes;

        std::string m_description; //textual description of the current_state
        Vector3i m_cursor_pos;
        std::map<HighlightingId, std::unique_ptr<VisualDebugHighlighting>> m_highlighted; //highlighted entities in the current current_state and their ids


        int m_current_chapter = -1;
        //keys: state_id, values: chapter_id		bookmarks of the first state after the beginning of a chapter
        std::map<int, int> m_chapter_bookmarks; 
        //key: chapter_id -- value: chapter description
        std::map<int, std::string> m_chapter_descriptions;
        bool m_skip_currentChapter = false;



        void validate_navigation() const;

        virtual bool custom_has_been_activated() const noexcept = 0;
        virtual void custom_deactivate() const noexcept = 0;

        virtual void init_chapterAlias() = 0;
        virtual void custom_stop() = 0;

        ////
        //	Set the volume of the map that have to be displayed. Useful for focusing the debug on a particular area.
        ////
        void set_frame(IntParallelepiped const& p);
        bool is_inside_frame(Vector3i const pos) const noexcept { return m_frame.contains(pos); }
        bool is_inside_frame(Vector2i const plane_pos, int const z) const noexcept { return m_frame.contains({plane_pos.x, plane_pos.y, z}); }
        bool is_inside_map(Vector2i const pos) const noexcept 
        { 
            return pos.x >= 0 && pos.x < m_map_length 
                && pos.y >= 0 && pos.y < m_map_width; 
        }

        ////
        //	@z: (in tiles -- map reference system)
        ////
        bool is_z_inside_map(int const z) {	return z >= 0 && z < m_map_height; }

        ////
        //	@pos: (in units -- map reference system)
        ////
        bool is_inside_map(Vector2f const pos) const noexcept
        {
            return pos.x >= 0.f && pos.x < GSet::tiles_to_units(m_map_length)
                && pos.y >= 0.f && pos.y < GSet::tiles_to_units(m_map_width);
        }


        auto window_width() const noexcept -> int { return m_frame.width * m_ppt; }
        auto window_height() const noexcept -> int { return m_frame.length * m_ppt; }
        virtual auto window_title() const noexcept -> std::string = 0;

        void open_window();
        void close_window();

        ////
        //	Update the position of the cursor according to the mouse position.
        ////
        void move_cursor(Vector2f const glfw_cursorPos);
        void move_cursor(Vector3i const drc);

        int last_state() const;

        void begin_change(std::string const& descr);
        virtual void init_newChange(int const current_st) = 0;
        void end_change();

        void go_to_state(int const change_id);
        void go_to_previousStep();
        ////
        //	@change_id: Id of the change that identify the transition from the current to the previous state.
        ////
        virtual void custom_goToPreviousStep(int const change_id) = 0;
        void go_to_nextStep(); 
        ////
        //	@change_id: Id of the change that identify the transition from the current to the next state.
        ////
        virtual void custom_goToNextStep(int const change_id) = 0;

        int compute_currentChapter() const;

        
        void handle_go_to_previous_step_input();
        void handle_go_to_next_step_input();

        ////
        //	Go to the first state of the previous chapter.
        ////
        void go_to_previousChapter();
        ////
        //	Go to the first state of the next chapter. If this is the last chapter, go to the last state.
        ////
        void go_to_nextChapter();
        
        ////
        //	Display the current_state of visual debug. Open the visual debug window if necessary and possible.
        ////
        void debug_loop();

        void display_currentState();

        void push_terrain(DebugVertices & vertices) const;
        virtual void custom_pushVertices(DebugVertices & vertices) const = 0;
        void push_highlightings(DebugVertices & vertices) const;
        void push_cursor(DebugVertices & vertices) const;
        void generate_UI();

        template<typename T>
        struct dependent_false
        {
            static bool constexpr value = false;
        };

        template <typename InputIt>
        auto create_tilesHighlighting(HighlightingId const hid, InputIt beg, InputIt end, Color const col, bool const permanent) -> TilesHighlighting
        {
            // Remove possible unexistent positions
            std::vector<Vector3i> valid_positions;
            
            if constexpr (std::is_same_v<typename std::iterator_traits<InputIt>::value_type, Vector3i>)
            {
                for (auto it = beg; it != end; ++it)
                {
                    if (is_inside_frame(*it))
                        valid_positions.push_back(*it);
                }

                return TilesHighlighting{ hid, std::move(valid_positions), col, permanent };
            }
            else if constexpr (std::is_same_v<typename std::iterator_traits<InputIt>::value_type, TileHighlightingWithInfo>)
            {
                std::unordered_map<Vector3i, std::string> infos;

                for (auto it = beg; it != end; ++it)
                {
                    if (is_inside_frame(it->pos))
                    {
                        valid_positions.push_back(it->pos);

                        if(!it->info.empty())
                            infos.insert({ it->pos, it->info });
                    }
                }
                
                return TilesHighlighting{ hid, std::move(valid_positions), std::move(infos), col, permanent };
            }
            else
                static_assert(dependent_false<InputIt>::value, "'InputIt' must be an iterator whose underlying type is either 'Vector3i' or 'TileHighlightingWithInfo'");
        }


        static void pressedKey_callback(Window & window, int const key);

        static void heldKey_callback(Window & window, int const key);

        static void mouseButton_callback(Window & window, Vector2f const pos, int const button, int const action, int const mods);

        static void mouseScroll_callback(Window& window, float const x_offset, float const y_offset);

        static void framebufferSize_callback(Window & window, Vector2i const new_fbo_size);

        static void windowSize_callback(Window & window, Vector2i const new_window_size);
};



} //namespace tgm


#endif //VISUALDEBUG


#endif //GM_VISUAL_DEBUG_HH
