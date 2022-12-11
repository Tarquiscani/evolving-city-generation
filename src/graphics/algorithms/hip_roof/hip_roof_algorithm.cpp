#include "hip_roof_algorithm.hh"


#include <set>
#include <unordered_set>
#include <bitset>
#include <optional>
#include <limits>

#include "settings/simulation/map_settings.hh"
#include "std_extensions/hash_functions.hh"
#include "system/rect.hh"
#include "hip_roof_matrix.hh"

#include "debug/logger/logger.hh"
#include "debug/visual/building_expansion_stream.hh"
#include "debug/visual/hip_roof_matrix_stream.hh"


namespace tgm
{



auto operator<<(Logger & lgr, BuildingAreaCompleteId const acid) -> Logger &
{
    lgr << "{bid: " << acid.bid << ", aid: " << acid.aid << "}";

    return lgr;
}

namespace HipRoofAlgorithm
{
    auto operator<<(Logger& lgr, RoofPolygons const& rp) -> Logger &
    {
        lgr << "RoofPolygons {"
            << Logger::addt
                << Logger::nltb << "South: " << rp.south
                << Logger::nltb << "West: " << rp.west
                << Logger::nltb << "North: " << rp.north
                << Logger::nltb << "East: " << rp.east
            << Logger::remt
            << Logger::nltb << "}" << std::endl;

        return lgr;
    }

    
    auto operator<<(std::ostream & os, RoofPerimeterMicrotileTypeStatistics const & rptts) -> std::ostream &
    {
        os << "RoofPerimeterTileTypeStatistics: "
        << "\nNone            " << rptts.None
        << "\nSideS_1:        " << rptts.SideS_1
        << "\nSideS_2:        " << rptts.SideS_2
        << "\nSideS_3:        " << rptts.SideS_3
        << "\nSideS_4:        " << rptts.SideS_4
        << "\nConvexSW_1:     " << rptts.ConvexSW_1
        << "\nConvexSW_2:     " << rptts.ConvexSW_2
        << "\nConvexSW_3:     " << rptts.ConvexSW_3
        << "\nConvexSW_4:     " << rptts.ConvexSW_4
        << "\nConcaveSW:      " << rptts.ConcaveSW
        << "\nSideW_1:        " << rptts.SideW_1
        << "\nSideW_2:        " << rptts.SideW_2
        << "\nSideW_3:        " << rptts.SideW_3
        << "\nSideW_4:        " << rptts.SideW_4
        << "\nConvexNW_1:     " << rptts.ConvexNW_1
        << "\nConvexNW_2:     " << rptts.ConvexNW_2
        << "\nConvexNW_3:     " << rptts.ConvexNW_3
        << "\nConvexNW_4:     " << rptts.ConvexNW_4
        << "\nConcaveNW:      " << rptts.ConcaveNW
        << "\nSideN_1:        " << rptts.SideN_1
        << "\nSideN_2:        " << rptts.SideN_2
        << "\nSideN_3:        " << rptts.SideN_3
        << "\nSideN_4:        " << rptts.SideN_4
        << "\nConvexNE_1:     " << rptts.ConvexNE_1
        << "\nConvexNE_2:     " << rptts.ConvexNE_2
        << "\nConvexNE_3:     " << rptts.ConvexNE_3
        << "\nConvexNE_4:     " << rptts.ConvexNE_4
        << "\nConcaveNE:      " << rptts.ConcaveNE
        << "\nSideE_1:        " << rptts.SideE_1
        << "\nSideE_2:        " << rptts.SideE_2
        << "\nSideE_3:        " << rptts.SideE_3
        << "\nSideE_4:        " << rptts.SideE_4
        << "\nConvexSE_1:     " << rptts.ConvexSE_1
        << "\nConvexSE_2:     " << rptts.ConvexSE_2
        << "\nConvexSE_3:     " << rptts.ConvexSE_3
        << "\nConvexSE_4:     " << rptts.ConvexSE_4
        << "\nConcaveSE:      " << rptts.ConcaveSE
        << "\nJunction_SE_NW: " << rptts.Junction_SE_NW
        << "\nJunction_NE_SW: " << rptts.Junction_NE_SW
        << "\nTangencyNE:     " << rptts.TangencyNE
        << "\nTangencySE:     " << rptts.TangencySE
        << "\nTangencySW:     " << rptts.TangencySW
        << "\nTangencyNW:     " << rptts.TangencyNW
        << "\nFoot_N_W:       " << rptts.Foot_N_W
        << "\nFoot_N_E:       " << rptts.Foot_N_E
        << "\nFoot_E_N:       " << rptts.Foot_E_N
        << "\nFoot_E_S:       " << rptts.Foot_E_S
        << "\nFoot_S_E:       " << rptts.Foot_S_E
        << "\nFoot_S_W:       " << rptts.Foot_S_W
        << "\nFoot_W_S:       " << rptts.Foot_W_S
        << "\nFoot_W_N:       " << rptts.Foot_W_N
        << "\nunexpected:     " << rptts.unexpected << std::endl;

        return os;
    }


    /////////////////////////////////////

    //		    Roofable zone	       //

    /////////////////////////////////////


    static auto compute_roofableMicrotiles(std::vector<Vector3i> const& roofable_poss, Matrix const& roof_matrix) -> std::unordered_set<Vector2i>
    {
        std::unordered_set<Vector2i> roofable_microtiles;

        for (auto pos : roofable_poss)
        {
            auto micro_pos = roof_matrix.tile_to_microtile(pos);

            roofable_microtiles.insert(micro_pos);
            roofable_microtiles.insert(micro_pos + Vector2i( -1,  0));
            roofable_microtiles.insert(micro_pos + Vector2i( -1,  1));
            roofable_microtiles.insert(micro_pos + Vector2i(  0,  1));
            roofable_microtiles.insert(micro_pos + Vector2i(  1,  1));
            roofable_microtiles.insert(micro_pos + Vector2i(  1,  0));
            roofable_microtiles.insert(micro_pos + Vector2i(  1, -1));
            roofable_microtiles.insert(micro_pos + Vector2i(  0, -1));
            roofable_microtiles.insert(micro_pos + Vector2i( -1, -1));
            
            #if HIPROOFALGORITHM_ROOF_EAVES
                // Fill also the sorrounding microtiles that are two microtiles away from the center of the tile.
                roofable_microtiles.insert( micro_pos + Vector2i( -2,  0) );
                roofable_microtiles.insert( micro_pos + Vector2i( -2,  1) );
                roofable_microtiles.insert( micro_pos + Vector2i( -2,  2) );
                roofable_microtiles.insert( micro_pos + Vector2i( -1,  2) );
                roofable_microtiles.insert( micro_pos + Vector2i(  0,  2) );
                roofable_microtiles.insert( micro_pos + Vector2i(  1,  2) );
                roofable_microtiles.insert( micro_pos + Vector2i(  2,  2) );
                roofable_microtiles.insert( micro_pos + Vector2i(  2,  1) );
                roofable_microtiles.insert( micro_pos + Vector2i(  2,  0) );
                roofable_microtiles.insert( micro_pos + Vector2i(  2, -1) );
                roofable_microtiles.insert( micro_pos + Vector2i(  2, -2) );
                roofable_microtiles.insert( micro_pos + Vector2i(  1, -2) );
                roofable_microtiles.insert( micro_pos + Vector2i(  0, -2) );
                roofable_microtiles.insert( micro_pos + Vector2i( -1, -2) );
                roofable_microtiles.insert( micro_pos + Vector2i( -2, -2) );
                roofable_microtiles.insert( micro_pos + Vector2i( -2, -1) );
            #endif
        }
        
        #if HIPROOFMATRIX_VISUALDEBUG
            HRMdeb.new_step("Roofable microtiles", 2);

            HRMdeb.highlight_tiles(roofable_microtiles.cbegin(), roofable_microtiles.cend(), 0, Color::Red);
        #endif

        return roofable_microtiles;
    }


    static auto fill_roofMatrix(std::unordered_set<Vector2i> const& roofable_microtiles, Matrix & roof_matrix)
    {
        for (auto micro_pos : roofable_microtiles)
        {
            roof_matrix.fill(micro_pos);
        }
    }



    ////////////////////////////////////

    // Projectable borders algorithm  //

    ////////////////////////////////////


    ////
    //	Check whether @pos belongs to @roofable_poss.
    ////
    static bool does_belong_to_roof(Vector2i const pos, std::unordered_set<Vector2i> const& roofable_poss)
    {
        //TODO: PERFORMANCE: This find is very demanding. The unordered set could be replaced with a vector of bool, each representing a position.
        return roofable_poss.find(pos) != roofable_poss.cend();
    }

    enum class RoofPerimeterMicrotileType
    {
        None,
        SideS,
        ConvexSW,
        ConcaveSW,
        SideW,
        ConvexNW,
        ConcaveNW,
        SideN,
        ConvexNE,
        ConcaveNE,
        SideE,
        ConvexSE,
        ConcaveSE,
        Junction_SE_NW,
        Junction_NE_SW,
    };

