#include "data_writer.hh"


#include "settings/game_state_settings.hh"
#include "utilities/filesystem_utilities.hh"


namespace tgm
{



auto operator<<(std::ofstream & ofs, GameMap const& map) -> std::ofstream &
{
	ofs << map.m_tiles;
	ofs << map.m_buildings;

	return ofs;
}

auto operator<<(std::ofstream & ofs, TileSet const& ts) -> std::ofstream &
{
	ofs << ts.length() << ' ' << ts.width() << ' ' << ts.height() << '\n';

	for (auto z = 0; z < ts.height(); ++z)
	{
		for (auto y = 0; y < ts.width(); ++y)
		{
			for (auto x = 0; x < ts.length(); ++x)
			{
				ofs << Vector3i{ x, y, z } << ' ';

				ofs << ts.get_existent(x, y, z);

				ofs << '\n';
			}
		}
	}

	return ofs;
}


auto operator<<(std::ofstream & ofs, TileType const& tt) -> std::ofstream &
{
	switch (tt)
	{
		case TileType::none:
			ofs << "none";
			break;
		case TileType::underground:
			ofs << "underground";
			break;
		case TileType::ground:
			ofs << "ground";
			break;
		case TileType::sky:
			ofs << "sky";
			break;
		case TileType::wooden:
			ofs << "wooden";
			break;
		case TileType::grout:
			ofs << "grout";
			break;
		default:
			throw std::runtime_error("Unexpected or new case.");
			break;
	}


	return ofs;
}

auto operator<<(std::ofstream & ofs, BorderStyle const& bs) -> std::ofstream &
{
	switch (bs)
	{
		case BorderStyle::none:
			ofs << "none";
			break;
		case BorderStyle::brickWall:
			ofs << "brickWall";
			break;
		default:
			throw std::runtime_error("Unexpected or new case.");
			break;
	}


	return ofs;
}

auto operator<<(std::ofstream & ofs, Tile const& t) -> std::ofstream &
{
	ofs << t.inner_area					<< ' ';
	ofs << t.m_block					<< ' ';
	ofs << t.door						<< ' ';
	ofs << t.door_open					<< ' ';

	ofs << t.m_borders					<< ' ';
	for (auto i = decltype(t.m_borders){0}; i < t.m_borders; ++i)
	{
		ofs << t.m_building_infos[i]	<< ' ';
	}
	ofs << t.type						<< ' ';
	ofs << t.m_border_style				<< ' ';

	ofs << t.m_roof_count				<< ' ';
	for (auto i = decltype(t.m_roof_count){0}; i < t.m_roof_count; ++i)
	{
		ofs << t.m_roof_infos[i]		<< ' ';
	}

	ofs << t.m_furniture_id				<< ' ';

	ofs << t.hosted_mobiles				<< ' ';


	return ofs;
}

auto operator<<(std::ofstream & ofs, TileBuildingInfo const& tbi) -> std::ofstream &
{
	ofs << tbi.m_bid << ' ' << tbi.m_aid;

	return ofs;
}

auto operator<<(std::ofstream & ofs, RoofInfo const& ri) -> std::ofstream &
{
	ofs << ri.bid << ' ' << ri.roof_id;

	return ofs;
}


namespace DataWriter
{
	auto generate_saveStream(std::string const& filename) -> std::ofstream
	{
		return FsUtil::create_overwriting(GStateSet::saves_folder + filename + GStateSet::saves_ext);
	}
}



} //namespace tgm


