#include "tile.hh"


#include "debug/logger/debug_printers.h"


namespace tgm
{



void Tile::build_innerArea(CityBlockId const cbid, BuildingId const bid, BuildingAreaId const aid, TileType const new_style)
{
	#if DYNAMIC_ASSERTS
		if (is_built()) { throw std::runtime_error("The tile already holds a building."); }
	#endif

	inner_area = true;
	m_building_infos[0].set(bid, aid);
	type = new_style;

	m_block = cbid;
}

void Tile::unbuild_innerArea(BuildingId const bid, BuildingAreaId const aid)
{
	#if DYNAMIC_ASSERTS
		assert_innerArea();
	#endif

	if (!m_building_infos[0].is(bid, aid)) { throw std::runtime_error("The actual building and areas don't match 'bid' and 'aid'"); }

	m_building_infos[0].reset(); // when the tile stores the paved part of an area, only the first slot is used

	type = TileType::ground;

	inner_area = false;

	m_block = 0;
}


void Tile::build_border(CityBlockId const cbid, BuildingId const bid, BuildingAreaId const aid, BorderStyle const style)
{
	if (inner_area) { throw std::runtime_error("The tile already holds an inner area."); }
	if (door) { throw std::runtime_error("The tile holds a door and can't be built further."); }
	if (m_borders >= max_borders) { throw std::runtime_error("This tile has already the maximum of built borders (4)."); }

	// Update building_infos
	m_building_infos[m_borders].set(bid, aid);
	++m_borders;

	// If this is the first border built 
	if (m_borders == 1)
	{
		m_border_style = style;

		if (m_block != 0) { throw std::runtime_error("The CityBlockId for a non-built tile should have been 0."); }
		m_block = cbid;
	}
	else
	{
		if (m_block != cbid) { throw std::runtime_error("The CityBlockId of the new border should have matched that of the other borders."); }
	}
}

void Tile::unbuild_border(BuildingId const bid, BuildingAreaId const aid)
{
	#if DYNAMIC_ASSERTS
		assert_border();

		if (door) { throw std::runtime_error("The tile holds a door and can't be unbuilt."); }
	#endif

	// Find the slot corresponding to 'bid' and 'aid'
	int slot = -1;
	for (auto i = 0u; i < m_borders; ++i)
	{
		if (m_building_infos[i].is(bid, aid))
		{
			slot = i;
		}
	}
	#if DYNAMIC_ASSERTS
		if (slot == -1) { throw std::runtime_error("There is no border corresponding to the supplied BuildingId and BuildingAreaId."); }
	#endif

	m_building_infos[slot].reset();
	--m_borders;

	// Reorder the array (move all the empty elements on the right)
	for (unsigned i = 0; i < m_borders; ++i)           //note: m_borders has been already drecreased but it's fine for the algorithm
	{
		if (m_building_infos[i].is_empty())
		{
			for (unsigned j = i + 1; j < m_building_infos.size(); ++j)
			{
				if (!m_building_infos[j].is_empty())
				{
					std::swap(m_building_infos[i], m_building_infos[j]);
					break;
				}
			}
		}
	}

	// If this was the last border built on the tile, then reset the block and the border style
	if (m_borders == 0)
	{
		m_block = 0;
		m_border_style = BorderStyle::none;
	}
}


void Tile::build_roof(BuildingId const bid, RoofId const rid)
{
	#if DYNAMIC_ASSERTS
		assert_built_with(bid);
		assert_notRoofed_for(bid);
	#endif

	if (m_roof_count == max_roofs) { throw std::runtime_error("Already hosts the max number of roofs."); }

	for (auto i = 0u; i < max_roofs; ++i)
	{
		auto & rinfo = m_roof_infos[i];
		if (rinfo.bid == 0)
		{
			rinfo.bid = bid;
			rinfo.roof_id = rid;
			++m_roof_count;
			return;
		}
	}

	throw std::runtime_error("All roof infos are full.");
}

void Tile::unbuild_roof(BuildingId const bid)
{
	bool rinfo_found = false;
	for (auto i = 0u; i < max_roofs; ++i)
	{
		auto & rinfo = m_roof_infos[i];
		if (rinfo.bid == bid)
		{
			rinfo.bid = 0;
			rinfo.roof_id = 0;

			rinfo_found = true;
			break;
		}
	}

	if (rinfo_found)
	{
		--m_roof_count;
		// Sort the array (move all the empty elements to the right)
		std::sort(m_roof_infos.begin(), m_roof_infos.end(), [](RoofInfo const& r1, RoofInfo const& r2) { return r1.bid > r2.bid; });
	}
	else
	{
		throw std::runtime_error("There is no RoofInfo mathcing 'bid'.");
	}
}

bool Tile::is_roofed_for(BuildingId const bid) const
{
	return std::find_if(m_roof_infos.cbegin(), m_roof_infos.cend(), [bid](RoofInfo const& rinfo) { return rinfo.bid == bid; }) != m_roof_infos.cend();
}

void Tile::assert_built_with(BuildingId const bid) const
{
	assert_built();

	if (inner_area)
	{
		if (m_building_infos[0].bid() == bid) { return; }
	}
	else if (is_border())
	{
		for (auto i = 0u; i < m_borders; ++i)
		{
			if (m_building_infos[i].bid() == bid) { return; }
		}
	}
	else
	{
		throw std::runtime_error("A built Tile must be either an inner area or a border");
	}

	throw std::runtime_error("There is no building info matching 'bid'");
}

void Tile::assert_notRoofed_for(BuildingId const bid) const
{
	if (is_roofed_for(bid))	{ throw std::runtime_error("Already roofed for 'bid'"); }
}

auto Tile::write(flatbuffers::FlatBufferBuilder & fbb) const -> flatbuffers::Offset<tgmschema::Tile>
{
	tgmschema::TileBuildingInfo binfo_array[max_borders];

	auto binfoArray_size = std::size_t{ 0 };
	if (is_built()) { binfoArray_size = is_innerArea() ? 1 : m_borders; }

	for (auto i = std::size_t{ 0 }; i < binfoArray_size; ++i)
	{
		binfo_array[i] = tgmschema::TileBuildingInfo{ m_building_infos[i].bid(), m_building_infos[i].aid() };
	}
	auto bld_infos = fbb.CreateVectorOfStructs(binfo_array, binfoArray_size);


	tgmschema::RoofInfo rinfo_arr[max_roofs];
	for (auto i = decltype(m_roof_count){ 0 }; i < m_roof_count; ++i)
	{
		rinfo_arr[i] = tgmschema::RoofInfo{ m_roof_infos[i].bid, m_roof_infos[i].roof_id };
	}
	auto rf_infos = fbb.CreateVectorOfStructs(rinfo_arr, m_roof_count);


	return tgmschema::CreateTile(fbb,
							  inner_area, m_block, door, door_open,
							  static_cast<std::int8_t>(m_borders), bld_infos,
							  static_cast<tgmschema::TileType>(type), static_cast<tgmschema::BorderStyle>(m_border_style),
							  rf_infos,
							  m_furniture_id, static_cast<std::int16_t>(hosted_mobiles)													);
}

void Tile::read(tgmschema::Tile const*const t)
{
	inner_area = t->inner_area();
	m_block = t->block();
	door = t->door();
	door_open = t->door_open();
	
	m_borders = t->border_count();

	auto const binfos_bin = t->building_infos();
	for (auto i = flatbuffers::uoffset_t{ 0 }; i < binfos_bin->size(); ++i)
	{
		m_building_infos[i].set(binfos_bin->Get(i)->bid(), binfos_bin->Get(i)->aid());
	}
	for (auto i = binfos_bin->size(); i < max_borders; ++i)
	{
		m_building_infos[i].reset();
	}

	type = static_cast<TileType>(t->type());
	m_border_style = static_cast<BorderStyle>(t->border_style());

	auto const rinfos_bin = t->roof_infos();
	m_roof_count = rinfos_bin->size();
	for (auto i = flatbuffers::uoffset_t{ 0 }; i < rinfos_bin->size(); ++i)
	{
		m_roof_infos[i].bid = rinfos_bin->Get(i)->bid();
		m_roof_infos[i].roof_id = rinfos_bin->Get(i)->roof_id();
	}
	for (auto i = rinfos_bin->size(); i < max_roofs; ++i)
	{
		m_roof_infos[i].bid = 0;
		m_roof_infos[i].roof_id = 0;
	}

	m_furniture_id = t->furniture_id();

	hosted_mobiles = t->hosted_mobiles();
}

auto operator<<(Logger & lgr, Tile const& t) -> Logger&
{
	lgr << "tile{"
		<< Logger::addt
			<< Logger::nltb << "coordinates: " << t.coordinates
			<< Logger::nltb << "borders:" << t.m_borders
			<< Logger::nltb << "door: " << t.door
		<< Logger::remt
		<< Logger::nltb << "}";


	return lgr;
}



} //namespace tgm