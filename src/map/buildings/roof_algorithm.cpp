#include "roof_algorithm.hh"


namespace tgm
{



namespace RoofAlgorithm
{

    static auto get_area(BuildingAreaCompleteId const acid, DataArray<Building> const& buildings) -> BuildingArea const&
    {
        auto const& b = buildings.get_or_throw(acid.bid);
        auto const& a = b.getOrThrow_area(acid.aid);

        return a;
    }

    static auto compute_roofablePositions(BuildingId const bid, std::unordered_set<Vector3i> const& starting_positions, DataArray<Building> const& buildings, TileSet const& tiles)
        -> std::unordered_set<Vector3i>
    {
        std::unordered_set<Vector3i> roofable_poss;

    
        std::unordered_set<Vector3i> examined_poss;
        std::unordered_set<Vector3i> pending_poss = starting_positions;

        // Since for each tile it's required to check whether its associated area is roofable or not, it's useful to cache those 
        // information to avoid multiple accesses to the Building, BuildingArea and AreaTemplate.
        std::unordered_map<BuildingAreaCompleteId, bool> isAreaRoofable_cache;		


        while (!pending_poss.empty())
        {
            auto current_pos = *pending_poss.begin();

            examined_poss.insert(current_pos);
            
            auto const current_t = tiles.get(current_pos);

            if (current_t && current_t->is_built() && !current_t->is_roofed_for(bid))
            {
                bool is_roofable_for_bid = false;

                for (auto const acid : current_t->areas())
                {
                    if (acid.bid == bid)
                    {
                        auto it = isAreaRoofable_cache.find(acid);
                        // if the area roofability isn't cached then cache it
                        if (it == isAreaRoofable_cache.cend())
                        {
                            auto const& b = buildings.get_or_throw(acid.bid);
                            auto const& a = b.getOrThrow_area(acid.aid);

                            auto const& atempl = area_templates.at(a.type());

                            isAreaRoofable_cache.insert({ acid, atempl.is_roofable() });

                            if (atempl.is_roofable()) {	is_roofable_for_bid = true; }
                        }
                        else
                        {
                            if (it->second)	{ is_roofable_for_bid = true; }
                        }
                    }
                }

                if (is_roofable_for_bid)
                {
                    roofable_poss.insert(current_pos);

                    auto const N_pos = current_pos + Vector3i{ -1,  0, 0 };
                    auto const E_pos = current_pos + Vector3i{  0,  1, 0 };
                    auto const S_pos = current_pos + Vector3i{  1,  0, 0 };
                    auto const W_pos = current_pos + Vector3i{  0, -1, 0 };

                    if (examined_poss.find(N_pos) == examined_poss.cend()) { pending_poss.insert(N_pos); }
                    if (examined_poss.find(E_pos) == examined_poss.cend()) { pending_poss.insert(E_pos); }
                    if (examined_poss.find(S_pos) == examined_poss.cend()) { pending_poss.insert(S_pos); }
                    if (examined_poss.find(W_pos) == examined_poss.cend()) { pending_poss.insert(W_pos); }
                }
            }

            pending_poss.erase(current_pos);
        }
        

        #if BUILDEXP_VISUALDEBUG_ROOF_GENERATION
            BEdeb.new_step("Roofable positions", 2);

            BEdeb.highlight_tiles(roofable_poss.cbegin(), roofable_poss.cend(), Color::Red);
        #endif

        return roofable_poss;
    }

    auto compute_roofablePositions_fromArea(BuildingAreaCompleteId const starting_area, DataArray<Building> const& buildings, TileSet const& tiles)
        -> std::unordered_set<Vector3i>
    {
        std::unordered_set<Vector3i> starting_positions;

        auto starting_vol = get_area(starting_area, buildings).volume();
        for (auto y = starting_vol.left; y < starting_vol.right(); ++y)
        {
            for (auto x = starting_vol.behind; x < starting_vol.front(); ++x)
            {
                starting_positions.insert({ x, y, starting_vol.down });
            }
        }

        return compute_roofablePositions(starting_area.bid, starting_positions, buildings, tiles);
    }
    
    auto compute_roofablePositions_fromTile(BuildingId const bid, Vector3i const starting_pos, DataArray<Building> const& buildings, TileSet const& tiles)
        -> std::unordered_set<Vector3i>
    {
        return compute_roofablePositions(bid, { starting_pos }, buildings, tiles);
    }

} //namespace RoofAlgorithm



} //namespace tgm