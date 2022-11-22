#ifndef GM_TILE_SET_HH
#define GM_TILE_SET_HH

#include <sstream>
#include <memory>

#include <flatbuffers/flatbuffers.h>

#include "mediators/queues/door_ev.hh"
#include "mediators/tile_graphics_mediator.hh"
#include "graphics/tile_vertices.hh"
#include "map/tiles/border_type.hh"
#include "map/tiles/tile.hh"
#include "map/tiles/tile_set.hh"
#include "io/flatbuffers/tileset_generated.h"

#include "settings/debug/debug_settings.hh"


namespace tgm
{



enum class BorderDoorType
{
	None,			// the border isn't suitable for a door
	Internal,		// the border is suitable for a door linking two areas of the same building
	BuildingsLinker,	// the border is suitable for a door linking two areas of two different buildings
	External,		// the border is suitable for a door linking an area to the outside
};

auto operator<<(std::ostream & os, BorderDoorType const bdt) -> std::ostream &;


struct DoorInfo
{
	BorderDoorType type = BorderDoorType::None;
	BuildingAreaCompleteId area1{};
	BuildingAreaCompleteId area2{};				// Null for external doors.
};

auto operator<<(std::ostream & os, DoorInfo const di) -> std::ostream &;


class TileSet
{
	public:
		//TODO: NOW: Forse andrebbe unificato il costruttore e reset(...) permettendo solo una lazy initialization.
		////
		//	N.B.: (@length * @width * @height) must be a multiple of GraphicsSettings::chunkSize_inTile.
		////
		TileSet(int const length, int const width, int const height, DoorEventQueues & door_events) : 
			m_length(length), m_width(width), m_height(height),	m_door_events(door_events)
		{
			auto const tile_count = static_cast<AT::size_type>(m_length) * static_cast<AT::size_type>(m_width) * static_cast<AT::size_type>(m_height); //cast in order to avoid overflows

			m_tileset = AT::allocate(m_alloc, tile_count);

			debug_generateDefaultTileset();
		}

		~TileSet()
		{
			try	{	//an exception mustn't escape a destructor
				free();
			}
			catch (std::exception const& e) {
				std::cout << "ERROR. Exception thrown when deallocating TileSet: " << e.what() << '\n';
			}
		}

		TileSet(TileSet const&) = delete;
		TileSet& operator=(TileSet const&) = delete;


		////
		//	N.B.: (@length * @width * @height) must be a multiple of GraphicsSettings::chunkSize_inTile.
		////
		void reset(int const length, int const width, int const height);

		auto length() const noexcept { return m_length; }
		auto width() const noexcept  { return m_width; }
		auto height() const noexcept { return m_height; }

		////
		//	Check if position is contained in an arbitrary plane of the tileset.
		////
		inline bool contains(Vector2i const pos) const noexcept
		{
			return pos.x >= 0 && pos.x < m_length 
				&& pos.y >= 0 && pos.y < m_width;
		}
		
		inline bool contains(Vector3i const& pos) const noexcept { return contains(pos.x, pos.y, pos.z); }
		inline bool contains(int const x, int const y, int const z) const noexcept
		{
			return x >= 0 && x < m_length 
				&& y >= 0 && y < m_width
				&& z >= 0 && z < m_height;
		}

		////
		//	Check if the parallelepiped of tiles is contained inside the tileset.
		////
		bool contains(int const top, int const left, int const down, int const length, int const width, int const height) const noexcept
		{
			return top >= 0  && top + length <= m_length
				&& left >= 0 && left + width <= m_width
				&& down >= 0 && down + height <= m_height;
		}
		
		auto get(Vector3i const pos) const noexcept -> Tile const* { return get(pos.x, pos.y, pos.z); }

		auto get(int const x, int const y, int const z) const noexcept -> Tile const*
		{
			if (!m_tileset || !contains(x, y, z))
			{
				return nullptr;
			}
			else
			{
				return m_tileset + (static_cast<AT::size_type>(z) * m_width * m_length + static_cast<AT::size_type>(y) * m_length + x); //static_cast to avoid int overflows
			}
		}
		
		auto get_existent(Vector2i const v, int const z) const -> Tile const& { return get_existent(v.x, v.y, z); }
		