    static auto operator<<(std::ostream& os, RoofPerimeterMicrotileType const type) -> std::ostream&
    {
        switch(type)
        {
            case RoofPerimeterMicrotileType::None:
                os << "None";
                break;
            case RoofPerimeterMicrotileType::SideS:
                os << "SideS";
                break;
            case RoofPerimeterMicrotileType::ConvexSW:
                os << "ConvexSW";
                break;
            case RoofPerimeterMicrotileType::ConcaveSW:
                os << "ConcaveSW";
                break;
            case RoofPerimeterMicrotileType::SideW:
                os << "SideW";
                break;
            case RoofPerimeterMicrotileType::ConvexNW:
                os << "ConvexNW";
                break;
            case RoofPerimeterMicrotileType::ConcaveNW:
                os << "ConcaveNW";
                break;
            case RoofPerimeterMicrotileType::SideN:
                os << "SideN";
                break;
            case RoofPerimeterMicrotileType::ConvexNE:
                os << "ConvexNE";
                break;
            case RoofPerimeterMicrotileType::ConcaveNE:
                os << "ConcaveNE";
                break;
            case RoofPerimeterMicrotileType::SideE:
                os << "SideE";
                break;
            case RoofPerimeterMicrotileType::ConvexSE:
                os << "ConvexSE";
                break;
            case RoofPerimeterMicrotileType::ConcaveSE:
                os << "ConcaveSE";
                break;
            case RoofPerimeterMicrotileType::Junction_SE_NW:
                os << "Junction_SE_NW";
                break;
            case RoofPerimeterMicrotileType::Junction_NE_SW:
                os << "Junction_NE_SW";
                break;
            default:
                throw std::runtime_error("Unexpected RoofParameterTileType");
                break;
        }

        return os;
    }



    struct ProjectableBorder
    {
        Vector2i firstVertex_pos;
        Vector2i firstVertex_versor;

        Vector2i secondVertex_pos;
        Vector2i secondVertex_versor;
        
        Vector2i side_versor; //versor pointing from the first vertex to the second
    };

    static auto compute_roofPerimeterMicrotileType(Vector2i const pos, std::unordered_set<Vector2i> const& roofable_poss) -> RoofPerimeterMicrotileType
    {
        bool N  = does_belong_to_roof(pos + Versor2i::N,  roofable_poss);
        bool NE = does_belong_to_roof(pos + Versor2i::NE, roofable_poss);
        bool E  = does_belong_to_roof(pos + Versor2i::E,  roofable_poss);
        bool SE = does_belong_to_roof(pos + Versor2i::SE, roofable_poss);
        bool S  = does_belong_to_roof(pos + Versor2i::S,  roofable_poss);
        bool SW = does_belong_to_roof(pos + Versor2i::SW, roofable_poss);
        bool W  = does_belong_to_roof(pos + Versor2i::W,  roofable_poss);
        bool NW = does_belong_to_roof(pos + Versor2i::NW, roofable_poss);
        
        
        //	###
        //	###
        //	###
        //in the middle of the roof (not in the perimeter)
        if (N && NE && E && SE && S && SW && W && NW) //11111111
            return RoofPerimeterMicrotileType::None;
        
        //	...			..#			#..			#.ws#
        //	###			###			###			###
        //	###			###			###			###
        //side S
        else if (   (!N && !NE &&  E &&  SE &&  S &&  SW &&  W && !NW) //00111110
                 || (!N &&  NE &&  E &&  SE &&  S &&  SW &&  W && !NW) //01111110
                 || (!N && !NE &&  E &&  SE &&  S &&  SW &&  W &&  NW) //00111111
                 || (!N &&  NE &&  E &&  SE &&  S &&  SW &&  W &&  NW) //01111111
                )
            return RoofPerimeterMicrotileType::SideS;
        
        //	...			...			#..			#..
        //	##.			##.			##.			##.
        //	##.			###			##.			###
        //convex vertex SW
        else if (!N && !NE && !E && !SE &&  S &&  SW &&  W && !NW) //00001110
            return RoofPerimeterMicrotileType::ConvexSW;

        //	##.
        //	###
        //	###
        //concave vertex SW
        else if (N && !NE && E && SE && S && SW && W && NW) //10111111
            return RoofPerimeterMicrotileType::ConcaveSW;

        //	##.			##.			###			###
        //	##.			##.			##.			##.
        //	##.			###			##.			###
        //side W
        else if (   ( N && !NE && !E && !SE &&  S &&  SW &&  W &&  NW) //10001111
                 || ( N && !NE && !E &&  SE &&  S &&  SW &&  W &&  NW) //10011111
                 || ( N &&  NE && !E && !SE &&  S &&  SW &&  W &&  NW) //11001111
                 || ( N &&  NE && !E &&  SE &&  S &&  SW &&  W &&  NW) //11011111
                )
            return RoofPerimeterMicrotileType::SideW;

        //	##.			##.			###			###
        //	##.			##.			##.			##.
        //	...			#..			...			#..
        //convex vertex NW
        else if ( N && !NE && !E && !SE && !S && !SW &&  W &&  NW) //10000011
            return RoofPerimeterMicrotileType::ConvexNW;
        
        //	###
        //	###
        //	##.
        //concave vertex NW
        else if (N && NE && E && !SE && S && SW && W && NW)
            return RoofPerimeterMicrotileType::ConcaveNW;
        
        //	###			###			###			###
        //	###			###			###			###
        //	...			#..			..#			#.#
        //side N
        else if (   ( N &&  NE &&  E && !SE && !S && !SW &&  W &&  NW) //11100011
                 || ( N &&  NE &&  E && !SE && !S &&  SW &&  W &&  NW) //11100111
                 || ( N &&  NE &&  E &&  SE && !S && !SW &&  W &&  NW) //11110011
                 || ( N &&  NE &&  E &&  SE && !S &&  SW &&  W &&  NW) //11110111
                )
            return RoofPerimeterMicrotileType::SideN;
        
        //	.##			###			.##			###
        //	.##			.##			.##			.##
        //	...			...			..#			..#
        //convex vertex NE
        else if ( N &&  NE &&  E && !SE && !S && !SW && !W && !NW) //11100000
            return RoofPerimeterMicrotileType::ConvexNE;
        
        //	###
        //	###
        //	.##
        //concave vertex NE
        else if (N && NE && E && SE && S && !SW && W && NW) //11111011
            return RoofPerimeterMicrotileType::ConcaveNE;

        //	.##			.##			###			###
        //	.##			.##			.##			.##
        //	.##			###			.##			###
        //side E
        else if (   (N && NE && E && SE && S && !SW && !W && !NW) //11111000
                 || (N && NE && E && SE && S &&  SW && !W && !NW) //11111100
                 || (N && NE && E && SE && S && !SW && !W &&  NW) //11111001
                 || (N && NE && E && SE && S &&  SW && !W &&  NW) //11111101
                )
            return RoofPerimeterMicrotileType::SideE;
        
        //	...			..#			...			..#
        //	.##			.##			.##			.##
        //	.##			.##			###			###
        //convex vertex SE
        else if (!N && !NE &&  E &&  SE &&  S && !SW && !W && !NW) //00111000
            return RoofPerimeterMicrotileType::ConvexSE;
        
        //	.##
        //	###
        //	###
        //concave vertex SE
        else if (N && NE && E && SE && S && SW && W && !NW)
            return RoofPerimeterMicrotileType::ConcaveSE;

        //	.##
        //	###
        //	##.
        //junction NE_SW
        else if (N && NE && E && !SE && S && SW && W && !NW)
            return RoofPerimeterMicrotileType::Junction_NE_SW;

        //	##.
        //	###
        //	.##
        //junction SE_NW
        else if (N && !NE && E && SE && S && !SW && W && NW)
            return RoofPerimeterMicrotileType::Junction_SE_NW;

        else
        {
            std::ostringstream oss;
            oss << "Unexpected neighbors configuration. " << "Position: " << pos << ". Neighbors: " 
                << (N ? "N " : "") << (NE ? "NE " : "") << (E ? "E " : "") << (SE ? "SE " : "") 
                << (S ? "S " : "") << (SW ? "SW " : "") << (W ? "W " : "") << (NW ? "NW" : "");

            std::cout << oss.str() << std::endl;
            throw std::runtime_error(oss.str());
        }
    }

