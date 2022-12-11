#ifndef GM_TILES_HIGHLIGHTING_H
#define GM_TILES_HIGHLIGHTING_H


#include "settings/debug/debug_settings.hh"
#if VISUALDEBUG


namespace tgm
{



#include <unordered_map>

#include "std_extensions/hash_functions.hh"
#include "utilities.hh"
#include "visual_debug_highlighting.hh"

struct TileHighlightingWithInfo
{
    Vector3i pos;
    std::string info;
};
inline auto operator<<(Logger & lgr, TileHighlightingWithInfo const thli) -> Logger &
{
    lgr << "{" << thli.pos << ", '" << thli.info << "'}";

    return lgr;
}

class TilesHighlighting final : public VisualDebugHighlighting
{
    public:
        TilesHighlighting(HighlightingId const id, Vector3i const pos, Color const col, std::string const& info, bool perm) :
            VisualDebugHighlighting(id, col, perm)
        {
            m_positions.push_back(pos);

            if(!info.empty())
                m_infos.insert({ pos, info });
        }

        TilesHighlighting(HighlightingId const id, std::vector<Vector3i> && positions, Color const col, bool perm) :
            VisualDebugHighlighting(id, col, perm), m_positions{std::move(positions)} {}

        TilesHighlighting(HighlightingId const id, 
                            std::vector<Vector3i> && positions, 
                            std::unordered_map<Vector3i, std::string> && infos,
                            Color const col, 
                            bool const perm) :
            VisualDebugHighlighting(id, col, perm), m_positions{std::move(positions)}, m_infos{std::move(infos)} {}

        auto positions_beg() const noexcept -> std::vector<Vector3i>::const_iterator { return m_positions.cbegin(); }
        auto positions_end() const noexcept -> std::vector<Vector3i>::const_iterator { return m_positions.cend(); }
        auto get_positionsCount() const noexcept ->std::vector<Vector3i>::size_type { return m_positions.size(); }

        virtual void push_vertices(DebugVertices & vertices) const override
        {
            vertices.push_tiles(m_positions, 3.f, color());
        }

        virtual bool has_info(Vector3i const cursor_pos) const noexcept override { return m_infos.find(cursor_pos) != m_infos.cend(); }
        virtual auto get_info(Vector3i const cursor_pos) const noexcept -> std::string override 
        {
            auto it = m_infos.find(cursor_pos);
                
            return it != m_infos.cend() ? it->second : std::string{};
        }


        virtual auto count() const noexcept -> std::size_t override 
        { 
            return sizeof(*this) 
                    + sizeof(Vector3i) * m_positions.size() 
                    + (sizeof(Vector3i) + sizeof(std::string)) * m_infos.size(); //not a very accurate count but I suppose that strings are short
        }

        virtual auto clone() const -> std::unique_ptr<VisualDebugHighlighting> override { return std::make_unique<TilesHighlighting>(*this); }
        virtual auto print(Logger& lgr) const -> Logger& override
        {
            VisualDebugHighlighting::print(lgr)
                << Logger::nltb << "positions: " << m_positions
                << Logger::nltb << "infos: " << m_infos;

            return lgr;
        }

    private:
        std::vector<Vector3i> m_positions;
        std::unordered_map<Vector3i, std::string> m_infos;
};



} // namespace tgm


#endif //VISUALDEBUG


#endif //GM_TILES_HIGHLIGHTING_H