#ifndef GM_POLYGON_HIGHLIGHTING_HH
#define GM_POLYGON_HIGHLIGHTING_HH


#include "settings/debug/debug_settings.hh"
#if VISUALDEBUG


#include "settings/graphics_settings.hh"
#include "system/rect.hh"
#include "visual_debug_highlighting.hh"
#include "utilities.hh"


namespace tgm
{



class PolygonHighlighting final : public VisualDebugHighlighting
{
    public:
        ////
        //	@vertices: The vertices of the polygons, ordered either clockwise or counterclockwise (in units -- map reference system)
        ////
        PolygonHighlighting(HighlightingId const id, std::vector<Vector2f> const& vertices, int const z, Color const col, bool const perm) :
            VisualDebugHighlighting(id, col, perm), m_vertices(vertices), m_z(z) { }

        virtual void push_vertices(DebugVertices & vertices) const override
        {

            #if VISUALDEBUG_DEBUGLOG
                VDlog << Logger::nltb << "Drawing PolygonHighlighting:"
                      << Logger::addt;
            #endif


            for(decltype(m_vertices)::size_type i = 0; i < m_vertices.size(); ++i)
            {

                auto const v1 = m_vertices[i];
                auto v2 = Vector2f{};
                if (i + 1 == m_vertices.size())
                    v2 = m_vertices[0];
                else
                    v2 = m_vertices[i + 1];

                vertices.push_segment(v1, v2, m_z, 4.f, color());
            }
        
            #if VISUALDEBUG_DEBUGLOG
                VDlog << Logger::remt;
            #endif
        }

        virtual auto count() const noexcept -> std::size_t override { return sizeof(*this) + sizeof(decltype(m_vertices)::value_type) * m_vertices.size(); }

        virtual auto clone() const -> std::unique_ptr<VisualDebugHighlighting> override { return std::make_unique<PolygonHighlighting>(*this); }
        virtual auto print(Logger& lgr) const -> Logger& override
        {
            VisualDebugHighlighting::print(lgr)	<< Logger::nltb << "vertices: " << m_vertices 
                                                << Logger::nltb << "z: "		<< m_z;

            return lgr;
        }

    private:
        std::vector<Vector2f> m_vertices; //in units -- SFML reference system
        int m_z;
};



} // namespace tgm


#endif //VISUALDEBUG


#endif //GM_POLYGON_HIGHLIGHTING_HH