    ////
    //	Register the RoofPerimeterMicrotileType of @pos in the global statistics of the occurrences of each case.
    ////
    #pragma warning(disable: 4505)
    static void insert_in_roofPerimeterMicrotileTypeStatistics(Vector2i const pos, std::unordered_set<Vector2i> const& roofable_poss)
    {
        bool N  = does_belong_to_roof(pos + Versor2i::N,  roofable_poss);
        bool NE = does_belong_to_roof(pos + Versor2i::NE, roofable_poss);
        bool E  = does_belong_to_roof(pos + Versor2i::E,  roofable_poss);
        bool SE = does_belong_to_roof(pos + Versor2i::SE, roofable_poss);
        bool S  = does_belong_to_roof(pos + Versor2i::S,  roofable_poss);
        bool SW = does_belong_to_roof(pos + Versor2i::SW, roofable_poss);
        bool W  = does_belong_to_roof(pos + Versor2i::W,  roofable_poss);
        bool NW = does_belong_to_roof(pos + Versor2i::NW, roofable_poss);
        
        
        //	###
        //	###
        //	###
        //in the middle of the roof (not in the perimeter)
        if (N && NE && E && SE && S && SW && W && NW) //11111111
            ++roofPerimeterMicrotileType_stats.None;
        
        //	...			..#			#..			#.#
        //	###			###			###			###
        //	###			###			###			###
        //side S
        else if (!N && !NE &&  E &&  SE &&  S &&  SW &&  W && !NW) //00111110
            ++roofPerimeterMicrotileType_stats.SideS_1;
        else if (!N &&  NE &&  E &&  SE &&  S &&  SW &&  W && !NW) //01111110
            ++roofPerimeterMicrotileType_stats.SideS_2;
        else if (!N && !NE &&  E &&  SE &&  S &&  SW &&  W &&  NW) //00111111
            ++roofPerimeterMicrotileType_stats.SideS_3;
        else if (!N &&  NE &&  E &&  SE &&  S &&  SW &&  W &&  NW) //01111111
            ++roofPerimeterMicrotileType_stats.SideS_4;
        
        //	...			...			#..			#..
        //	##.			##.			##.			##.
        //	##.			###			##.			###
        //convex vertex SW
        else if (!N && !NE && !E && !SE &&  S &&  SW &&  W && !NW) //00001110
            ++roofPerimeterMicrotileType_stats.ConvexSW_1;

        //	##.
        //	###
        //	###
        //concave vertex SW
        else if (N && !NE && E && SE && S && SW && W && NW) //10111111
            ++roofPerimeterMicrotileType_stats.ConcaveSW;

        //	##.			##.			###			###
        //	##.			##.			##.			##.
        //	##.			###			##.			###
        //side W
        else if ( N && !NE && !E && !SE &&  S &&  SW &&  W &&  NW) //10001111
            ++roofPerimeterMicrotileType_stats.SideW_1;
        else if ( N && !NE && !E &&  SE &&  S &&  SW &&  W &&  NW) //10011111
            ++roofPerimeterMicrotileType_stats.SideW_2;
        else if ( N &&  NE && !E && !SE &&  S &&  SW &&  W &&  NW) //11001111
            ++roofPerimeterMicrotileType_stats.SideW_3;
        else if ( N &&  NE && !E &&  SE &&  S &&  SW &&  W &&  NW) //11011111
            ++roofPerimeterMicrotileType_stats.SideW_4;

        //	##.			##.			###			###
        //	##.			##.			##.			##.
        //	...			#..			...			#..
        //convex vertex NW
        else if ( N && !NE && !E && !SE && !S && !SW &&  W &&  NW) //10000011
            ++roofPerimeterMicrotileType_stats.ConvexNW_1;
        
        //	###
        //	###
        //	##.
        //concave vertex NW
        else if (N && NE && E && !SE && S && SW && W && NW)
            ++roofPerimeterMicrotileType_stats.ConcaveNW;
        
        //	###			###			###			###
        //	###			###			###			###
        //	...			#..			..#			#.#
        //side N
        else if ( N &&  NE &&  E && !SE && !S && !SW &&  W &&  NW) //11100011
            ++roofPerimeterMicrotileType_stats.SideN_1;
        else if ( N &&  NE &&  E && !SE && !S &&  SW &&  W &&  NW) //11100111
            ++roofPerimeterMicrotileType_stats.SideN_2;
        else if ( N &&  NE &&  E &&  SE && !S && !SW &&  W &&  NW) //11110011
            ++roofPerimeterMicrotileType_stats.SideN_3;
        else if ( N &&  NE &&  E &&  SE && !S &&  SW &&  W &&  NW) //11110111
            ++roofPerimeterMicrotileType_stats.SideN_4;
        
        //	.##			###			.##			###
        //	.##			.##			.##			.##
        //	...			...			..#			..#
        //convex vertex NE
        else if ( N &&  NE &&  E && !SE && !S && !SW && !W && !NW) //11100000
            ++roofPerimeterMicrotileType_stats.ConvexNE_1;
        
        //	###
        //	###
        //	.##
        //concave vertex NE
        else if (N && NE && E && SE && S && !SW && W && NW) //11111011
            ++roofPerimeterMicrotileType_stats.ConcaveNE;

        //	.##			.##			###			###
        //	.##			.##			.##			.##
        //	.##			###			.##			###
        //side E
        else if (N && NE && E && SE && S && !SW && !W && !NW) //11111000
            ++roofPerimeterMicrotileType_stats.SideE_1;
        else if (N && NE && E && SE && S &&  SW && !W && !NW) //11111100
            ++roofPerimeterMicrotileType_stats.SideE_2;
        else if (N && NE && E && SE && S && !SW && !W &&  NW) //11111001
            ++roofPerimeterMicrotileType_stats.SideE_3;
        else if (N && NE && E && SE && S &&  SW && !W &&  NW) //11111101
            ++roofPerimeterMicrotileType_stats.SideE_4;
        
        //	...			..#			...			..#
        //	.##			.##			.##			.##
        //	.##			.##			###			###
        //convex vertex SE
        else if (!N && !NE &&  E &&  SE &&  S && !SW && !W && !NW) //00111000
            ++roofPerimeterMicrotileType_stats.ConvexSE_1;
        
        //	.##
        //	###
        //	###
        //concave vertex SE
        else if (N && NE && E && SE && S && SW && W && !NW)
            ++roofPerimeterMicrotileType_stats.ConcaveSE;

        //	.##
        //	###
        //	##.
        //junction NE_SW
        else if (N && NE && E && !SE && S && SW && W && !NW)
            ++roofPerimeterMicrotileType_stats.Junction_NE_SW;

        //	##.
        //	###
        //	.##
        //junction SE_NW
        else if (N && !NE && E && SE && S && !SW && W && NW)
            ++roofPerimeterMicrotileType_stats.Junction_SE_NW;

        else
            ++roofPerimeterMicrotileType_stats.unexpected;

            
        #pragma warning(default: 4505)
    }
    

    ////
    //	Decide whether @type identify a vertex.
    //  Note: never use this function check a first vertex.
    //	@return: 'true' for a second vertex, 'false' for a side.
    ////
    static bool is_roofPerimeterVertex(RoofPerimeterMicrotileType const type)
    {
        switch (type)
        {
            case RoofPerimeterMicrotileType::SideS:
            case RoofPerimeterMicrotileType::SideW:
            case RoofPerimeterMicrotileType::SideN:
            case RoofPerimeterMicrotileType::SideE:
                return false;
                break;
            case RoofPerimeterMicrotileType::ConvexSW:
            case RoofPerimeterMicrotileType::ConcaveSW:
            case RoofPerimeterMicrotileType::ConvexNW:
            case RoofPerimeterMicrotileType::ConcaveNW:
            case RoofPerimeterMicrotileType::ConvexNE:
            case RoofPerimeterMicrotileType::ConcaveNE:
            case RoofPerimeterMicrotileType::ConvexSE:
            case RoofPerimeterMicrotileType::ConcaveSE:
            case RoofPerimeterMicrotileType::Junction_SE_NW:
            case RoofPerimeterMicrotileType::Junction_NE_SW:
                return true;
                break;
            case RoofPerimeterMicrotileType::None:
                throw std::runtime_error("Unexpected RoofParameterTileType. It was expected a side or a vertex.");
            default:
                throw std::runtime_error("Unexpected or new RoofParameterTileType");
                break;
        }
    }
    
    
    ////
    //	@return: The versor of the second vertex.
    ////
    static auto compute_secondVertexVersor(RoofPerimeterMicrotileType const type, Vector2i const side_versor) -> Vector2i
    {
        Vector2i v2_vs;

        switch (type)
        {
            case RoofPerimeterMicrotileType::ConvexSW:
            case RoofPerimeterMicrotileType::ConcaveSW:
                v2_vs = {  1, -1 };
                break;
            case RoofPerimeterMicrotileType::ConvexNW:
            case RoofPerimeterMicrotileType::ConcaveNW:
                v2_vs = { -1, -1 };
                break;
            case RoofPerimeterMicrotileType::ConvexNE:
            case RoofPerimeterMicrotileType::ConcaveNE:
                v2_vs = { -1,  1 };
                break;
            case RoofPerimeterMicrotileType::ConvexSE:
            case RoofPerimeterMicrotileType::ConcaveSE:
                v2_vs = {  1,  1 };
                break;
            case RoofPerimeterMicrotileType::Junction_SE_NW:
            {
                if		(side_versor == Vector2i{  1,  0 })
                    v2_vs = {  1, -1 };
                else if (side_versor == Vector2i{ -1,  0 })
                    v2_vs = { -1,  1 };
                else
                {
                    std::ostringstream oss; oss << "Unexpected side_versor " << side_versor << " for Junction_SE_NW";
                    throw std::runtime_error(oss.str());
                }

                break;
            }
            case RoofPerimeterMicrotileType::Junction_NE_SW:
            {
                if		(side_versor == Vector2i{  0,  1 })
                    v2_vs = {  1,  1 };
                else if (side_versor == Vector2i{  0, -1 })
                    v2_vs = { -1, -1 };
                else
                {
                    std::ostringstream oss; oss << "Unexpected side_versor " << side_versor << " for Junction_NE_SW";
                    throw std::runtime_error(oss.str());
                }

                break;
            }
            case RoofPerimeterMicrotileType::None:
            case RoofPerimeterMicrotileType::SideS:
            case RoofPerimeterMicrotileType::SideW:
            case RoofPerimeterMicrotileType::SideN:
            case RoofPerimeterMicrotileType::SideE:
                throw std::runtime_error("Unexpected RoofParameterTileType for a second vertex.");
                break;
            default:
                throw std::runtime_error("Unexpected RoofParameterTileType");
                break;
        }

        return v2_vs;
    }

