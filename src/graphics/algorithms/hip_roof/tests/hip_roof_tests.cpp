#include "hip_roof_tests.hh"

#include <unordered_set>
#include <bitset>

#include "settings/simulation/simulation_settings.hh"
#include "settings/simulation/map_settings.hh"
#include "map/buildings/roof_algorithm.hh"
#include "graphics/algorithms/hip_roof/hip_roof_algorithm.hh"

#include "debug/visual/building_expansion_stream.hh"
#include "debug/visual/hip_roof_matrix_stream.hh"


namespace tgm
{


namespace HipRoofAlgorithm
{
    //	.##
    //	###
    //	##.
    static void Junction_NE_SW(PrefabBuilding & p)
    {
        g_log << "Junction_NE_SW" << std::endl;

        auto const orig = Vector3i{ 30, 30, sim_settings.map.ground_floor };

        p.append_area(AreaType::farmyard, orig + Vector3i{ 10,  0, 0 }, { 11, 11 }); //linking areas
        p.append_area(AreaType::farmyard, orig + Vector3i{ 20,  0, 0 }, { 11, 11 });
        p.append_area(AreaType::farmyard, orig + Vector3i{  0, 10, 0 }, { 11, 11 });
        p.append_area(AreaType::farmyard, orig + Vector3i{  0, 20, 0 }, { 11, 11 });

        p.append_area(AreaType::farmyard, orig + Vector3i{ 10, 20, 0 }, { 11, 11 });
        p.append_area(AreaType::farmyard, orig + Vector3i{ 20, 10, 0 }, { 11, 11 });
    }

    //	##.
    //	###
    //	.##
    static void Junction_SE_NW(PrefabBuilding & p)
    {
        g_log << "Junction_SE_NW" << std::endl;

        auto const orig = Vector3i{ 30, 30, sim_settings.map.ground_floor };

        p.append_area(AreaType::farmyard, orig + Vector3i{ 10,  0, 0 }, { 11, 11 }); //linking areas
        p.append_area(AreaType::farmyard, orig + Vector3i{ 20,  0, 0 }, { 11, 11 }); 
        p.append_area(AreaType::farmyard, orig + Vector3i{ 20, 10, 0 }, { 11, 11 });
        p.append_area(AreaType::farmyard, orig + Vector3i{ 20, 20, 0 }, { 11, 11 });
        
        p.append_area(AreaType::farmyard, orig + Vector3i{  0, 10, 0 }, { 11, 11 });
        p.append_area(AreaType::farmyard, orig + Vector3i{ 10, 20, 0 }, { 11, 11 });
    }
    
