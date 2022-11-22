#ifndef GM_MAP_SETTINGS_HH
#define GM_MAP_SETTINGS_HH


#include <cstdint>


//If activated it will create a roof one microtile larger than the underlying area (graphically ugly for the moment, but could be a useful feature in the future)
#define HIPROOFALGORITHM_ROOF_EAVES false 
	

// If active it makes a statistics about the occurrence of each of the RoofPerimeterMicrotileType.
#define TESTS_HIPROOFALGORITHM_ROOFPERIMETERMICROTILETYPESTATISTICS false


namespace tgm
{



struct MapSettings
{
	//This variables will be removed in the future, dimensions of the gamemap are non-static (dynamic).
	//Note: test_length * test_width * test_height must be a multiple of chunkSize_inTile
	//int const test_length = 100;
	//int const int test_width = 150;
	//int const int test_height = 50;
	//int const int ground_floor = 29; 
		
	int const test_length = 1000;
	int const test_width = 1500;
	int const test_height = 1;
	int const ground_floor = 0; // since the first version of the game will have only one floor, then ground_floor will remain static


	int const max_cityBlockSurface = 1500;	//in tiles
	int const road_dim = 2;					//in tiles


	unsigned const max_buildingCount = 15000u;
	unsigned const max_npcCount = 30000u;

		
	unsigned const max_cityCount = 20u;
	unsigned const max_blockCount = 200u;


	unsigned const max_roofCount = 2000u;
	bool generate_roofs = true;				//if true the BuildingAlgorithm generates the roof logic
	bool const roof_every_area = true;		//cover every area with a roof, despite of the fact that they are roofable areas or not

	unsigned const test_farm_expId = 1;
	unsigned const test_alwaysReplace_expId = 2;
};



} //namespace tgm


#endif //GM_MAP_SETTINGS_HH