#ifndef GM_HIP_ROOF_TESTS_HH
#define GM_HIP_ROOF_TESTS_HH


#include "graphics/roof_vertices.hh"
#include "map/gamemap.h"


namespace tgm
{



namespace HipRoofAlgorithm
{	
	////
	//	Test every possible configuration of 3x3 roofable_tiles. Such a test should address every possible RoofPerimeterMicrotileType case.
	//	The test is used to find out possible unforeseen special cases.
	////
	void test_everyRoofPerimeterMicrotileTypeCase();
	
	////
	//  Old test. It was useful when the HipRoofAlgorithm computed the ProjectableBorders using the tiles. Now it uses microtiles and the above test
	//  (test_everyRoofPerimeterMicrotileTypeCase) can efficiently test all the possible cases (special ones included).
	//	This old test is still valuable to test particular roof shape patterns. The function remebers how many times it's been invoked, and
	//	each time tests a different special case, adding a different building to the map.
	////
	void oldTest_roofPerimeterTileType_specialCases(GameMap & map, RoofVertices & roof_vertices);


	////
	//	Add special buildings to test particular cases of roof polygon. Each time the function is called the next special case will be tested.
	////
	void test_polygons_specialCases(GameMap & map, RoofVertices & roof_vertices);

	////
	//	For each plane position try to add a building and try to expand that building to the largest extent.
	////
	void automatically_test_building_expansion(GameMap & map);
}



} //namespace tgm


#endif //GM_HIP_ROOF_TESTS_HH