    //	..#		..#		#.#		#.#
    //	##.		##.		##.		##.
    //	##.		###		##.		###
    static void TangencyNE(PrefabBuilding & p)
    {
        g_log << "TangencyNE" << std::endl;



        // CASE I
        p.append_area(AreaType::farmyard, Vector3i(20, 30, 0), { 11, 11 }); //linking areas
        p.append_area(AreaType::farmyard, Vector3i(20, 20, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(20, 10, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(30, 10, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(40, 10, 0), { 11, 11 });
        
        p.append_area(AreaType::farmyard, Vector3i(41, 20, 0), { 10, 10 });
        


        // CASE II
        p.append_area(AreaType::farmyard, Vector3i(20, 40, 0), { 11, 11 }); //linking areas
        p.append_area(AreaType::farmyard, Vector3i(20, 70, 0), { 11, 11 }); 
        p.append_area(AreaType::farmyard, Vector3i(20, 60, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(20, 50, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(30, 50, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(40, 50, 0), { 11, 11 });
        
        p.append_area(AreaType::farmyard, Vector3i(30, 70, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(30, 60, 0),  { 11, 9 });
        p.append_area(AreaType::farmyard, Vector3i(41, 60, 0),  { 10, 10 });
        

        
        // CASE III
        p.append_area(AreaType::farmyard, Vector3i(20, 80, 0), { 11, 11 }); //linking areas
        p.append_area(AreaType::farmyard, Vector3i(20, 110, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(20, 100, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(20, 90, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(30, 90, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(40, 90, 0), { 11, 11 });
        
        p.append_area(AreaType::farmyard, Vector3i(30, 110, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(42, 109, 0),  { 11, 9 });
        p.append_area(AreaType::farmyard, Vector3i(41, 100, 0),  { 10, 10 });

        

        // CASE IV
        p.append_area(AreaType::farmyard, Vector3i(50, 10, 0), { 11, 11 }); //linking areas
        p.append_area(AreaType::farmyard, Vector3i(60, 30, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(60, 20, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(60, 10, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(70, 10, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(80, 10, 0), { 11, 11 });
        
        p.append_area(AreaType::farmyard, Vector3i(70, 30, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(70, 20, 0), { 11, 9 });
        p.append_area(AreaType::farmyard, Vector3i(82, 29, 0), { 10, 10 });
        p.append_area(AreaType::farmyard, Vector3i(81, 20, 0), { 10, 10 });
    }

    //	##.		##.		###		###
    //	##.		##.		##.		##.
    //	..#		#.#		..#		#.#
    static void TangencySE(PrefabBuilding & p)
    {
        g_log << "TangencySE" << std::endl;
        
        // CASE I
        p.append_area(AreaType::farmyard, Vector3i(10, 20, 0), { 11, 31 }); //linking areas
        p.append_area(AreaType::farmyard, Vector3i(20, 40, 0), { 21, 11 });
        
        p.append_area(AreaType::farmyard, Vector3i(20, 20, 0), { 10, 10 });
        


        // CASE II
        p.append_area(AreaType::farmyard, Vector3i(10, 50, 0), { 11, 41 }); //linking areas
        p.append_area(AreaType::farmyard, Vector3i(20, 80, 0), { 21, 11 });
        
        p.append_area(AreaType::farmyard, Vector3i(30, 70, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(29, 60, 0), { 12, 9 });
        p.append_area(AreaType::farmyard, Vector3i(20, 60, 0), { 10, 10 });
        


        // CASE III
        p.append_area(AreaType::farmyard, Vector3i(10, 90, 0), { 11, 41 }); //linking areas
        p.append_area(AreaType::farmyard, Vector3i(20, 120, 0), { 21, 11 });
        
        p.append_area(AreaType::farmyard, Vector3i(30, 110, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(20, 109, 0), { 9, 12 });
        p.append_area(AreaType::farmyard, Vector3i(20, 100, 0), { 10, 10 });



        // CASE IV
        p.append_area(AreaType::farmyard, Vector3i(40, 40, 0), { 41, 11 }); //linking areas
        p.append_area(AreaType::farmyard, Vector3i(50, 20, 0), { 11, 21 });
        
        p.append_area(AreaType::farmyard, Vector3i(70, 30, 0), { 11, 11 });
        p.append_area(AreaType::farmyard, Vector3i(69, 20, 0), { 12, 9 });
        p.append_area(AreaType::farmyard, Vector3i(60, 29, 0), {  9, 12 });
        p.append_area(AreaType::farmyard, Vector3i(60, 20, 0), { 10, 10 });
    }

    //	.##		.##		###		###
    //	.##		.##		.##		.##
    //	#..		#.#		#..		#.#
    static void TangencySW(PrefabBuilding & p)
    {
        g_log << "TangencySW" << std::endl;
        

        // CASE I
        {
            Vector3i orig{ 10, 20, sim_settings.map.ground_floor };

            p.append_area(AreaType::farmyard, orig + Vector3i( 0,  0, 0), { 31, 11 }); //linking areas
            p.append_area(AreaType::farmyard, orig + Vector3i( 0, 10, 0), { 11, 21 });

            p.append_area(AreaType::farmyard, orig + Vector3i(10, 21, 0), { 10, 10 });
        }


        // CASE II
        {
            Vector3i orig{ 10, 60, sim_settings.map.ground_floor };

            p.append_area(AreaType::farmyard, orig + Vector3i( 0,-10, 0), { 11, 11 }); //linking areas
            p.append_area(AreaType::farmyard, orig + Vector3i( 0,  0, 0), { 31, 11 }); 
            p.append_area(AreaType::farmyard, orig + Vector3i( 0, 10, 0), { 11, 21 });

            p.append_area(AreaType::farmyard, orig + Vector3i(20, 10, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, orig + Vector3i(19, 22, 0), { 12,  9 });
            p.append_area(AreaType::farmyard, orig + Vector3i(10, 21, 0), { 10, 10 });
        }

        
        // CASE III
        {
            Vector3i orig{ 10, 100, sim_settings.map.ground_floor };

            p.append_area(AreaType::farmyard, orig + Vector3i( 0,-10, 0), { 11, 11 }); //linking areas
            p.append_area(AreaType::farmyard, orig + Vector3i( 0,  0, 0), { 31, 11 }); 
            p.append_area(AreaType::farmyard, orig + Vector3i( 0, 10, 0), { 11, 21 });

            p.append_area(AreaType::farmyard, orig + Vector3i(20, 10, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, orig + Vector3i(10, 10, 0), {  9, 12 });
            p.append_area(AreaType::farmyard, orig + Vector3i(10, 21, 0), { 10, 10 });
        }


        // CASE IV
        {
            Vector3i orig{ 50, 20, sim_settings.map.ground_floor };

            p.append_area(AreaType::farmyard, orig + Vector3i(-10, 0, 0), { 11, 11 }); //linking areas
            p.append_area(AreaType::farmyard, orig + Vector3i( 0,  0, 0), { 31, 11 }); 
            p.append_area(AreaType::farmyard, orig + Vector3i( 0, 10, 0), { 11, 21 });

            p.append_area(AreaType::farmyard, orig + Vector3i(20, 10, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, orig + Vector3i(19, 22, 0), { 12,  9 });
            p.append_area(AreaType::farmyard, orig + Vector3i(10, 10, 0), {  9, 12 });
            p.append_area(AreaType::farmyard, orig + Vector3i(10, 21, 0), { 10, 10 });
        }
    }

    //	#..		#.#		#..		#.#
    //	.##		.##		.##		.##
    //	.##		.##		###		###
    static void TangencyNW(PrefabBuilding & p)
    {
        g_log << "TangencyNW" << std::endl;
        
        // CASE I
        {
            p.append_area(AreaType::farmyard, Vector3i( 10, 20, 0), { 11, 111 }); //linking areas
            p.append_area(AreaType::farmyard, Vector3i( 20, 40, 0), { 61,  11 });

            p.append_area(AreaType::farmyard, Vector3i( 20, 20, 0), { 10,  10 });
        }


        // CASE II
        {
            p.append_area(AreaType::farmyard, Vector3i( 20, 80, 0), { 21, 11 }); //linking areas

            p.append_area(AreaType::farmyard, Vector3i( 30, 70, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 20, 71, 0), { 11, 10 });
            p.append_area(AreaType::farmyard, Vector3i( 20, 60, 0), { 10, 10 });
        }

        
        // CASE III
        {
            p.append_area(AreaType::farmyard, Vector3i(20, 120, 0), { 21, 11 }); //linking areas

            p.append_area(AreaType::farmyard, Vector3i(30, 110, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, Vector3i(31, 100, 0), { 10, 11 });
            p.append_area(AreaType::farmyard, Vector3i(20, 100, 0), { 10, 10 });
        }

        // CASE IV
        {
            p.append_area(AreaType::farmyard, Vector3i(50, 20, 0), { 11, 21 }); //linking areas

            p.append_area(AreaType::farmyard, Vector3i(70, 30, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, Vector3i(60, 31, 0), { 11, 10 });
            p.append_area(AreaType::farmyard, Vector3i(71, 20, 0), { 10, 11 });
            p.append_area(AreaType::farmyard, Vector3i(60, 20, 0), { 10, 10 });
        }
    }


    //	##.		##.
    //	.##		.##
    //	.##		###
    static void Foot_N_W(PrefabBuilding & p)
    {
        g_log << "Foot_N_W" << std::endl;

        //linking areas
        p.append_area(AreaType::farmyard, Vector3i( 10, 20, 0), { 11,  71 });
        p.append_area(AreaType::farmyard, Vector3i( 20, 40, 0), { 21,  11 });
        p.append_area(AreaType::farmyard, Vector3i( 20, 80, 0), { 21,  11 });

        // CASE I
        {
            p.append_area(AreaType::farmyard, Vector3i( 20, 20, 0), { 10,  11 });
        }

        // CASE II
        {
            p.append_area(AreaType::farmyard, Vector3i( 30, 70, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 31, 60, 0), { 10, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 20, 60, 0), { 10, 11 });
        }
    }


    //	.##		.##
    //	##.		##.
    //	##.		###
    static void Foot_N_E(PrefabBuilding & p)
    {
        g_log << "Foot_N_E" << std::endl;

        //linking areas
        p.append_area(AreaType::farmyard, Vector3i( 10, 20, 0), { 11,  71 });
        p.append_area(AreaType::farmyard, Vector3i( 20, 20, 0), { 21,  11 });
        p.append_area(AreaType::farmyard, Vector3i( 20, 60, 0), { 21,  11 });

        // CASE I
        {
            p.append_area(AreaType::farmyard, Vector3i( 20, 40, 0), { 10,  11 });
        }

        // CASE II
        {
            p.append_area(AreaType::farmyard, Vector3i( 30, 70, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 31, 80, 0), { 10, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 20, 80, 0), { 10, 11 });
        }
    }


    //	..#		#.#
    //	###		###
    //	##.		##.
    static void Foot_E_N(PrefabBuilding & p)
    {
        g_log << "Foot_E_N" << std::endl;

        //linking areas
        p.append_area(AreaType::farmyard, Vector3i( 10, 20, 0), { 11,  71 });
        p.append_area(AreaType::farmyard, Vector3i( 20, 20, 0), { 21,  11 });
        p.append_area(AreaType::farmyard, Vector3i( 20, 60, 0), { 21,  11 });

        // CASE I
        {
            p.append_area(AreaType::farmyard, Vector3i( 20, 41, 0), { 11,  10 });
        }

        // CASE II
        {
            p.append_area(AreaType::farmyard, Vector3i( 30, 70, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 20, 70, 0), { 11, 10 });
            p.append_area(AreaType::farmyard, Vector3i( 20, 81, 0), { 11, 10 });
        }
    }


    //	##.		##.
    //	###		###
    //	..#		#.#
    static void Foot_E_S(PrefabBuilding & p)
    {
        g_log << "Foot_E_S" << std::endl;

        //linking areas
        p.append_area(AreaType::farmyard, Vector3i( 50, 20, 0), { 11,  71 });
        p.append_area(AreaType::farmyard, Vector3i( 30, 20, 0), { 21,  11 });
        p.append_area(AreaType::farmyard, Vector3i( 30, 60, 0), { 21,  11 });

        // CASE I
        {
            p.append_area(AreaType::farmyard, Vector3i( 40, 41, 0), { 11,  10 });
        }

        // CASE II
        {
            p.append_area(AreaType::farmyard, Vector3i( 30, 70, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 40, 70, 0), { 11, 10 });
            p.append_area(AreaType::farmyard, Vector3i( 40, 81, 0), { 11, 10 });
        }
    }


    //	##.		###
    //	##.		##.
    //	.##		.##
    static void Foot_S_E(PrefabBuilding & p)
    {
        g_log << "Foot_S_E" << std::endl;
        
        //linking areas
        p.append_area(AreaType::farmyard, Vector3i( 50, 20, 0), { 11,  71 });
        p.append_area(AreaType::farmyard, Vector3i( 30, 20, 0), { 21,  11 });
        p.append_area(AreaType::farmyard, Vector3i( 30, 60, 0), { 21,  11 });

        // CASE I
        {
            p.append_area(AreaType::farmyard, Vector3i( 41, 40, 0), { 10,  11 });
        }

        // CASE II
        {
            p.append_area(AreaType::farmyard, Vector3i( 30, 70, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 30, 80, 0), { 10, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 41, 80, 0), { 10, 11 });
        }
    }


    //	.##		###
    //	.##		.##
    //	##.		##.
    static void Foot_S_W(PrefabBuilding & p)
    {
        g_log << "Foot_S_W" << std::endl;
        
        //linking areas
        p.append_area(AreaType::farmyard, Vector3i( 50, 20, 0), { 11,  71 });
        p.append_area(AreaType::farmyard, Vector3i( 30, 40, 0), { 21,  11 });
        p.append_area(AreaType::farmyard, Vector3i( 30, 80, 0), { 21,  11 });

        // CASE I
        {
            p.append_area(AreaType::farmyard, Vector3i( 41, 20, 0), { 10,  11 });
        }

        // CASE II
        {
            p.append_area(AreaType::farmyard, Vector3i( 30, 70, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 30, 60, 0), { 10, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 41, 60, 0), { 10, 11 });
        }
    }


    //	.##		.##
    //	###		###
    //	#..		#.#
    static void Foot_W_S(PrefabBuilding & p)
    {
        g_log << "Foot_W_S" << std::endl;
        
        //linking areas
        p.append_area(AreaType::farmyard, Vector3i( 50, 20, 0), { 11,  71 });
        p.append_area(AreaType::farmyard, Vector3i( 30, 40, 0), { 21,  11 });
        p.append_area(AreaType::farmyard, Vector3i( 30, 80, 0), { 21,  11 });

        // CASE I
        {
            p.append_area(AreaType::farmyard, Vector3i( 40, 20, 0), { 11,  10 });
        }

        // CASE II
        {
            p.append_area(AreaType::farmyard, Vector3i( 30, 70, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 40, 71, 0), { 11, 10 });
            p.append_area(AreaType::farmyard, Vector3i( 40, 60, 0), { 11, 10 });
        }
    }


    //	#..		#.#
    //	###		###
    //	.##		.##
    static void Foot_W_N(PrefabBuilding & p)
    {
        g_log << "Foot_W_N" << std::endl;

        //linking areas
        p.append_area(AreaType::farmyard, Vector3i( 10, 20, 0), { 11,  71 });
        p.append_area(AreaType::farmyard, Vector3i( 20, 40, 0), { 21,  11 });
        p.append_area(AreaType::farmyard, Vector3i( 20, 80, 0), { 21,  11 });

        // CASE I
        {
            p.append_area(AreaType::farmyard, Vector3i( 20, 20, 0), { 11,  10 });
        }

        // CASE II
        {
            p.append_area(AreaType::farmyard, Vector3i( 30, 70, 0), { 11, 11 });
            p.append_area(AreaType::farmyard, Vector3i( 20, 71, 0), { 11, 10 });
            p.append_area(AreaType::farmyard, Vector3i( 20, 60, 0), { 11, 10 });
        }
    }

    #pragma warning(disable: 4702)
    void oldTest_roofPerimeterTileType_specialCases(GameMap & map, RoofVertices & roof_vertices)
    {
        if (sim_settings.map.generate_roofs) { throw std::runtime_error("Cannot perform the test if roof generation is active."); }

        if (map.tiles().length() < 100 || map.tiles().width() < 150 || map.tiles().height() < 1)
        {
            throw std::runtime_error("Cannot perform the test if the map is not large enough.");
        }
        else if (sim_settings.map.ground_floor != 0)
        {
            throw std::runtime_error("Cannot perform the test if the ground floor is not zero.");
        }
        else if (sim_settings.map.max_cityBlockSurface < 5000)
        {
            throw std::runtime_error("Cannot perform the test if maximum city block surface is not big enough.");
        }


        static auto test_id = 0;
        static auto last_bid = BuildingId{ 0 };

        ++test_id;
        if (test_id > 14) { test_id = 1; }

        if (last_bid != 0)
        {
            #if VISUALDEBUG
                visualDebug_runtime_openWindow = false;
            #endif

            map.debug_remove_building(last_bid);
            last_bid = 0;

            #if VISUALDEBUG
                visualDebug_runtime_openWindow = true;
            #endif
        }


        g_log << "Added a building to test RoofPerimeterTileType::";

        
        auto prefab = PrefabBuilding{ "farm" };
        prefab.append_area(AreaType::test, { 30, 30, sim_settings.map.ground_floor }, { 11, 11 });
        
        switch (test_id)
        {
            case 1:
                Junction_NE_SW(prefab);
                break;
            case 2:
                Junction_SE_NW(prefab);
                break;
            case 3:
                TangencyNE(prefab);
                break;
            case 4:
                TangencySE(prefab);
                break;
            case 5:
                TangencySW(prefab);
                break;
            case 6:
                TangencyNW(prefab);
                break;
            case 7:
                Foot_N_W(prefab);
                break;
            case 8:
                Foot_N_E(prefab);
                break;
            case 9:
                Foot_E_N(prefab);
                break;
            case 10:
                Foot_E_S(prefab);
                break;
            case 11:
                Foot_S_E(prefab);
                break;
            case 12:
                Foot_S_W(prefab);
                break;
            case 13:
                Foot_W_S(prefab);
                break;
            case 14:
                Foot_W_N(prefab);
                break;
            default:
                throw std::runtime_error("Unexpected test_id");
        }

        auto const building_handler = map.debug_build_prefabBuilding(prefab);
        last_bid = building_handler.first;
        if (!last_bid) { throw std::runtime_error("Impossible to build the building."); }

        // Pick the BuildingAreaId of the first area of the building as a starting point to build the roof
        auto const random_aid = building_handler.second->areas_by_ref().cbegin()->id();

        #if !BUILDEXP_VISUALDEBUG_ROOF_GENERATION
            throw std::runtime_error("Cannot perform the test if BUILDEXP_VISUALDEBUG_ROOF_GENERATION is not active.");
        #endif
        #if BUILDEXP_VISUALDEBUG
            BEdeb.begin_chapter("Hip Roof Test - Old RoofPerimeterTileType special case");
        #endif
        #if VISUALDEBUG
            visualDebug_runtime_openWindowForBuildingExpansion = true;
            visualDebug_runtime_openWindowForHipRoofMatrix = true;
            int old_maxRecordableDepth = visualDebug_runtime_maxRecordableDepth;
            visualDebug_runtime_maxRecordableDepth = 3;
        #endif


        roof_vertices.clear();

        auto const roofable_poss = RoofAlgorithm::compute_roofablePositions_fromArea({ last_bid, random_aid }, map.debug_get_buildings(), map.tiles());
        auto const polygons = HipRoofAlgorithm::generate_hipRoof({ roofable_poss.cbegin(), roofable_poss.cend() }, sim_settings.map.ground_floor, map.tiles().length(), map.tiles().width());
            
        for (auto const& p : polygons.south)
        {
            roof_vertices.south_roof.create_polygon(p);
        }

        for (auto const& p : polygons.west)
        {
            roof_vertices.west_roof.create_polygon(p);
        }

        for (auto const& p : polygons.north)
        {
            roof_vertices.north_roof.create_polygon(p);
        }

        for (auto const& p : polygons.east)
        {
            roof_vertices.east_roof.create_polygon(p);
        }

        #if BUILDEXP_VISUALDEBUG
            BEdeb.end_chapter();
        #endif
        #if VISUALDEBUG
            visualDebug_runtime_openWindowForBuildingExpansion = false;
            visualDebug_runtime_openWindowForHipRoofMatrix = false;
            visualDebug_runtime_maxRecordableDepth = old_maxRecordableDepth;
        #endif

            
        #pragma warning(default: 4702)
    }
    

    #pragma warning(disable: 4702)
    void test_everyRoofPerimeterMicrotileTypeCase()
    {
        #if !MAPSET_HIPROOFALGORITHM_ROOF_PERIMETER_MICROTILE_TYPE_STATISTICS
            throw std::runtime_error("Cannot perform the test if RoofPerimeterMicrotileType statistics isn't active.");
        #endif

        auto const z = 0;
        auto const fake_mapLength = 100;
        auto const fake_mapWidth = 100;

        for (auto i = int{ 0b000000001 }; i <= 0b111111111; ++i)
        {
            std::bitset<9> config(i);
            std::unordered_set<Vector3i> roofable_tiles;

            if ((config & std::bitset<9>{0b000000001}).any())
                roofable_tiles.emplace(0, 0, z);

            if ((config & std::bitset<9>{0b000000010}).any())
                roofable_tiles.emplace(1, 0, z);

            if ((config & std::bitset<9>{0b000000100}).any())
                roofable_tiles.emplace(2, 0, z);

            if ((config & std::bitset<9>{0b000001000}).any())
                roofable_tiles.emplace(0, 1, z);

            if ((config & std::bitset<9>{0b000010000}).any())
                roofable_tiles.emplace(1, 1, z);

            if ((config & std::bitset<9>{0b000100000}).any())
                roofable_tiles.emplace(2, 1, z);

            if ((config & std::bitset<9>{0b001000000}).any())
                roofable_tiles.emplace(0, 2, z);

            if ((config & std::bitset<9>{0b010000000}).any())
                roofable_tiles.emplace(1, 2, z);

            if ((config & std::bitset<9>{0b100000000}).any())
                roofable_tiles.emplace(2, 2, z);

            generate_hipRoof({ roofable_tiles.cbegin(), roofable_tiles.cend() }, 0, fake_mapLength, fake_mapWidth);
        }


        g_log << "test_everyRoofPerimeterMicrotileTypeCase completed.\n" << roofPerimeterMicrotileType_stats << std::endl;
        
        #pragma warning(default: 4702)
    }




    static void triangleRoof_north(PrefabBuilding & p)
    {
        g_log << "Triangle roof - North" << std::endl;
        

        Vector3i orig{ 20, 30, sim_settings.map.ground_floor };

        p.append_area(AreaType::farmyard, orig + Vector3i(  0,  0,  0), { 11, 23 });
        p.append_area(AreaType::farmyard, orig + Vector3i(  10, 12, 0), { 11, 11 });
    }
    
    static void triangleRoof_east(PrefabBuilding & p)
    {
        g_log << "Triangle roof - East" << std::endl;
        

        Vector3i orig{ 30, 30, sim_settings.map.ground_floor };

        p.append_area(AreaType::farmyard, orig + Vector3i(  0, 10,  0), { 23, 11 });
        p.append_area(AreaType::farmyard, orig + Vector3i(  12,  0, 0), { 11, 11 });
    }
    
    static void triangleRoof_south(PrefabBuilding & p)
    {
        g_log << "Triangle roof - South" << std::endl;
        

        Vector3i orig{ 30, 30, sim_settings.map.ground_floor };

        p.append_area(AreaType::farmyard, orig + Vector3i( 10,  0, 0), { 11, 23 });
        p.append_area(AreaType::farmyard, orig + Vector3i(  0, 12, 0), { 11, 11 });
    }
    
    static void triangleRoof_west(PrefabBuilding & p)
    {
        g_log << "Triangle roof - West" << std::endl;
        

        Vector3i orig{ 30, 20, sim_settings.map.ground_floor };

        p.append_area(AreaType::farmyard, orig + Vector3i(  0,  0,  0), { 23, 11 });
        p.append_area(AreaType::farmyard, orig + Vector3i(  12, 10, 0), { 11, 11 });
    }

    static void triangleRoofs_together(PrefabBuilding & p)
    {
        g_log << "Triangle roofs - all together" << std::endl;
        

        p.append_area(AreaType::farmyard, Vector3i(  20,  30, sim_settings.map.ground_floor), { 11, 13 });
        p.append_area(AreaType::farmyard, Vector3i(  20,  42, sim_settings.map.ground_floor), { 13, 11 });
        p.append_area(AreaType::farmyard, Vector3i(  32,  40, sim_settings.map.ground_floor), {  9, 13 });
    }

#pragma warning(disable: 4702)
    void test_polygons_specialCases(GameMap & map, RoofVertices & roof_vertices)
    {
        if (sim_settings.map.generate_roofs) { throw std::runtime_error("Cannot perform the test if roof generation is active."); }
            
        if (map.tiles().length() < 100 || map.tiles().width() < 150 || map.tiles().height() < 1)
        {
            throw std::runtime_error("Cannot perform the test if the map settings are not correctly set up.");
        }


        static auto test_id = 0;

        ++test_id;
        if (test_id > 5) { test_id = 1; }
        

        static auto last_bid = BuildingId{ 0 };

        if (last_bid != 0)
        {
            map.debug_remove_building(last_bid);
            last_bid = 0;
        }


        g_log << "Added a building in order to test: ";
        
        auto prefab = PrefabBuilding{ "farm" };
        prefab.append_area(AreaType::test, { 30, 30, sim_settings.map.ground_floor }, { 11, 11 });
        
        switch (test_id)
        {
            case 1:
                triangleRoof_north(prefab);
                break;
            case 2:
                triangleRoof_east(prefab);
                break;
            case 3:
                triangleRoof_south(prefab);
                break;
            case 4:
                triangleRoof_west(prefab);
                break;
            case 5:
                triangleRoofs_together(prefab);
                break;
            default:
                throw std::runtime_error("Unexpected test_id");
                break;
        }


        auto const building_handler = map.debug_build_prefabBuilding(prefab);
        last_bid = building_handler.first;
        if (!last_bid) { throw std::runtime_error("Impossible to build the building."); }

        // Pick the BuildingAreaId of the first area of the building as a starting point to build the roof
        auto const random_aid = building_handler.second->areas_by_ref().cbegin()->id();

        
        #if !HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
            throw std::runtime_error("Cannot perform the test if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS is not active.");
        #endif		
        #if BUILDEXP_VISUALDEBUG
            BEdeb.begin_chapter("Hip Roof Test - Polygon special case");
        #endif
        #if VISUALDEBUG
            visualDebug_runtime_openWindowForHipRoofMatrix = true;
            int old_maxRecordableDepth = visualDebug_runtime_maxRecordableDepth;
            visualDebug_runtime_maxRecordableDepth = 2;
        #endif

        roof_vertices.clear();

        auto const roofable_poss = RoofAlgorithm::compute_roofablePositions_fromArea({ last_bid, random_aid }, map.debug_get_buildings(), map.tiles());
        auto const polygons = HipRoofAlgorithm::generate_hipRoof({ roofable_poss.cbegin(), roofable_poss.cend() }, sim_settings.map.ground_floor, map.tiles().length(), map.tiles().width());
                    
        for (auto const& p : polygons.south)
        {
            roof_vertices.south_roof.create_polygon(p);
        }

        for (auto const& p : polygons.west)
        {
            roof_vertices.west_roof.create_polygon(p);
        }

        for (auto const& p : polygons.north)
        {
            roof_vertices.north_roof.create_polygon(p);
        }

        for (auto const& p : polygons.east)
        {
            roof_vertices.east_roof.create_polygon(p);
        }


        #if BUILDEXP_VISUALDEBUG
            BEdeb.end_chapter();
        #endif
        #if VISUALDEBUG
            visualDebug_runtime_openWindowForHipRoofMatrix = false;
            visualDebug_runtime_maxRecordableDepth = old_maxRecordableDepth;
        #endif

            
    #pragma warning(default: 4702)
    }


    //TODO: 01: Fai partire questi test automatici (anche provando diverse dimensioni della mappa)
    void automatically_test_building_expansion(GameMap & map)
    {
        if (!sim_settings.map.generate_roofs) { throw std::runtime_error("Switch on 'MapSettings::generate_roofs' before launching this test."); }

        #if HIPROOFMATRIX_VISUALDEBUG
            throw std::runtime_error("Switch off HIPROOFMATRIX_VISUALDEBUG before launching this test.");
        #endif		
        #if BUILDEXP_VISUALDEBUG
            throw std::runtime_error("Switch off BUILDEXP_VISUALDEBUG before launching this test.");
        #endif
            
        // Test settings
        static auto constexpr expansions_per_position = 1000;
        static auto const building_potentialAreas = std::string{ "farm" };
        static auto const building_startingArea = AreaType::test;
        static auto const startingArea_length = area_templates.at(AreaType::test).min_dims().x;
        static auto const startingArea_width = area_templates.at(AreaType::test).min_dims().y;


        Logger lgr{ g_log };

        // A building can't be built too near to the end of map.
        auto const road_margin = sim_settings.map.road_dim + 1; //"+1" because of how city algorithm works

        for (int x = road_margin; x < map.tiles().length() - road_margin - startingArea_length + 1; ++x)
        {
            ////Skip some positions I've already tested.
            //if (x < 0)
            //	continue; 

            for (int y = road_margin; y < map.tiles().width() - road_margin - startingArea_width + 1; ++y)
            {
                /*static int x = 0;
                static int y = 0;
                static BuildingId bid = 0;

                if (bid != 0)
                    map.debug_remove_building(bid);*/


                lgr << "Testing a building whose starting position is: " << Vector3i{ x, y, sim_settings.map.ground_floor } << std::endl;
                lgr << Logger::addt;
                
                auto prefab = PrefabBuilding{ building_potentialAreas };
                prefab.append_area(building_startingArea, { x, y, sim_settings.map.ground_floor }, { 11, 11 });

                auto [bid, building_ptr] = map.debug_build_prefabBuilding(prefab);
                if (!bid) { throw std::runtime_error("Impossible to build the building."); }

                for (auto i = 0; i < expansions_per_position; ++i)
                {
                    lgr << "\r" << Logger::tabs << "Expansion attempt #" << i << "              ";
                    map.debug_request_buildingExpansion(bid);
                    map.debug_expand_buildings();
                }

                lgr << roofPerimeterMicrotileType_stats;

                lgr << Logger::remt << std::endl;
                
                
                map.debug_remove_building(bid);

                /*if (x < map.tiles().length())
                {
                    if(y < map.tiles().width())
                        ++y;
                    else
                    {
                        ++x;
                        y = 0;
                    }
                }
                else
                {
                    lgr << "All positions tested." << std::endl;
                }*/
            }
        }
    }
}



} //namespace tgm