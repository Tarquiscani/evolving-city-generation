#ifndef GM_MAP_SETTINGS_HH
#define GM_MAP_SETTINGS_HH


#include <cstdint>


////
//  When enabled it creates a roof that is one microtile larger than the underlying area (graphically ugly for the moment, but could be a useful feature in the future).
////
#define MAPSET_HIPROOFALGORITHM_ROOF_EAVES false 
    

////
//  When enabled it collects data about about the occurrences of the different RoofPerimeterMicrotileType.
////
#define MAPSET_HIPROOFALGORITHM_ROOF_PERIMETER_MICROTILE_TYPE_STATISTICS false


namespace tgm
{



struct MapSettings
{
    //int const test_length = 100;
    //int const int test_width = 150;
    //int const int test_height = 50;
    //int const int ground_floor = 29; 
    
    ////
    //  Dimensions of the game map. Beware that the for now the engine doesn't support chunking or streaming, so a large map will fill up your RAM and VRAM.
    //  In the future the dimensions of the map will be determined by the user in-game and these settings will be removed.
    //  Note: For an implementation reason the product of test_length * test_width * test_height must be a multiple of 
    //        GraphicsSettings::chunkSize_inTile.
    ////
#if CMAKE_LOW_QUALITY
    int const test_length = 150;
    int const test_width = 150;
#elif CMAKE_MEDIUM_QUALITY
    int const test_length = 450;
    int const test_width = 500;
#else // CMAKE_HIGH_QUALITY
    int const test_length = 1000;
    int const test_width = 1500;
#endif
    int const test_height = 1;


    ////
    //  The engine was originally conceived to handle multistorey buildings, but the feature has been postponed.
    ////
    int const ground_floor = 0;
    
    ////
    //  Maximum allowed surface for a block (in tiles).
    ////
    int const max_cityBlockSurface = 1500;

    ////
    //  Minimum space between two blocks (in tiles).
    ////
    int const road_dim = 2;

    
    ////
    //  Maximum number of buildings that the map can contain. It's a hard cap, adding more buildings will throw an exception.
    ////
    unsigned const max_buildingCount = 30000u;

    ////
    //  Maximum number of characters and mobiles that the map can contain. It's a hard cap, adding more mobiles will throw an exception.
    ////
    unsigned const max_npcCount = 30000u;

        
    ////
    //  Maximum number of cities that the map can contain. It's a hard cap, adding more cities will throw an exception.
    ////
    unsigned const max_cityCount = 20u;

    ////
    //  Maximum number of blocks that the map can contain. It's a hard cap, adding more blocks will throw an exception.
    ////
    unsigned const max_blockCount = 400u;

    
    ////
    //  Maximum number of roofs that the map can contain. It's a hard cap, adding more roofs will throw an exception.
    ////
    unsigned const max_roofCount = 30000u;

    ////
    //  Determine whether or not the City Generation Algorithm should also generates and display roofs.
    ////
    bool generate_roofs = true;

    ////
    //  Some areas are meant to be without a roof (farms, gardens, etc.), but by enabling this options all areas will be roofed (used for debug purpose). 
    ////
    bool const roof_every_area = true;

    unsigned const test_farm_expId = 1;
    unsigned const test_alwaysReplace_expId = 2;
};



} //namespace tgm


#endif //GM_MAP_SETTINGS_HH