    ////
    //	Starting from a vertex it traverses the perimeter up to a new vertex and then create the projectable border.
    ////
    static auto create_projectableBorder(Vector2i const firstVertex_pos,
                                         Vector2i const firstVertex_versor,
                                         Vector2i const side_versor,
                                         std::unordered_set<Vector2i> const& roofable_poss)
    {
        ProjectableBorder proj;

        proj.firstVertex_pos = firstVertex_pos;
        proj.firstVertex_versor = firstVertex_versor;
        proj.side_versor = side_versor;

        auto current_pos = firstVertex_pos + side_versor;

        auto current_type = compute_roofPerimeterMicrotileType(current_pos, roofable_poss);

        #if TESTS_HIPROOFALGORITHM_ROOFPERIMETERMICROTILETYPESTATISTICS
            insert_in_roofPerimeterMicrotileTypeStatistics(current_pos, roofable_poss);
        #endif

        while (!is_roofPerimeterVertex(current_type))
        {
            current_pos += side_versor;
            current_type = compute_roofPerimeterMicrotileType(current_pos, roofable_poss);

            #if TESTS_HIPROOFALGORITHM_ROOFPERIMETERMICROTILETYPESTATISTICS
                insert_in_roofPerimeterMicrotileTypeStatistics(current_pos, roofable_poss);
            #endif
        }

        if (!is_roofPerimeterVertex(current_type))
            throw std::runtime_error("Unexpected algorithm behavior.");

        proj.secondVertex_pos = current_pos;
        proj.secondVertex_versor = compute_secondVertexVersor(current_type, side_versor);

        if (proj.firstVertex_pos.x != proj.secondVertex_pos.x && proj.firstVertex_pos.y != proj.secondVertex_pos.y)
        {
            std::ostringstream oss;
            oss << " -- v2_type: " << current_type << "   --   Positions: " << proj.firstVertex_pos << " -> " << proj.secondVertex_pos;
            throw std::runtime_error(oss.str());
        }

        return proj;
    }