		auto get_existent(Vector3i const& v) const -> Tile const& { return get_existent(v.x, v.y, v.z); }
		
		auto get_existent(int const x, int const y, int const z) const -> Tile const&
		{
			auto const t = get(x, y, z);

			#if DYNAMIC_ASSERTS
				if (!t)	{ throw std::runtime_error("Trying to access an unexistent tile.");	}
			#endif

			return *t;
		}

		void build_innerArea(int const x, int const y, int const z, CityBlockId const cbid, BuildingId const bid, BuildingAreaId const aid, TileType const new_style);
		void unbuild_innerArea(int const x, int const y, int const z, BuildingId const bid, BuildingAreaId const aid);
		
		void build_border(int const x, int const y, int const z, CityBlockId const cbid, BuildingId const bid, BuildingAreaId const aid, BorderStyle const style);
		////
		//	Note: It unbuilds only a border without a door. Before unbuilding the border call unbuild_door to check and possibly remove a door.
		////
		void unbuild_border(int const x, int const y, int const z, BuildingId const bid, BuildingAreaId const aid);
		
		void build_roof(BuildingId const bid, Vector3i const pos, RoofId const rid) { get_existentMutable(pos.x, pos.y, pos.z).build_roof(bid, rid); }
		void unbuild_roof(BuildingId const bid, Vector3i const pos) { get_existentMutable(pos.x, pos.y, pos.z).unbuild_roof(bid); }

		////
		//	@return: The type of door that the border can host. A BuildingCompleteAreaId of the other area adjacent to the provided one.
		////
		auto compute_borderDoorType(Vector3i const pos) const -> DoorInfo;

		void build_internalDoor(int const x, int const y, int const z, DoorId const did, TileType const tile_style);
		void build_externalDoor(int const x, int const y, int const z, DoorId const did, TileType const tile_style);
		////
		//	Try to unbuild a door in that position.
		//	@return: The DoorId of the hosted door or 0 if the tile doesn't host a door.
		////
		void unbuild_door(int const x, int const y, int const z);

		void open_door(Vector3i const pos) { get_existentMutable(pos.x, pos.y, pos.z).open_door(); };
		void close_door(Vector3i const pos) { get_existentMutable(pos.x, pos.y, pos.z).close_door(); };


		void add_mobile(Vector3i const pos) { get_existentMutable(pos.x, pos.y, pos.z).add_mobile(); }
		void remove_mobile(Vector3i const pos) { get_existentMutable(pos.x, pos.y, pos.z).remove_mobile(); }

		auto write(flatbuffers::FlatBufferBuilder & fbb) const -> flatbuffers::Offset<schema::TileSet>;
		void read(schema::TileSet const*const ts);

	private:
		int m_length = 0;
		int m_width = 0;
		int m_height = 0;

		using AT = std::allocator_traits<std::allocator<Tile>>;
		std::allocator<Tile> m_alloc;
		Tile * m_tileset = nullptr;
		
		DoorEventQueues & m_door_events;
		
		void free();
			  
		auto get_mutable(int const x, int const y, int const z) noexcept -> Tile *
		{
			if (!m_tileset || !contains(x, y, z))
			{
				return nullptr;
			}
			else
			{
				return m_tileset + (static_cast<AT::size_type>(z) * m_width * m_length + static_cast<AT::size_type>(y) * m_length + x); //static_cast to avoid int overflows
			}
		}

		auto get_existentMutable(int const x, int const y, int const z) -> Tile &
		{
			auto t = get_mutable(x, y, z);

			#if DYNAMIC_ASSERTS
				if (!t)	{ throw std::runtime_error("Trying to access an unexistent tile.");	}
			#endif

			return *t;
		}

		bool is_between_twoBorders(int const x, int const y, int const z) const;

		void debug_generateDefaultTileset();
		void debug_generateDefaultLevel(int const z, TileType const ttype);

		
	friend auto operator<<(std::ofstream & ofs, TileSet const& ts) -> std::ofstream &;
	friend auto operator>>(std::ifstream & ifs, TileSet & ts) -> std::ifstream &;
};



} //namespace tgm
using namespace tgm;


#endif //GM_TILE_SET_HH