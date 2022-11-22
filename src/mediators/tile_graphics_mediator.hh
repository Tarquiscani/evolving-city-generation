#ifndef GM_tile_graphics_mediator_HH
#define GM_tile_graphics_mediator_HH


#include <unordered_map>
#include <optional>

#include "std_extensions/hash_functions.hh"
#include "system/vector3.hh"
#include "system/parallelepiped.hh"
#include "map/tiles/tile.hh"


namespace tgm
{



class TileGraphicsMediator
{
	public:
		TileGraphicsMediator() = default;
		TileGraphicsMediator(TileGraphicsMediator const&) = delete;
		auto operator=(TileGraphicsMediator const&) -> TileGraphicsMediator & = delete;


		auto const& changes() const { return m_changes; }
		auto reset() const { return m_reset; }
		

		void record_borderChange(Vector3i const& pos) { m_changes[pos] = true; }
		void record_borderChange(int const x, int const y, int const z) { m_changes[{x, y, z}] = true; }
		void record_areaChange(IntParallelepiped const& vol);

		void record_reset() { m_reset = true; }


		void changes_acquired() { m_changes.clear(); }
		void reset_acquired() { m_reset = false; }


		auto const& debug_tileStyle_change() const { return m_debug_tileStyle_change; }
		void debug_record_tileStyleChange(Vector3i const pos, TileType const new_style) { m_debug_tileStyle_change.emplace(pos, new_style); }
		void debug_tileStyleChange_acquired() { m_debug_tileStyle_change.reset(); }

	private:
		bool m_reset = true;
		std::unordered_map<Vector3i, bool> m_changes{};							//tile changed (the bool indicate whether they were/are border)

		std::optional<std::pair<Vector3i, TileType>> m_debug_tileStyle_change{};
};



} //namespace tgm


#endif //GM_tile_graphics_mediator_HH