    ////
    //	Check if @pos is a vertex, if that is the case, following the perimeter of the roofable area, trace the border associated to that vertex.
    ////
    static void check_for_border(Vector2i const pos,
                                std::unordered_set<Vector2i> const& roofable_poss,
                                std::vector<ProjectableBorder> & projectable_borders)
    {
        // Check if the microtile is suitable to be the first vertex of a border
        auto pos_type = compute_roofPerimeterMicrotileType(pos, roofable_poss);

        #if TESTS_HIPROOFALGORITHM_ROOFPERIMETERMICROTILETYPESTATISTICS
            insert_in_roofPerimeterMicrotileTypeStatistics(pos, roofable_poss);
        #endif

        try{//TODO: Remove this try block when debug is complete
        switch (pos_type)
        {
            case RoofPerimeterMicrotileType::None:
            case RoofPerimeterMicrotileType::SideS:
            case RoofPerimeterMicrotileType::SideW:
            case RoofPerimeterMicrotileType::SideN:
            case RoofPerimeterMicrotileType::SideE:
                return; // It's the center of the roof or a side. Not suitable as first vertex. Skip this position.
                break;
            case RoofPerimeterMicrotileType::ConvexSW:
            {
                Vector2i v1_vs{  1, -1 };
                Vector2i side_versor{ 1, 0 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs, side_versor, roofable_poss));
                break;
            }
            case RoofPerimeterMicrotileType::ConcaveSW:
            {
                Vector2i v1_vs{  1, -1 };
                Vector2i side_versor{ 0, 1 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs, side_versor, roofable_poss));
                break;
            }
            case RoofPerimeterMicrotileType::ConvexNW:
            {
                Vector2i v1_vs{ -1, -1 };
                Vector2i side_versor{ 0, -1 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs, side_versor, roofable_poss));
                break;
            }
            case RoofPerimeterMicrotileType::ConcaveNW:
            {
                Vector2i v1_vs{ -1, -1 };
                Vector2i side_versor{  1,  0 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs, side_versor, roofable_poss));
                break;
            }
            case RoofPerimeterMicrotileType::ConvexNE:
            {
                Vector2i v1_vs{ -1,  1 };
                Vector2i side_versor{ -1,  0 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs, side_versor, roofable_poss));
                break;
            }
            case RoofPerimeterMicrotileType::ConcaveNE:
            {
                Vector2i v1_vs{ -1,  1 };
                Vector2i side_versor{  0, -1 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs, side_versor, roofable_poss));
                break;
            }
            case RoofPerimeterMicrotileType::ConvexSE:
            {
                Vector2i v1_vs{  1,  1 };
                Vector2i side_versor{  0,  1 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs, side_versor, roofable_poss));
                break;
            }
            case RoofPerimeterMicrotileType::ConcaveSE:
            {
                Vector2i v1_vs{  1,  1 };
                Vector2i side_versor{ -1,  0 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs, side_versor, roofable_poss));
                break;
            }
            case RoofPerimeterMicrotileType::Junction_SE_NW:
            {
                // On the contrary to other vertices type, a junction is the vertex of four borders, so each junction is the first vertex of two borders
                    
                Vector2i v1_vs_1{  1, -1 };
                Vector2i sideVersor_1{  0, 1 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs_1, sideVersor_1, roofable_poss));
                    

                Vector2i v1_vs_2{ -1,  1 };
                Vector2i sideVersor_2{  0, -1 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs_2, sideVersor_2, roofable_poss));

                break;
            }
            case RoofPerimeterMicrotileType::Junction_NE_SW:
            {
                // On the contrary to other vertices type, a junction is the vertex of four borders, so each junction is the first vertex of two borders
                    
                Vector2i v1_vs_1{  1,  1 };
                Vector2i sideVersor_1{ -1,  0 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs_1, sideVersor_1, roofable_poss));
                    
                Vector2i v1_vs_2{ -1, -1 };
                Vector2i sideVersor_2{  1,  0 };
                projectable_borders.push_back(create_projectableBorder(pos, v1_vs_2, sideVersor_2, roofable_poss));
                    
                break;
            }
            default:
                throw std::runtime_error("Unexpected RoofParameterTileType");
                break;
        }}
        catch (std::exception & e) 
        { 
            std::ostringstream oss;
            oss << "BUG! v1_type: " << pos_type << e.what();
            std::cout << oss.str() << std::endl;
            throw std::runtime_error(oss.str()); 
        }
    }

    #if HIPROOFMATRIX_VISUALDEBUG
        //TODO: Think about this function, could it still be useful?
        static auto pick_roofPerimeterTileTypeColor(RoofPerimeterMicrotileType const type) -> Color
        {
            switch(type)
            {
                case RoofPerimeterMicrotileType::None:
                    return Color(0xFFFFFFFF);//White
                    break;
                case RoofPerimeterMicrotileType::SideS:
                    return Color(0x800000FF);//Maroon
                    break;
                case RoofPerimeterMicrotileType::ConvexSW:
                    return Color(0x9A6324FF);//Brown
                    break;
                case RoofPerimeterMicrotileType::ConcaveSW:
                    return Color(0x808000FF);//Olive
                    break;
                case RoofPerimeterMicrotileType::SideW:
                    return Color(0xe6194BFF);//Red
                    break;
                case RoofPerimeterMicrotileType::ConvexNW:
                    return Color(0xf58231FF);//Orange
                    break;
                case RoofPerimeterMicrotileType::ConcaveNW:
                    return Color(0xffe119FF);//Yellow
                    break;
                case RoofPerimeterMicrotileType::SideN:
                    return Color(0xbfef45FF);//Lime
                    break;
                case RoofPerimeterMicrotileType::ConvexNE:
                    return Color(0xfabebeFF);//Pink
                    break;
                case RoofPerimeterMicrotileType::ConcaveNE:
                    return Color(0xffd8b1FF);//Apricot
                    break;
                case RoofPerimeterMicrotileType::SideE:
                    return Color(0xfffac8FF);//Beige
                    break;
                case RoofPerimeterMicrotileType::ConvexSE:
                    return Color(0x3cb44bFF);//Green
                    break;
                case RoofPerimeterMicrotileType::ConcaveSE:
                    return Color::Mint;
                    break;
                case RoofPerimeterMicrotileType::Junction_SE_NW:
                    return Color(0x42d4f4FF);//Cyan
                    break;
                case RoofPerimeterMicrotileType::Junction_NE_SW:
                    return Color(0x469990FF);//Teal
                    break;
                default:
                    throw std::runtime_error("Unexpected RoofParameterTileType");
                    break;
            }
        }

        ////
        //	@versor: A vertex versor or a side versor.
        //
        //	@return: 1) A textual description of the versor. 2) A color that represents the versor.
        ////
        static auto pick_roofPerimeterTileVersorDescr(Vector2i const versor) -> std::pair<std::string, Color>
        {
            std::pair<std::string, Color> ret;
            auto & [descr, color] = ret;

            if (versor == Vector2i{ -1,  1 })
            {
                descr = "Vertex towards NE";
                color = Color(0x3cb44bFF); //Green
            }
            else if (versor == Vector2i{  1,  1 })
            {
                descr = "Vertex towards SE";
                color = Color(0xf58231FF); //Orange
            }
            else if (versor == Vector2i{  1, -1 })
            {
                descr = "Vertex towards SW";
                color = Color(0xbfef45FF); //Lime
            }
            else if (versor == Vector2i{ -1, -1 })
            {
                descr = "Vertex towards NW";
                color = Color(0x42d4f4FF); //Cyan
            }

            else if (versor == Vector2i{  0,  1 })
            {
                descr = "Side towards South";
                color = Color(0x808000FF); //Olive
            }
            else if (versor == Vector2i{  1,  0 })
            {
                descr = "Side towards West";
                color = Color(0xffe119FF); //Yellow
            }
            else if (versor == Vector2i{  0, -1 })
            {
                descr = "Side towards North";
                color = Color(0xffd8b1FF); //Apricot
            }
            else if (versor == Vector2i{ -1,  0 })
            {
                descr = "Side towards East";
                color = Color(0x800000FF); //Maroon
            }
            else
            {
                throw std::runtime_error("Unexpected RoofParameterTileType");
            }

            return ret;
        }

        static void visualDebug_highlightProjectableBorder(ProjectableBorder const proj)
        {
            auto [v1_descr, v1_color] = pick_roofPerimeterTileVersorDescr(proj.firstVertex_versor);
            auto [v2_descr, v2_color] = pick_roofPerimeterTileVersorDescr(proj.secondVertex_versor);
            HRMdeb.highlight_tile(proj.firstVertex_pos, 0, v1_color, v1_descr);
            HRMdeb.highlight_tile(proj.secondVertex_pos, 0, v2_color, v2_descr);

            if(proj.firstVertex_pos != proj.secondVertex_pos)
            {
                auto [borderSide_descr, borderSide_color] = pick_roofPerimeterTileVersorDescr(proj.side_versor);
            
                std::vector<TileHighlightingWithInfo> hls;

                auto current_pos = proj.firstVertex_pos + proj.side_versor;
                while (current_pos != proj.secondVertex_pos)
                {
                    hls.push_back({ {current_pos.x, current_pos.y, 0}, borderSide_descr });
                    current_pos += proj.side_versor;
                }

                HRMdeb.highlight_tiles(hls.begin(), hls.end(), borderSide_color);
            }
        }
    #endif


    static auto compute_projectableBorders(std::unordered_set<Vector2i> const& roofable_microposs)
        -> std::vector<ProjectableBorder>
    {
        std::unordered_set<Vector2i> examined_poss;

        // each projectable border is mapped to its first vertex (the first bumped when walking clockwise around the roof)
        std::vector<ProjectableBorder> projectable_borders;
        
        for (auto pos : roofable_microposs)
        {
            if (examined_poss.find(pos) == examined_poss.cend())
            {
                examined_poss.insert(pos);
        
                check_for_border(pos, roofable_microposs, projectable_borders);
            }
        }


        #if HIPROOFMATRIX_VISUALDEBUG
            for (auto const& proj : projectable_borders)
            {
                std::ostringstream oss;	oss << "Projectable border (id: " << proj.firstVertex_pos << ")";
                HRMdeb.new_step(oss.str(), 3);

                HRMdeb.highlight_tiles(roofable_microposs.cbegin(), roofable_microposs.cend(), 0, Color::Mint);

                visualDebug_highlightProjectableBorder(proj);
            }
            
            std::ostringstream oss;	oss << "Projectable borders (all " << projectable_borders.size() << " together)";
            HRMdeb.new_step(oss.str(), 1);
            
            HRMdeb.highlight_tiles(roofable_microposs.cbegin(), roofable_microposs.cend(), 0, Color::Mint);

            for (auto const& proj : projectable_borders)
                visualDebug_highlightProjectableBorder(proj);
        #endif

        return projectable_borders;
    }



    
    ////////////////////////////////////

    //	   Roof matrix projection	  //

    ////////////////////////////////////
    
    
    #if HIPROOFMATRIX_VISUALDEBUG
        static auto pick_microtileColor(Microtile const microtile)
        {
            if (microtile.height() == -1)
                return Color::Black;

            bool flat = microtile.flat_inclination();
            bool N = microtile.north_inclination();
            bool E = microtile.east_inclination();
            bool S = microtile.south_inclination();
            bool W = microtile.west_inclination();

            if( flat && !N && !E && !S && !W)
                return Color(0x42d4f4FF);//Cyan
            else if(!flat &&  N && !E && !S && !W)
                return Color(0x808000FF);//Olive
            else if(!flat && !N &&  E && !S && !W)
                return Color(0xf58231FF);//Orange
            else if(!flat && !N && !E &&  S && !W)
                return Color(0xffe119FF);//Yellow
            else if(!flat && !N && !E && !S &&  W)
                return Color(0xfabebeFF);//Pink
            else
                return Color(0x800000FF);//Maroon


                //case RoofPerimeterMicrotileType::ConvexSE:
                //	return Color(0x3cb44bFF);//Green
                //	break;
                //case RoofPerimeterMicrotileType::ConcaveSE:
                //	return Color::Mint;
                //	break;
                //case RoofPerimeterMicrotileType::Junction_SE_NW:
                //	return Color(0x42d4f4FF);//Cyan
                //	break;
                //case RoofPerimeterMicrotileType::Junction_NE_SW:
                //	return Color(0x469990FF);//Teal
        }
    #endif
        

    static auto create_roofMatrix(std::vector<Vector3i> const& roofable_poss, int const map_length, int const map_width) -> Matrix
    {
        #if DYNAMIC_ASSERTS
            if (roofable_poss.empty()) { throw std::runtime_error("Wrong argument. Cannot create a roof matrix when the set of roofable positions is empty."); }
        #endif

        int min_x = map_length;
        int max_x = 0;
        int min_y = map_width;
        int max_y = 0;

        for (auto pos : roofable_poss)
        {
            if (min_x > pos.x)
                min_x = pos.x;

            if (max_x < pos.x)
                max_x = pos.x;

            if (min_y > pos.y)
                min_y = pos.y;

            if (max_y < pos.y)
                max_y = pos.y;	
        }

        return Matrix{ min_x, max_x, min_y, max_y };
    }


    ////
    //
    ////
    static bool has_twoConsecutiveNeighbors_with_thatInclination(Vector2i const micro_pos, Matrix const& matrix, MicrotileInclination const inclin)
    {
        bool was_previous_ok = false;

        //From North counterwise up to North again. North is walked two times to see whether NW and N are consecutive
        for (int neighbor_id = 0; neighbor_id < 9; ++neighbor_id)
        {
            auto nghb = matrix.get_neighbor(micro_pos, neighbor_id);
            if (nghb.has_inclination(inclin))
            {
                if (!was_previous_ok)
                    was_previous_ok = true;
                else
                    return true;
            }
            else
            {
                was_previous_ok = false;
            }
        }
        
        return false;
    }

    static auto project_borders(std::vector<ProjectableBorder> & projectable_borders, Matrix & roof_matrix)
    {
        for (auto const& proj : projectable_borders)
        {
            roof_matrix.project_border(proj.firstVertex_pos,  proj.firstVertex_versor, proj.secondVertex_pos, proj.secondVertex_versor, proj.side_versor);

            #if HIPROOFMATRIX_VISUALDEBUG_PROJECT_BORDERS
                HRMdeb.new_step("Border projected", 3);
                HRMdeb.print_matrix(roof_matrix);
            #endif
        }
        
        
        // Needed to solve a flaw of the algorithm
        for (int y = 0; y < roof_matrix.width(); ++y)
        {
            for(int x = 0; x < roof_matrix.length(); ++x)
            {
                Vector2i micro_pos{ x,y };

                auto & micro = roof_matrix.get(micro_pos);

                if (micro.has_multiple_inclinations())
                {
                    if (micro.south_inclination() && !has_twoConsecutiveNeighbors_with_thatInclination(micro_pos, roof_matrix, MicrotileInclination::South))
                        micro.remove_inclination(MicrotileInclination::South);

                    if (micro.west_inclination() && !has_twoConsecutiveNeighbors_with_thatInclination(micro_pos, roof_matrix, MicrotileInclination::West))
                        micro.remove_inclination(MicrotileInclination::West);

                    if (micro.north_inclination() && !has_twoConsecutiveNeighbors_with_thatInclination(micro_pos, roof_matrix, MicrotileInclination::North))
                        micro.remove_inclination(MicrotileInclination::North);

                    if (micro.east_inclination() && !has_twoConsecutiveNeighbors_with_thatInclination(micro_pos, roof_matrix, MicrotileInclination::East))
                        micro.remove_inclination(MicrotileInclination::East);
                }
            }
        }

        
        #if HIPROOFMATRIX_VISUALDEBUG
            HRMdeb.new_step("After the fix.", 1);
            HRMdeb.print_matrix(roof_matrix);
        #endif
    }
    



    ////////////////////////////////////

    //		  Compute polygons	      //

    ////////////////////////////////////
    
    struct sfVector3i_compare
    {
        bool operator()(Vector3i const lhs, Vector3i const rhs) const
        {
            std::tuple<int, int, int> lhs_tuple{ lhs.x, lhs.y, lhs.z };
            std::tuple<int, int, int> rhs_tuple{ rhs.x, rhs.y, rhs.z };

            return lhs_tuple < rhs_tuple;
        }
    };


    struct Polygon
    {
        std::vector<Vector3i> vertices;
        MicrotileInclination inclination;
    };
    bool operator==(Polygon const& lhs, Polygon const& rhs)
    {
        //if (lhs.vertices.size() == rhs.vertices.size())
        //{
        //	auto lhs_first = lhs.vertices.cend();
        //	auto rhs_first = rhs.vertices.cend();

        //	for (auto lhs_it = lhs.vertices.cbegin(); lhs_it != rhs.vertices.cend(); ++lhs_it)
        //	{
        //		auto rhs_it = std::find(rhs.vertices.cbegin(), rhs.vertices.cend(), *lhs_it);

        //		if (rhs_it != rhs.vertices.cend())
        //		{
        //			lhs_first = lhs_it;
        //			rhs_first = rhs_it;
        //			break;
        //		}
        //	}

        //	bool are_vertices_equals = false;

        //	if (lhs_first != lhs.vertices.cend() && rhs_first != rhs.vertices.cend())
        //	{
        //		auto lhs_it = lhs_first;
        //		auto rhs_it = lhs_first;

        //		for ()
        //	}
        //}

        std::set<Vector3i, sfVector3i_compare> lhs_set{ lhs.vertices.cbegin(), lhs.vertices.cend() };
        std::set<Vector3i, sfVector3i_compare> rhs_set{ rhs.vertices.cbegin(), rhs.vertices.cend() };

        return lhs_set == rhs_set && lhs.inclination == rhs.inclination;
    }
    auto operator<<(std::ostream & os, Polygon const& pg) -> std::ostream &
    {
        os << "Polygon { ";
        for (auto v : pg.vertices)
        {
            os << v << ", ";
        }

        os << pg.inclination << "}";

        return os;
    }
    struct PolygonHasher
    {
        std::size_t operator()(Polygon const& pg) const
        {
            // Needed to generate the same hash for polygons
            std::set<Vector3i, sfVector3i_compare> vset{ pg.vertices.cbegin(), pg.vertices.cend() };


            std::size_t seed = 0;


            std::hash<Vector3i> v3i_hasher;
            for (auto v : vset)
                ::hash_combine(seed, v3i_hasher(v));


            ::hash_combine(seed, static_cast<int>(pg.inclination));


            return seed;
        }
    };
    
    
    struct StartingVertexInfo
    {
        StartingVertexInfo(Vector2i a_pos, MicrotileInclination a_incl, Vector2i a_starting_versor) :
            pos(a_pos), incl(a_incl), starting_versor(a_starting_versor) { }

        Vector2i pos;
        MicrotileInclination incl;
        Vector2i starting_versor;
    };
    bool operator==(StartingVertexInfo const& lhs, StartingVertexInfo const& rhs)
    {
        return lhs.pos == rhs.pos && lhs.incl == rhs.incl && lhs.starting_versor == rhs.starting_versor;
    }
    bool operator!=(StartingVertexInfo const& lhs, StartingVertexInfo const& rhs)
    {
        return !(lhs == rhs);
    }
    auto operator<<(Logger & lgr, StartingVertexInfo const& svi) -> Logger &
    {
        lgr << "StartingVertexInfo{ pos: " << svi.pos << " -- incl: " << svi.incl << " -- starting_versor: " << svi.starting_versor << " }";

        return lgr;
    }

    struct StartingVertexInfoHasher
    {
        std::size_t operator()(StartingVertexInfo const& svi) const
        {
            std::size_t seed = 0;

            ::hash_combine(seed, svi.pos.x);
            ::hash_combine(seed, svi.pos.y);
            ::hash_combine(seed, static_cast<int>(svi.incl));
            ::hash_combine(seed, svi.starting_versor.x);
            ::hash_combine(seed, svi.starting_versor.y);

            return seed;
        }
    };

    Logger lgr{ std::cout };

    ////
    //	Check whether the neighbor indicated by @nghb_uvecCcwId belongs to the perimeter of the current polygon.
    //	@incoming_unitVector: Direction from the previous position to @current_pos.
    //	@current_is_edge: Indicate whether current_pos is an edge or a vertex (other types are excluded).
    //	@current_pos: The central position of which @nghb_pos is neghbor.
    //	@nghb_uvecCcwId: The counterclockwise id of @nghb_uvec.
    //	@nghb_uvec: The unit vector that move @current_pos to @nghb_pos.
    ////
    static bool does_neighbor_belong_to_perimeter(Vector2i const incoming_unitVector,
                                                  bool const current_is_edge,
                                                  Vector2i const current_pos,
                                                  int const nghb_uvecCcwId,
                                                  Vector2i const nghb_uvec,
                                                  Vector2i const nghb_pos,
                                                  Matrix const& roof_matrix,
                                                  MicrotileInclination const polygon_incl)
    {
        #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
            HRMdeb.new_step("Analyzing this neighbor", 5);
            HRMdeb.highlight_tile(current_pos, 0, Color::Blue);
            HRMdeb.highlight_tile(nghb_pos, 0, Color::Red);
        #endif			

        auto & nghb = roof_matrix.get(nghb_pos);
            
        // The first condition is that the neighbor must share the same inclination of the polygon.
        if (nghb.has_inclination(polygon_incl))
        {			
            if (current_is_edge)
            {
                // After an edge the only valid neighbor is the one that lies in the same direction of previous_unitVector
                if (nghb_uvec == incoming_unitVector)
                {
                    #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                        HRMdeb.new_step("It's the the continuation of the edge.", 5);
                        HRMdeb.highlight_tile(current_pos, 0, Color::Blue);
                        HRMdeb.highlight_tile(nghb_pos, 0, Color::Red);
                    #endif

                    return true;
                }

                //Note: This special handling when current_pos is an edge is due to this special case:
                //		****		****							****
                //		*#**		*V**		V: Vertex			*P**		P: Previous postion
                //		*##*		*EE*		E: Edge				*C#*		C: Current position
                //		*###		*ESE		S: Side				*N##		N: Current neighbor
                //
                //		The general case working for vertices doesn't work in this particular case. In fact not all the 
                //		next neighbors until the previous position belong to the polygon:
                //		****	
                //		*PW*	
                //		*CK*	K: Ok
                //		*NK#	W: Wrong
                //		The next neighbor W doesn't belong to the polygon. So the general case can't be used here. 
                //
                //		Another issue is in the opposite case:
                //		*****		*****							*****									*****
                //		***#*		***V*		V: Vertex			***N*		N: Current neighbor			**WN*		W: Wrong
                //		**##*		**EE*		E: Edge				**#C*		C: Current position			**#C*
                //		*###*		*ESE*		S: Side				*##P*		P: Previous postion			*##P*
                //		The eligible neighbor N would be discarded because the next neighbor W didn't belong to the polygon.
                //		
            }		
            else //current_pos is a vertex
            {
                #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                    HRMdeb.new_step("It's an eligible continuation after the vertex.", 5);
                    HRMdeb.highlight_tile(current_pos, 0, Color::Blue);
                    HRMdeb.highlight_tile(nghb_pos, 0, Color::Red);
                #endif

                // Check if the neighbors that lies (counterclockwise) between this neighbor and the previous position belong to the polygon.
                bool do_nextNghbs_belong_to_polygon = true;
                auto incomingUvec_id = DirectionUtil::planeUnitVector_to_counterclockwiseId(-incoming_unitVector);

                for (int j = nghb_uvecCcwId + 1; j < incomingUvec_id + 8; ++j)
                {
                    auto nextNghb_uvec = DirectionUtil::counterclockwiseId_to_planeUnitVector(j);
                    auto nextNghb_pos = current_pos + nextNghb_uvec;
                    auto & nextNghb = roof_matrix.get(nextNghb_pos);
                    
                    #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                        HRMdeb.new_step("Analyzing the next neighbor.", 5);
                        HRMdeb.highlight_tile(current_pos, 0, Color::Blue);
                        HRMdeb.highlight_tile(nghb_pos, 0, Color::Red);
                        HRMdeb.highlight_tile(nextNghb_pos, 0, Color::Magenta);
                    #endif

                    if (!nextNghb.has_inclination(polygon_incl))
                    {
                        #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                            HRMdeb.new_step("Doesn't have the same inclination.", 5);
                            HRMdeb.highlight_tile(current_pos, 0, Color::Blue);
                            HRMdeb.highlight_tile(nghb_pos, 0, Color::Red);
                            HRMdeb.highlight_tile(nextNghb_pos, 0, Color::Magenta);
                        #endif

                        do_nextNghbs_belong_to_polygon = false;
                        break;
                    }
                }

                if (do_nextNghbs_belong_to_polygon)
                    return true;
            }		
        }

        return false;
    }

    ////
    //  Choose the next microtile belonging to the polygon between the neighbors of @current_pos.
    //	@incoming_unitVector: Direction from the previous position to @current_pos.
    //	@current_is_edge: Indicate whether current_pos is an edge or a vertex (other types are excluded).
    //	@current_pos: The central position of which the neighbors will be examined.
    //	@nghb_uvecCcwId: The counterclockwise id of @nghb_uvec.
    //	@nghb_uvec: The unit vector that move @current_pos to @nghb_pos.
    //
    //	@return: The unit vector that move @current_pos to the next neighbor.
    ////
    static auto choose_nextMicrotile(Vector2i incoming_unitVector,
                                     bool current_is_edge,
                                     Vector2i current_pos,
                                     Matrix const& roof_matrix,
                                     MicrotileInclination const& polygon_incl)
        -> Vector2i
    {
        /*if(visualDebug_runtime_maxRecordableDepth == 4)
            std::cout << "incoming_unitVector: " << incoming_unitVector << " -- current_is_edge: " << std::boolalpha << current_is_edge << std::noboolalpha << " -- current_pos: " << current_pos << std::endl;
*/
        #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
            HRMdeb.new_step("current_pos", 4);
            HRMdeb.highlight_tile(current_pos, 0, Color::Blue);
        #endif


        std::map<int, Vector2i> possible_edges; //possible edges sorted by distance
        std::optional<Vector2i> possible_vertex;

        auto previousUvec_id = DirectionUtil::planeUnitVector_to_counterclockwiseId(-incoming_unitVector);
        
        /*if(visualDebug_runtime_maxRecordableDepth == 4)
            lgr << Logger::nltb << "previousUvec_id: " << previousUvec_id;
            */	

        // Check, counterclockwise, the neighbors until finding the next microtile belonging to the perimeter 
        for (int i = previousUvec_id + 1; i < previousUvec_id + 8; ++i)
        {
            auto nghb_uvec = DirectionUtil::counterclockwiseId_to_planeUnitVector(i);
            auto nghb_pos = current_pos + nghb_uvec;

            if (does_neighbor_belong_to_perimeter(incoming_unitVector, current_is_edge, current_pos, i, nghb_uvec, nghb_pos, roof_matrix, polygon_incl))
            {
                return nghb_uvec;
            }
        }
        

        throw std::runtime_error("Unexpected situation. No suitable neighbor to procede further.");
    }


    ////
    //	Starting from the provided first vertex (@v0_info) traverse all the perimeter until being back in the first vertex. In the meanwhile save all the crossed vertices.
    ////
    static auto compute_polygon(StartingVertexInfo const v0_info, Matrix const& roof_matrix)
        -> Polygon
    {
        Polygon polygon{ {}, v0_info.incl };


        auto & v0_micro = roof_matrix.get(v0_info.pos);
        polygon.vertices.push_back({ v0_info.pos.x, v0_info.pos.y, v0_micro.height() });

        
        bool has_v0_been_reached = false;
        Vector2i incoming_unitVector = v0_info.starting_versor;

        Vector2i current_pos = v0_info.pos + v0_info.starting_versor;

        auto current_type = roof_matrix.get(current_pos).type();
        bool current_is_edge = current_type == MicrotileType::Edge;
        if (current_type == MicrotileType::Vertex)
        {
            //TODO: For what I've tested until now it doesn't seem a problem if the microtile after the starting vertex is itself a vertex. But there may be some extreme cases
            //		in which the algorithm doesn't work as expected. Keep an eye out.
            auto & current_micro = roof_matrix.get(current_pos);
            polygon.vertices.push_back({ current_pos.x, current_pos.y, current_micro.height() });
        }

        #if DYNAMIC_ASSERTS
            if (current_type != MicrotileType::Edge && current_type != MicrotileType::Vertex)
            {
                throw std::runtime_error("The first microtile after a starting vertex must be an edge or a vertex.");
            }
        #endif

        while (!has_v0_been_reached)
        {
            auto outgoing_unitVector = choose_nextMicrotile(incoming_unitVector, current_is_edge, current_pos, roof_matrix, polygon.inclination);
            auto next_pos = current_pos + outgoing_unitVector;
            auto next = roof_matrix.get(next_pos);

            if (next_pos == v0_info.pos) // if the next position of the perimeter coincide with the first vertex of the polygon, we are done.
            {
                has_v0_been_reached = true;
            }
            else
            {
                auto next_type = next.type();

                if (next_type == MicrotileType::Edge)
                {
                    //if (visualDebug_runtime_maxRecordableDepth == 4)
                    //	lgr << Logger::nltb << "Edge!";
                    
                    current_is_edge = true;
                }
                else if (next_type == MicrotileType::Vertex)
                {
                    //if (visualDebug_runtime_maxRecordableDepth == 4)
                    //	lgr << Logger::nltb << "Vertex!";
                    
                    current_is_edge = false;
                    polygon.vertices.push_back({ next_pos.x, next_pos.y, next.height() });
                }
                else
                {
                    throw std::runtime_error("Unexpected type for a polygon perimeter microtile.");
                }
            }

            incoming_unitVector = outgoing_unitVector;
            current_pos = next_pos;
        }


        return polygon;
    }

    ////
    //	Check if @pos is a vertex suitable as first vertex of one or more polygons. If that is the case the vertex/vertices is/are added at @starting_vertices.
    ////
    static void add_suitable_startingVertices(Vector2i const pos, Matrix const& roof_matrix, std::unordered_set<StartingVertexInfo, StartingVertexInfoHasher> & starting_vertices)
    {
        auto & micro = roof_matrix.get(pos);

        if (micro.height() == 0 && micro.type() == MicrotileType::Vertex)
        {
            #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                std::vector<StartingVertexInfo> deb_standard_cases;
                std::vector<StartingVertexInfo> deb_special_cases;
            #endif

            if (micro.south_inclination())
            {
                Vector2i uvec1{  0, -1 };
                auto & nghb = roof_matrix.get(pos + uvec1);
                if (nghb.height() == 0 && nghb.south_inclination())
                {
                    starting_vertices.insert({ pos, MicrotileInclination::South, uvec1 });

                    #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                        deb_standard_cases.push_back({ pos, MicrotileInclination::South, uvec1 });
                    #endif
                }

                // Special case
                Vector2i uvec2{  1, -1 };
                auto & nghb2 = roof_matrix.get(pos + uvec2);
                auto & nghbE = roof_matrix.get(pos + Vector2i{  1,  0 });
                if (   nghb2.height() == 1
                    && (nghb2.inclinations() & (Microtile::south | Microtile::west)).count() == 2 
                    && (nghbE.inclinations() & Microtile::south).any())
                {
                    starting_vertices.insert({ pos, MicrotileInclination::South, uvec2 });

                    #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                        deb_special_cases.push_back({ pos, MicrotileInclination::South, uvec2 });
                    #endif
                }
            }
                
            if (micro.west_inclination())
            {
                Vector2i uvec1{ -1,  0 };
                auto & nghb = roof_matrix.get(pos + uvec1);
                if (nghb.height() == 0 && nghb.west_inclination())
                {
                    starting_vertices.insert({ pos, MicrotileInclination::West, uvec1 });

                    #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                        deb_standard_cases.push_back({ pos, MicrotileInclination::West, uvec1 });
                    #endif
                }

                // Special case
                Vector2i uvec2{ -1, -1 };
                auto & nghb2 = roof_matrix.get(pos + uvec2);
                auto & nghbE = roof_matrix.get(pos + Vector2i{  0, -1 });
                if (   nghb2.height() == 1
                    && (nghb2.inclinations() & (Microtile::north | Microtile::west)).count() == 2 
                    && (nghbE.inclinations() & Microtile::west).any())
                {
                    starting_vertices.insert({ pos, MicrotileInclination::West, uvec2 });

                    #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                        deb_special_cases.push_back({ pos, MicrotileInclination::West, uvec2 });
                    #endif
                }
            }
                
            if (micro.north_inclination())
            {
                Vector2i uvec1{  0,  1 };
                auto & nghb = roof_matrix.get(pos + uvec1);
                if (nghb.height() == 0 && nghb.north_inclination())
                {
                    starting_vertices.insert({ pos, MicrotileInclination::North, uvec1 });

                    #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                        deb_standard_cases.push_back({ pos, MicrotileInclination::North, uvec1 });
                    #endif
                }

                // Special case
                Vector2i uvec2{ -1,  1 };
                auto & nghb2 = roof_matrix.get(pos + uvec2);
                auto & nghbN = roof_matrix.get(pos + Vector2i{ -1,  0 });
                if (   nghb2.height() == 1
                    && (nghb2.inclinations() & (Microtile::north | Microtile::east)).count() == 2 
                    && (nghbN.inclinations() & Microtile::north).any())
                {
                    starting_vertices.insert({ pos, MicrotileInclination::North, uvec2 });
                    
                    #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                        deb_special_cases.push_back({ pos, MicrotileInclination::North, uvec2 });
                    #endif
                }
            }
                
            if (micro.east_inclination())
            {
                Vector2i uvec1{  1,  0 };
                auto & nghb = roof_matrix.get(pos + uvec1);
                if (nghb.height() == 0 && nghb.east_inclination())
                {
                    starting_vertices.insert({ pos, MicrotileInclination::East, uvec1 });
                    
                    #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                        deb_standard_cases.push_back({ pos, MicrotileInclination::East, uvec1 });
                    #endif
                }

                // Special case
                Vector2i uvec2{  1,  1 };
                auto & nghb2 = roof_matrix.get(pos + uvec2);
                auto & nghbE = roof_matrix.get(pos + Vector2i{  0,  1 });
                if (   nghb2.height() == 1
                    && (nghb2.inclinations() & (Microtile::south | Microtile::east)).count() == 2 
                    && (nghbE.inclinations() & Microtile::east).any())
                {
                    starting_vertices.insert({ pos, MicrotileInclination::East, uvec2 });
                    
                    #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                        deb_special_cases.push_back({ pos, MicrotileInclination::East, uvec2 });
                    #endif
                }
            }



            #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                // Note: There are situations in which there is no standard case and situations in which there can be more than one standard case.
                //		 Typically such situations occurs when vertex (with height 0) is between two opposite inclinations (for example when is the 
                //		 vertex of an inverted pyramid and all the surrounding microtile are of height 1, but could also happen in other situations).

                for (int i = 0; i < deb_standard_cases.size() ; ++i)
                {
                    std::ostringstream oss;
                    oss << "Starting vertex - standard case #" << i;
                    HRMdeb.new_step(oss.str(), 4);
                    HRMdeb.print_matrix(roof_matrix);
                    HRMdeb.highlight_tile(deb_standard_cases[i].pos, 0, Color::Red);
                    HRMdeb.highlight_tile(deb_standard_cases[i].pos + deb_standard_cases[i].starting_versor, 0, Color::Blue);
                }
                
                for (int i = 0; i < deb_special_cases.size() ; ++i)
                {
                    std::ostringstream oss;
                    oss << "Starting vertex - special case #" << i;
                    HRMdeb.new_step(oss.str(), 4);
                    HRMdeb.print_matrix(roof_matrix);
                    HRMdeb.highlight_tile(deb_special_cases[i].pos, 0, Color::Red);
                    HRMdeb.highlight_tile(deb_special_cases[i].pos + deb_special_cases[i].starting_versor, 0, Color::Blue);
                }
            #endif
        }
    }
    

    ////
    //	Starting from the @roof_matrix generate all the polygons representing the roof sides.
    ////
    static auto compute_polygons(Matrix const& roof_matrix) -> std::unordered_set<Polygon, PolygonHasher>
    {
        std::unordered_set<Polygon, PolygonHasher> polygons;
        std::unordered_set<StartingVertexInfo, StartingVertexInfoHasher> starting_vertices;

        // Find all the suitable starting vertices
        for (int y = 0; y < roof_matrix.width(); ++y)
            for (int x = 0; x < roof_matrix.length(); ++x)
                add_suitable_startingVertices({ x, y }, roof_matrix, starting_vertices);



        #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
            HRMdeb.new_step("Starting vertices", 3);
            HRMdeb.print_matrix(roof_matrix);
            
            std::vector<Vector2i> debug_starting_vertices;
            std::vector<Vector2i> debug_starting_versors;
            for (auto v0frac : starting_vertices)
            {
                debug_starting_vertices.push_back(v0frac.pos);
                debug_starting_versors.push_back(v0frac.pos + v0frac.starting_versor);
            }
            HRMdeb.highlight_tiles(debug_starting_vertices.begin(), debug_starting_vertices.end(), 0, Color::Red);
            HRMdeb.highlight_tiles(debug_starting_versors.begin(), debug_starting_versors.end(), 0, Color::Blue);
        #endif


        for(auto starting_vertex : starting_vertices)
        {
            #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                auto [deb_it, deb_success] = /*polygons.insert(compute_polygon(starting_vertex, roof_matrix));  //expected following line  */ 
            #endif
                
            polygons.insert(compute_polygon(starting_vertex, roof_matrix));	
                    
            #if HIPROOFMATRIX_VISUALDEBUG_COMPUTE_POLYGONS
                std::ostringstream oss;
                oss << "Roof polygon";
                if (!deb_success)
                    oss << "  --  The polygon already existed";
                HRMdeb.new_step(oss.str(), 3);
                HRMdeb.print_matrix(roof_matrix);
                HRMdeb.highlight_tilePolygon(deb_it->vertices.cbegin(), deb_it->vertices.cend(), 0, Color::Red);
            #endif
        }

        
        #if HIPROOFMATRIX_VISUALDEBUG
            HRMdeb.new_step("All polygons: ", 1);
            HRMdeb.print_matrix(roof_matrix);
            for(auto const& poly : polygons)
                HRMdeb.highlight_tilePolygon(poly.vertices.cbegin(), poly.vertices.cend(), 0, Color::Red);
        #endif


        return polygons;
    }
    //TODO: 01: Ricontrolla un'ultima volta questa funzione e fai in modo che funzioni con la nuova unità di misura
    static auto draw_polygons(std::unordered_set<Polygon, PolygonHasher> const& polygons, Matrix const& roof_matrix, int const building_zLevel) -> RoofPolygons
    {
        RoofPolygons ret;

        for (auto const& poly : polygons)
        {
            auto & free_polygon = ret.add_polygon(poly.inclination);
            

            // Compute polygon origin (each vertex position is computed in relation to this origin)
            Vector2f origin{ std::numeric_limits<float>::max(), 
                             std::numeric_limits<float>::max() }; // a value reasonably bigger than any roof_matrix it'll be ever created

            for (auto const v : poly.vertices)
            {
                auto const vertex = roof_matrix.microtile_to_mapUnits({ v.x, v.y });

                origin.x = std::min(origin.x, vertex.x);
                origin.y = std::min(origin.y, vertex.y);
            }
            auto const origin_z = GSet::tiles_to_units(building_zLevel + 1);

            free_polygon.set_pos({ origin.x, origin.y, origin_z });

            
            // Compute OpenGL wolrd reference coordinates for polygon vertices
            for (auto const v : poly.vertices)
            {
                auto const z = roof_matrix.get(v.x, v.y).height();

                // Relative positions (in units) of the triangle vertices from the polygon origin.
                auto v_rPos = roof_matrix.microtile_to_mapUnits({v.x, v.y}) - origin;

                // Relative heights (in units) of the triangle vertices from the polygon origin.
                auto const v_rZ = GSet::tiles_to_units(z / 2.f); //conversion from matrix heights to units (map reference system)

                // Each polygon has its own texture. The origin of the texture coincide to the origin of the polygon. Then some kind of symmetrical transformation
                // is applied depending on the inclination of polygon (i.e. the roof tiles must go in the direction of the water).

                //TODO: The code works fine, but the problem is that using a lower resolution texture
                //		for the roofs (even with lower screen resolutions) produces an ugly effect. That's in part because 
                //		of the poor quality of the present texture and in part because the roofVertex_wySliding_ratio 
                //		is applied after the computation of the texture.
                //		Thus the texture (already being low in quality) is also stretched along the map x-axis, producing an 
                //		even lower-quality output. Maybe I should use 4 different textures (pre-stretched) for each roof side.
                Vector2f const tex = v_rPos / GSet::upt * GSet::pptxu(); //from "units" to "tiles" and then to "texture pixels"
                //Vector2f tex = v_rPos / GSet::upt * GSet::ultraHighDefinition_pptxu;	//alternative code if I decide to use the ultrahd texture with any TextureDefinition

                Vector2f adj_tex;
                switch (poly.inclination)
                {
                    case MicrotileInclination::North:
                    {
                        adj_tex = { tex.y, tex.x };
                        break;
                    }
                    case MicrotileInclination::East:
                    {
                        adj_tex = { tex.x, -tex.y };
                        break;
                    }
                    case MicrotileInclination::South:
                    {
                        adj_tex = { tex.y, -tex.x };
                        break;
                    }
                    case MicrotileInclination::West:
                    {
                        adj_tex = tex;
                        break;
                    }

                    case MicrotileInclination::Flat:
                    default:
                        throw std::runtime_error("Unexpected MicrotileInclination.");
                        break;
                }


                // Each vertex must be shifted along map x-axis to simulate height.
                v_rPos.x -= v_rZ * GSet::roofVertex_wySliding_ratio();

                // The roof must be shifted along the map x-axis to perfectly fit over the walls.
                v_rPos.x += GSet::upt * GSet::roof_wyOffset();


                free_polygon.push_vertex({ v_rPos.x, v_rPos.y, v_rZ , adj_tex.x * 4, adj_tex.y * 4 });
            }
        }
            
        return ret;
    }


    auto generate_hipRoof(std::vector<Vector3i> const& roofable_poss, int const z_floor, int const map_length, int const map_width) 
        -> RoofPolygons
    {
        //TODO: PERFORMANCE: Here there are a lot of nested return-by-value. There should be a combination of copy-elision and move. Be sure that no copy is made.
        RoofPolygons free_polygons; //NRVO

        if(!roofable_poss.empty())
        {
            auto roof_matrix = create_roofMatrix(roofable_poss, map_length, map_width);
        
            #if HIPROOFMATRIX_VISUALDEBUG
                HRMdeb.begin_chapter("Roof matrix filled");
                HRMdeb.set_matrixDimensions(roof_matrix.length(), roof_matrix.width());
                HRMdeb.print_matrix(roof_matrix);
            #endif

            //TODO: PERFORMANCE: Maybe using directly the roof_matrix instead of creating an additional unordered_set there could be a performance gain.
            //					 The tradeoff between vector lookup and unordered_set lookup is negligible but it would spare space from not creating the
            //					 additional unordered set. I would exchange an unordered_set insertion for a vector element update.
            auto roofable_microtiles = compute_roofableMicrotiles(roofable_poss, roof_matrix);

            fill_roofMatrix(roofable_microtiles, roof_matrix);

            auto projectableBorders = compute_projectableBorders(roofable_microtiles);

            project_borders(projectableBorders, roof_matrix);

            auto polygons = compute_polygons(roof_matrix);
        
            free_polygons = draw_polygons(polygons, roof_matrix, z_floor);

            #if HIPROOFMATRIX_VISUALDEBUG
                HRMdeb.end_chapter();
            #endif
        }

        return free_polygons;
    }

} //namespace HipRoofAlgorithm



} //namespace tgm