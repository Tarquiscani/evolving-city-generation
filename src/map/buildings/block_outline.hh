#ifndef GM_BLOCK_OUTLINE_HH
#define GM_BLOCK_OUTLINE_HH


#include <vector>
#include <utility>

#include "system/vector3.hh"
#include "map/tiles/tile_set.hh"

namespace tgm
{


    
struct OutlinePivot
{
    OutlinePivot() = default;
    OutlinePivot(Vector3i const a_pos, Vector3i const a_drc) : pos{ a_pos }, drc{ a_drc } {}

    Vector3i pos{};
    Vector3i drc{};
};

inline bool operator==(OutlinePivot const& lhs, OutlinePivot const& rhs) 
{ 
    return lhs.pos == rhs.pos && lhs.drc == rhs.drc; 
}

inline bool operator!=(OutlinePivot const& lhs, OutlinePivot const& rhs) 
{ 
    return !(lhs == rhs); 
}


    
struct BlockOutlineSegment
{
    Vector3i begin{};
    Vector3i drc{};
    int length = 0;

    auto end() const { return begin + drc * length; }
};

struct BlockOutline
{
    std::vector<BlockOutlineSegment> segments{};
    std::vector<Vector3i> external_doors{};

    int concave_count = 0;
    int convex_count = 0;

    void assert_angleCount() const;

    bool is_internal() const { return concave_count - 4 == convex_count; }

    bool does_contain_border(Vector3i const pos) const
    {
        for (auto const& segment : segments)
        {
            auto const left = DirectionUtil::orthogonalLeft_planeUnitVector(segment.drc);
            for (auto p = segment.begin + left; p != segment.end() + left; p += segment.drc)
            {
                if (p == pos) { return true; }
            }
        }

        return false;
    }
};



}


#endif //GM_BLOCK_OUTLINE_HH