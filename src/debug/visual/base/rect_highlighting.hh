#ifndef GM_RECT_HIGHLIGHTING_HH
#define GM_RECT_HIGHLIGHTING_HH


#include "settings/debug/debug_settings.hh"
#if VISUALDEBUG


#include "system/rect.hh"
#include "visual_debug_highlighting.hh"


namespace tgm
{



class RectHighlighting final : public VisualDebugHighlighting
{
	public:
		RectHighlighting(HighlightingId const id, FloatRect const rect, int z_level, Color const col, bool const perm) :
			VisualDebugHighlighting(id, col, perm), m_rectangle(rect), m_z(z_level) { }

		virtual void push_vertices(DebugVertices & vertices) const override
		{
			#if VISUALDEBUG_DEBUGLOG
				VDlog << Logger::nltb << "Drawing RectHighlighting: " << m_rectangle << "(in units -- map RS)";
			#endif

			vertices.push_rectangle(m_rectangle, m_z, 4.f, color());
		}

		virtual auto count() const noexcept -> std::size_t override { return sizeof(*this); }

		virtual auto clone() const -> std::unique_ptr<VisualDebugHighlighting> override { return std::make_unique<RectHighlighting>(*this); }
		virtual auto print(Logger& lgr) const -> Logger& override
		{
			VisualDebugHighlighting::print(lgr)
				<< Logger::nltb << "rect: " << m_rectangle << "\tz:" << m_z;

			return lgr;
		}

	private:
		FloatRect m_rectangle;	//in units -- map reference system
		int m_z;
};



} // namespace tgm


#endif //VISUALDEBUG


#endif //GM_RECT_HIGHLIGHTING_HH
