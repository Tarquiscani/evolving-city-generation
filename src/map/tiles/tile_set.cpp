#include "tile_set.hh"


#include <bitset>

#include "map/buildings/building_area.hh"
#include "settings/simulation/simulation_settings.hh"

#include "debug/visual/player_movement_stream.hh"


namespace tgm
{



auto operator<<(std::ostream & os, BorderDoorType const bdt) -> std::ostream &
{
	switch (bdt)
	{
		case BorderDoorType::None:
			os << "None";
			break;
		case BorderDoorType::Internal:
			os << "Internal";
			break;
		case BorderDoorType::BuildingsLinker:
			os << "BuildingsLinker";
			break;
		case BorderDoorType::External:
			os << "External";
			break;
		default:
			throw std::runtime_error("Unexpected case.");
			break;
	}

	return os;
}

auto operator<<(std::ostream & os, DoorInfo const di) -> std::ostream &
{
	os << "DoorInfo{ type: " << di.type << "; area1: " << di.area1 << "; area2: " << di.area2 << " }";

	return os;
}

void TileSet::reset(int const length, int const width, int const height)
{
	free();

	

	m_length = length;
	m_width = width;
	m_height = height;
	
	auto const new_tileCount = static_cast<AT::size_type>(length) * static_cast<AT::size_type>(width) * static_cast<AT::size_type>(height); //cast needed to avoid overflows
	m_tileset = AT::allocate(m_alloc, new_tileCount);

	// Construct each tile with temporary values (the actual value is then read by an input file)
	for (auto z = 0; z < height; ++z)
	{
		for (auto y = 0; y < width; ++y)
		{
			for (auto x = 0; x < length; ++x)
			{
				auto t = get_mutable(x, y, z);
				AT::construct(m_alloc, t, Vector3i(x, y, z), TileType::sky);
			}
		}
	}
}

void TileSet::free()
{
	auto const tile_count = static_cast<AT::size_type>(m_length) * static_cast<AT::size_type>(m_width) * static_cast<AT::size_type>(m_height); //cast needed to avoid overflows
	for (auto p = m_tileset + tile_count; p != m_tileset; )
	{
		AT::destroy(m_alloc, --p);
	}

	AT::deallocate(m_alloc, m_tileset, tile_count);
}

void TileSet::build_innerArea(int const x, int const y, int const z, CityBlockId const cbid, BuildingId const bid, BuildingAreaId const aid, TileType const new_style)
{
	get_existentMutable(x, y, z).build_innerArea(cbid, bid, aid, new_style);
}
	
//TODO: Manage better the default TileType when unbuilding
void TileSet::unbuild_innerArea(int const x, int const y, int const z, BuildingId const bid, BuildingAreaId const aid)
{
	get_existentMutable(x, y, z).unbuild_innerArea(bid, aid);
}

void TileSet::build_border(int const x, int const y, int const z, CityBlockId const cbid, BuildingId const bid, BuildingAreaId const aid, BorderStyle const style)
{
	auto & t = get_existentMutable(x, y, z);
	t.build_border(cbid, bid, aid, style);


	#if PLAYERMOVEMENT_VISUALDEBUG
		if (t.borders_count() == 1) // add the impassable tile only if it wasn't already impassable
		{
			PMdeb.add_impassableTile(t.get_coordinates());
		}
	#endif
}


//TODO: Manage better the default TileType when unbuilding
void TileSet::unbuild_border(int const x, int const y, int const z, BuildingId const bid, BuildingAreaId const aid)
{
	auto & t = get_existentMutable(x, y, z);

	#if DYNAMIC_ASSERTS
		if (t.is_door()) { throw std::runtime_error("Cannot unbuild a border that is a door. Unbuild the door before."); }
	#endif

	t.unbuild_border(bid, aid);
	
	#if PLAYERMOVEMENT_VISUALDEBUG
		if (t.borders_count() == 0) // remove the impassable tile only if now there is no border 
			PMdeb.remove_impassableTile(t.get_coordinates());
	#endif
}

auto TileSet::compute_borderDoorType(Vector3i const pos) const -> DoorInfo
{
	auto dpos = DoorInfo{}; //NRVO
	dpos.type = BorderDoorType::None;

	auto const& t = get_existent(pos);

	//if (!t.is_border()) { throw std::runtime_error("It's not a border!"); }

	if (t.borders_count() == 2)
	{
		auto const N_tile = get(pos.x - 1, pos.y    , pos.z);
		auto const E_tile = get(pos.x	 , pos.y + 1, pos.z);
		auto const S_tile = get(pos.x + 1, pos.y	, pos.z);
		auto const W_tile = get(pos.x	 , pos.y - 1, pos.z);

		// Check if the neighbors form a suitable pattern
		if (N_tile && E_tile && S_tile && W_tile)
		{
			if (   (N_tile->is_border() && S_tile->is_border() && E_tile->is_innerArea() && W_tile->is_innerArea())
				|| (E_tile->is_border() && W_tile->is_border() && N_tile->is_innerArea() && S_tile->is_innerArea())   )
			{
				// Check that the tile link two areas of the same building and find the id of the other area
				auto const binfo = t.get_borderInfos();
				auto i1 = binfo[0];
				auto i2 = binfo[1];
				if (i1.bid() == i2.bid())
				{
					dpos.type = BorderDoorType::Internal;
					dpos.area1 = i1.acid();
					dpos.area2 = i2.acid();
				}
				else
				{
					dpos.type = BorderDoorType::BuildingsLinker;					
					dpos.area1 = i1.acid();
					dpos.area2 = i2.acid();
				}
			}
		}
	}
	else if (t.borders_count() == 1)
	{
		auto const N_tile = get(pos.x - 1, pos.y    , pos.z);
		auto const E_tile = get(pos.x	 , pos.y + 1, pos.z);
		auto const S_tile = get(pos.x + 1, pos.y	, pos.z);
		auto const W_tile = get(pos.x	 , pos.y - 1, pos.z);

		if (N_tile && E_tile && S_tile && W_tile)
		{
			// Check if the neighbors form a suitable pattern
			bool const horiz_case  = E_tile->is_border() && W_tile->is_border()
									 && ( (N_tile->is_innerArea() && !S_tile->is_built()) || (!N_tile->is_built() && S_tile->is_innerArea()) );
			bool const vert_case   = N_tile->is_border() && S_tile->is_border()
								     && ( (E_tile->is_innerArea() && !W_tile->is_built()) || (!E_tile->is_built() && W_tile->is_innerArea()) );

			if (horiz_case || vert_case)
			{
				dpos.type = BorderDoorType::External;
				dpos.area1 = t.get_borderInfos()[0].acid();
			}
		}
		
	}
	//else not a border or non border non suitable for a door

	return dpos;
}

void TileSet::build_internalDoor(int const x, int const y, int const z, DoorId const did, TileType const tile_style)
{
	#if DYNAMIC_ASSERTS
		if (!is_between_twoBorders(x, y, z)) { throw std::runtime_error("Unexpected state: an internal door must be built between and only between two borders."); }
	#endif

	get_existentMutable(x, y, z).build_internalDoor(did, tile_style);
}

void TileSet::build_externalDoor(int const x, int const y, int const z, DoorId const did, TileType const tile_style)
{
	#if DYNAMIC_ASSERTS
		if (!is_between_twoBorders(x, y, z)) { throw std::runtime_error("Unexpected state: an external door must be built between and only between two borders."); }
	#endif

	get_existentMutable(x, y, z).build_externalDoor(did, tile_style);
}

void TileSet::unbuild_door(int const x, int const y, int const z)
{
	#if DYNAMIC_ASSERTS
		if (!is_between_twoBorders(x, y, z)) 
		{	
			throw std::runtime_error("Unexpected state: the door should have been built between and only between two borders."); 
		}
	#endif

	get_existentMutable(x, y, z).unbuild_door();
}

		
bool TileSet::is_between_twoBorders(int const x, int const y, int const z) const
{
	auto const N_tile = get(x - 1, y, z);
	auto const E_tile = get(x, y + 1, z);
	auto const S_tile = get(x + 1, y, z);
	auto const W_tile = get(x, y - 1, z);

	return (N_tile && S_tile && N_tile->is_border() && S_tile->is_border())
		|| (W_tile && E_tile && W_tile->is_border() && E_tile->is_border());
}

void TileSet::debug_generateDefaultTileset()
{
	for (int z = 0; z < m_height; ++z)
	{
		//underground
		if (z < sim_settings.map.ground_floor)
			debug_generateDefaultLevel(z, TileType::underground);
		//ground
		else if (z == sim_settings.map.ground_floor)
			debug_generateDefaultLevel(z, TileType::ground);
		//sky
		else
			debug_generateDefaultLevel(z, TileType::sky);
	}
}

void TileSet::debug_generateDefaultLevel(int const z, TileType const ttype)
{
	for (auto y = 0; y < m_width; ++y)
	{
		for (auto x = 0; x < m_length; ++x)
		{
			auto t = get_mutable(x, y, z);
			//TODO: set coordinates and ttype during construction
			AT::construct(m_alloc, t, Vector3i(x, y, z), ttype);
		}
	}
}


auto TileSet::write(flatbuffers::FlatBufferBuilder & fbb) const -> flatbuffers::Offset<tgmschema::TileSet>
{
	auto tiles_offsetVec = std::vector<flatbuffers::Offset<tgmschema::TileWrapper>>{};

	for (auto z = 0; z < height(); ++z)
	{
		for (auto y = 0; y < width(); ++y)
		{
			for (auto x = 0; x < length(); ++x)
			{
				auto pos = tgmschema::Vector3i{ x, y, z };
				auto tile_offset = get_existent(x, y, z).write(fbb);

				tiles_offsetVec.push_back(tgmschema::CreateTileWrapper(fbb, &pos, tile_offset));
			}
		}
	}

	auto const tiles_offset = fbb.CreateVector(tiles_offsetVec);
	return tgmschema::CreateTileSet(fbb, length(), width(), height(), tiles_offset);
}

void TileSet::read(tgmschema::TileSet const*const ts)
{
	//TODO: 12: Il reset del TileSet va anche ad impattare sui VisualDebug, resettare anche quello.
	reset(ts->length(), ts->width(), ts->height());

	for (auto z = 0; z < height(); ++z)
	{
		for (auto y = 0; y < width(); ++y)
		{
			for (auto x = 0; x < length(); ++x)
			{
				auto tw = ts->tiles()->Get(static_cast<flatbuffers::uoffset_t>(z) * m_width * m_length + static_cast<flatbuffers::uoffset_t>(y) * m_length + x);

				get_existentMutable(x, y, z).read(tw->t());
			}
		}
	}
}



} //namespace tgm