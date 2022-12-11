#ifndef GM_VISUAL_DEBUG_HIGHLIGHTING_HH
#define GM_VISUAL_DEBUG_HIGHLIGHTING_HH


#include "settings/debug/debug_settings.hh"
#if VISUALDEBUG


#include <vector>

#include "debug_vertices.hh"
#include "graphics/color.hh"
#include "settings/graphics_settings.hh"
#include "system/parallelepiped.hh"

#include "debug/logger/debug_printers.hh"
#include "debug/logger/logger.hh"


namespace tgm
{



using HighlightingId = int;

class VisualDebugHighlighting
{
    public:
        VisualDebugHighlighting(HighlightingId const id, Color const color, bool const perm) :
            m_id(id), m_color(color), m_permanent(perm) { }

        virtual ~VisualDebugHighlighting() {}

        auto id() const noexcept -> HighlightingId { return m_id; }//TODO: Maybe useless
        auto color() const noexcept -> Color { return m_color; }
        bool is_permanent() const noexcept { return m_permanent; }


        ////
        //  Note: The design to pass the @tile_dim as an argument is required because the dimension of a debug window tile 
        //  can change independently from this highlighting.
        //	@tile_dim: Dimension of the tile in the current VisualDebug window (in pixels).
        ////
        virtual void push_vertices(DebugVertices & vertices) const = 0;
    
        #pragma warning (suppress: 4100) //It's perfectly fine that cursor_pos isn't referenced
        virtual bool has_info(Vector3i const cursor_pos) const noexcept { return false; }
        #pragma warning (suppress: 4100) //It's perfectly fine that cursor_pos isn't referenced
        virtual auto get_info(Vector3i const cursor_pos) const noexcept -> std::string { return ""; }

        ////
        //	How many variables this highlighting store.
        ////
        virtual auto count() const noexcept -> std::size_t = 0;

        virtual auto clone() const -> std::unique_ptr<VisualDebugHighlighting> = 0;
        virtual auto print(Logger & lgr) const -> Logger&
        {
            lgr << "Highlighting{"
                << Logger::addt
                    << Logger::nltb << "id: " << m_id
                    << Logger::nltb << "color: " << m_color
                    << Logger::nltb << "permanent: " << std::boolalpha << m_permanent << std::noboolalpha;

            return lgr;
        }

    private:
        HighlightingId m_id;//TODO: Maybe useless
        Color m_color;
        bool m_permanent = false;
};

auto operator<<(Logger & lgr, VisualDebugHighlighting const& th) -> Logger &;



} // namespace tgm


#endif //VISUALDEBUG


#endif //GM_VISUAL_DEBUG_HIGHLIGHTING_HH