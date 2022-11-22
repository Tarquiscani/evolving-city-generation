#ifndef GM_HIP_ROOF_ALGORITHM_HH
#define GM_HIP_ROOF_ALGORITHM_HH


#include "data_strctures/data_array.hh"
#include "hip_roof_microtile.hh"
#include "graphics/roof_vertices.hh"
#include "map/map_forward_decl.hh"
#include "map/tiles/tile_set.hh"
#include "map/buildings/building.hh"


namespace tgm
{



namespace HipRoofAlgorithm
{
	struct RoofPolygons
	{
		RoofPolygons() = default;

		// I don't want this class to be copied, it's too expensive
		RoofPolygons(RoofPolygons const&) = delete;
		auto operator=(RoofPolygons const&) -> RoofPolygons & = delete;

		RoofPolygons(RoofPolygons&&) = default;
		auto operator=(RoofPolygons&&) -> RoofPolygons & = default;

		auto add_polygon(MicrotileInclination const incl) -> FreePolygon &
		{
			switch (incl)
			{
				case MicrotileInclination::South:
				{
					return south.emplace_back();
					break;
				}
				case MicrotileInclination::West:
				{
					return west.emplace_back();
					break;
				}
				case MicrotileInclination::North:
				{
					return north.emplace_back();
					break;
				}
				case MicrotileInclination::East:
				{
					return east.emplace_back();
					break;
				}

				case MicrotileInclination::Flat:
				default:
					throw std::runtime_error("Unexpected MicrotileInclination.");
					break;
			}
		}

		std::vector<FreePolygon> south;
		std::vector<FreePolygon> west;
		std::vector<FreePolygon> north;
		std::vector<FreePolygon> east;
	};

	auto operator<<(Logger & lgr, RoofPolygons const& rp) -> Logger &;
	


	auto generate_hipRoof(std::vector<Vector3i> const& roofable_poss, int const z_floor, int const map_length, int const map_width) 
		-> RoofPolygons;

	
	//TODO: Not all of these cases actually exist. That's due to the legacy system that checked the Tiles configuration
	//		in the present system the Microtiles are used and the number of cases are way less.
	struct RoofPerimeterMicrotileTypeStatistics
	{
		long long None = 0;
		long long SideS_1 = 0;
		long long SideS_2 = 0;
		long long SideS_3 = 0;
		long long SideS_4 = 0;
		long long ConvexSW_1 = 0;
		long long ConvexSW_2 = 0;
		long long ConvexSW_3 = 0;
		long long ConvexSW_4 = 0;
		long long ConcaveSW = 0;
		long long SideW_1 = 0;
		long long SideW_2 = 0;
		long long SideW_3 = 0;
		long long SideW_4 = 0;
		long long ConvexNW_1 = 0;
		long long ConvexNW_2 = 0;
		long long ConvexNW_3 = 0;
		long long ConvexNW_4 = 0;
		long long ConcaveNW = 0;
		long long SideN_1 = 0;
		long long SideN_2 = 0;
		long long SideN_3 = 0;
		long long SideN_4 = 0;
		long long ConvexNE_1 = 0;
		long long ConvexNE_2 = 0;
		long long ConvexNE_3 = 0;
		long long ConvexNE_4 = 0;
		long long ConcaveNE = 0;
		long long SideE_1 = 0;
		long long SideE_2 = 0;
		long long SideE_3 = 0;
		long long SideE_4 = 0;
		long long ConvexSE_1 = 0;
		long long ConvexSE_2 = 0;
		long long ConvexSE_3 = 0;
		long long ConvexSE_4 = 0;
		long long ConcaveSE = 0;
		long long Junction_SE_NW = 0;
		long long Junction_NE_SW = 0;
		long long TangencyNE = 0;
		long long TangencySE = 0;
		long long TangencySW = 0;
		long long TangencyNW = 0;
		long long Foot_N_W = 0;
		long long Foot_N_E = 0;
		long long Foot_E_N = 0;
		long long Foot_E_S = 0;
		long long Foot_S_E = 0;
		long long Foot_S_W = 0;
		long long Foot_W_S = 0;
		long long Foot_W_N = 0;

		long long unexpected = 0;
	};
	auto operator<<(std::ostream & os, RoofPerimeterMicrotileTypeStatistics const & rptts) -> std::ostream &;

	inline RoofPerimeterMicrotileTypeStatistics roofPerimeterMicrotileType_stats{};

} //namespace HipRoofAlgorithm



} //namespace tgm
using namespace tgm;


#endif //GM_HIP_ROOF_ALGORITHM_HH