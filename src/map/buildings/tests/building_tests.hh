#ifndef GM_BUILDING_TESTS_HH
#define GM_BUILDING_TESTS_HH


#include "map/gamemap.h"
#include "map/buildings/prefab_building.hh"


namespace tgm
{



namespace BuildingSpecialCases
{
	////
	//	This building is special because there are some replaceable areas that share the same border.
	//	But I don't remember exactly why this is important. It was an old special case that the current
	//	algorithm can easily manage.
	////
	auto tris_building(Vector3i const pos) -> PrefabBuilding;
	
	////
	//	This buildings contain an empty yard inside (of different sizes). They're useful to test outlines.
	////
	auto inner_yard(Vector3i const pos) -> PrefabBuilding;
	auto thin_innerYard(Vector3i const pos) -> PrefabBuilding;
	auto singleTile_innerYard(Vector3i const pos) -> PrefabBuilding;
	auto twoTile_innerYard(Vector3i const pos) -> PrefabBuilding;
	auto double_innerYard(Vector3i const pos) -> PrefabBuilding;
	auto replaceable_in_innerYard(Vector3i const pos) -> PrefabBuilding;
	


	////
	//	Useful to test what happens when the new area could replace every area of the building.
	////
	auto full_replacement(Vector3i const pos) -> PrefabBuilding;

	////
	//	Useful to test what happens when replacing an area would disconnect a building.
	////
	auto disconnecting_replacement(Vector3i const pos) -> PrefabBuilding;

	////
	//	Useful to test what happens when replacing an area would disconnect a building.
	////
	auto disconnecting_replacement2(Vector3i const pos) -> PrefabBuilding;

	////
	//	Useful to test what happens when replacing an area would NOT discconect a building.
	////
	auto nondisconnecting_replacement(Vector3i const pos) -> PrefabBuilding;

	////
	//	Useful to test what happens when replacing the first area of the building (caused a bug before).
	////
	auto firstArea_replacement(Vector3i const pos) -> PrefabBuilding;

	////
	//	Useful to test what happens when replacing an area around a courtyard.
	////
	auto thinInnerYard_replacement(Vector3i const pos) -> PrefabBuilding;
};


namespace BuildingAlgorithmTests
{
	void blockOutline_tests(GameMap & map);

	void automatic_cityDevelopment(GameMap & map, std::vector<BuildingId> & created_buildings);
};



} //namespace tgm


#endif //GM_BUILDING_TESTS_HH