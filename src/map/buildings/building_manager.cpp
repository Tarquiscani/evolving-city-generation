#include "building_manager.hh"


#include "map/buildings/roof_algorithm.hh"
#include "settings/simulation/simulation_settings.hh"
#include "utilities.hh"

#include "debug/visual/building_expansion_stream.hh"
#include "debug/visual/hip_roof_matrix_stream.hh"


namespace tgm
{



BuildingManager::BuildingManager(std::mt19937 & random_generator, TileSet & tiles, DataArray<Building> & buildings, DoorManager & door_manager, 
                                 TileGraphicsMediator & tg_mediator, RoofGraphicsMediator & rg_mediator) :
    m_random_generator{ random_generator },
    m_tiles{ tiles },
    m_buildings{ buildings },
    m_door_manager{ door_manager },
    m_tgraphics_mediator{ tg_mediator },
    m_rgraphics_mediator{ rg_mediator }
{
    #if BUILDEXP_VISUALDEBUG
        BEdeb.start(tiles.length(), tiles.width(), tiles.height(), m_buildings);
    #endif

    #if HIPROOFMATRIX_VISUALDEBUG
        HRMdeb.start(tiles.length(), tiles.width());
    #endif
}

BuildingManager::~BuildingManager()
{
    #if VISUALDEBUG
        try {
        #if BUILDEXP_VISUALDEBUG
            BEdeb.stop();
        #endif

        #if HIPROOFMATRIX_VISUALDEBUG
            HRMdeb.stop();
        #endif
        } catch (std::exception const& e) {
            std::cout << "Error stopping VisualDebug: " << e.what();
            std::terminate();
        }
    #endif
}



auto BuildingManager::get_nearestCity(Vector2f const pos) -> CityId
{
    auto nearest_cid = CityId{0};
    auto current_distance = std::numeric_limits<float>::max();
    for (auto const& [cid, c] : m_cities)
    {
        auto const d = distance(compute_cityCenter(c), pos);
        if (d < current_distance)
        {
            current_distance = d;
            nearest_cid = cid;
        }
    }
    
    // If there are no cities, then build one
    if (nearest_cid == 0)
    {
        auto const& city_el = m_cities.create();
        nearest_cid = city_el.id();
    }

    return nearest_cid;
}

auto BuildingManager::compute_cityCenter(City const& city) const -> Vector2f
{
    auto center = Vector2f{};

    for (auto const cbid : city.blocks())
    {
        auto const block_center = m_blocks.get_or_throw(cbid).center();

        center += block_center;
    }

    auto block_count = static_cast<float>(city.blocks().size());
    center /= block_count;

    return center;
}

auto BuildingManager::buildBuilding_inNearestCity(BuildingRecipe const& recipe) -> std::optional<BuildingId> 
{
    auto ret = std::optional<BuildingId>{};

    #if BUILDEXP_VISUALDEBUG
        BEdeb.begin_chapter("Building building in the nearest city.", Vector3i{Vector3f{recipe.proposed_position().x, recipe.proposed_position().y, 0.f}});
        BEdeb.focus_onPosition(recipe.proposed_position());
    #endif

    auto new_bid = BuildingId{0};
    
    auto const nearest_cid = get_nearestCity(recipe.proposed_position());
    
    new_bid = build_building_inCity(recipe, nearest_cid);
    //if (new_bid == 0) { throw std::runtime_error("Can't build in this city."); }
    if (new_bid != 0)
    {
        ret.emplace(new_bid);
    }
    
    
    #if BUILDEXP_VISUALDEBUG
        BEdeb.end_chapter();
    #endif

    
    return new_bid;
}

auto BuildingManager::build_firstBuilding_inCity(CityId const cid, CityBlockId const cbid, 
                                                 Vector3i const pos, BuildingRecipe const& recipe, 
                                                 std::vector<BuildingAreaCompleteId> const& replaceable_areas,
                                                 CityBlock & cblock) -> BuildingId
{
    auto new_bid = BuildingId{0};

    auto [buildable, replaced_areas] = is_area_buildable(cbid, pos, recipe.startingArea_dims(), replaceable_areas);
    if (buildable)
    {
        new_bid = internal_build_building(cid, cbid, pos, recipe, {replaced_areas.cbegin(), replaced_areas.cend()}, cblock);
    }

    return new_bid;
}

//TODO: 04: In the future should be possible to add a building with predetermined areas. For example I may want to build 
//		a building with two 10x10 areas, one 12x12 and five 8x8 areas; the areas could be built without any further
//		restriction, allowing the city algorithm to insert these connected areas in the city. The problem is that the current 
//		algorithm can't foresee if a CityBlock can accomodate more than one area. Find a method to foresee if such an 
//		accomodation is possible, without actually build the areas before.
auto BuildingManager::build_building_inCity(BuildingRecipe const& recipe, CityId const cid) -> BuildingId
{
    auto replaceable_areas = std::vector<BuildingAreaCompleteId>{}; //TODO: Add the possibility to specify a list of replaceable areas.


    auto & city = m_cities.get_or_throw(cid);

    auto const area_surface = recipe.startingArea_dims().x * recipe.startingArea_dims().y;

    auto new_bid = BuildingId{0};

    // If there are no blocks, then create the first one
    if (city.empty())
    {
        auto & [cbid, cb] = m_blocks.create();
        city.add_block(cbid);

        new_bid = build_firstBuilding_inCity(cid, cbid, tgm::Utilities::v2f_to_v3i(recipe.proposed_position()), recipe, replaceable_areas, cb);
    }
    // Otherwise, expand an existent block or create a new one
    else
    {
        for(auto const cbid : city.blocks())
        {
            auto & cblock = m_blocks.get_or_throw(cbid);

            if(cblock.has_room_for(area_surface))
            {
                #if BUILDEXP_VISUALDEBUG
                    BEdeb.new_step("Trying to expand the highlighted block:", 1);
                    BEdeb.focus_onPosition(cblock.center());
                    visualDebug_highlightCityBlock(cblock, Color::Blue);
                #endif

                auto best_pos = Vector3i{};
                auto replaced_areas = std::vector<BuildingAreaCompleteId>{};
                if (is_blockExpansion_possible(cbid, cblock, recipe, replaceable_areas, best_pos, replaced_areas))
                {
                    new_bid = internal_build_building(cid, cbid, best_pos, recipe, replaced_areas, cblock);

                    break;
                }
                else
                {
                    #if BUILDEXP_VISUALDEBUG
                        BEdeb.new_step("Impossible to build the building in the current block.", 1);
                    #endif
                }
            }
        }
        
        // If all the blocks are full, then create a new block.
        if (new_bid == 0u)
        {
            auto & [cbid, cb] = m_blocks.create();
            city.add_block(cbid);
        
            #if BUILDEXP_VISUALDEBUG
                BEdeb.new_step("Creating a new block.", 1);
            #endif

            auto best_pos = Vector3i{};
            auto replaced_areas = std::vector<BuildingAreaCompleteId>{};
            if (is_cityExpansion_possible(city, cbid, recipe, replaceable_areas, best_pos, replaced_areas))
            {
                new_bid = internal_build_building(cid, cbid, best_pos, recipe, replaced_areas, cb);
            }
            else
            {
                #if BUILDEXP_VISUALDEBUG
                    BEdeb.new_step("Impossible to create a new block.", 1);
                #endif
            }
        }
    }


    return new_bid;
}

void BuildingManager::request_buildingExpansion(BuildingId const bid)
{
    #if DYNAMIC_ASSERTS
        try { m_buildings.assert_idValidity(bid); }
        catch (std::exception e) { std::ostringstream oss; oss << "Invalid BuildingId: " << e.what() << std::endl; throw std::runtime_error(oss.str()); }
    #endif
    buildingExpansion_queue.push(bid);
}

void BuildingManager::expand_buildings()
{
    for (auto n = 0; n < max_buildingExpansions; ++n)
    {
        if (buildingExpansion_queue.empty()) { break; }
        
        static auto deb_exp_counter = 0u;
        //std::cout << "Expansion #" << deb_exp_counter << std::endl;
        ++deb_exp_counter;

        auto const bid = buildingExpansion_queue.front();

        expand_building(bid, n);
            
        buildingExpansion_queue.pop();
    }
}

void BuildingManager::debug_expand_random_building()
{
    for (auto& [bid, b] : m_buildings)
    {
        if (m_unexpandable_buildings.find(bid) == m_unexpandable_buildings.end())
        {
            expand_building(bid, 0);
            break;
        }
    }
}

bool BuildingManager::expand_building(BuildingId const bid, int const queue_id)
{
    auto ret = false;

    auto const pb = m_buildings.weak_get(bid);

    // Check if the building still exists and if it's expandable
    if (pb)
    {
        if (m_unexpandable_buildings.find(bid) == m_unexpandable_buildings.end())
        {
            auto& building = *pb;

            #if BUILDEXP_VISUALDEBUG
                std::ostringstream oss;	oss << queue_id << "th building in queue.";
                BEdeb.begin_chapter(oss.str());
                BEdeb.focus_onBuilding(building);
                visualDebug_highlightBuilding(building, Color::Blue);
            #endif

                
            auto & cblock = m_blocks.get_or_throw(building.cbid());
            #if DYNAMIC_ASSERTS
                if(!cblock.contains(bid)) { throw std::runtime_error("The CityBlock doesn't contain the BuildingId of the expanding building."); }
            #endif

            auto [reuse, power, candidate_areas] = building.propose_expansion(building_expansionTemplates.at(building.expTempl_id()), m_random_generator);
    
            auto best_position = Vector3i{};
            auto selected_area = AreaType::none;
            auto replaced_areas = std::vector<BuildingAreaCompleteId>{};

            if (is_buildingExpansion_possible(cblock, bid, building, candidate_areas, best_position, selected_area, replaced_areas))
            {
                internal_expand_building(bid, selected_area, best_position, replaced_areas, cblock, building);
                
                #if DYNAMIC_ASSERTS
                    if (!is_building_connected(bid, building)) { throw std::runtime_error("The areas of the building aren't connected."); }
                #endif

                ret = true;
            }
            else
            {
                m_unexpandable_buildings.insert(bid);
                ret = false;
            }

            #if BUILDEXP_DEBUGLOG
                BElog << Logger::nltb << "---- " << n << "th expansion ends.\n\n";
            #endif
            #if BUILDEXP_VISUALDEBUG
                BEdeb.end_chapter();
            #endif
        }
        else // The building was cached as non-expandable
        {
            ret = false;
        }
    }
    else // The building doesn't exist anymore
    {
        ret = false;
    }

    return ret;
}

//TODO: 03:   E' meglio che Building sia una classe compatta che incapsuli le Aree al suo interno, oppure che sia un semplice raccoglitore?
//			 Disincapsulando le Aree da Building si va incontro a una serie di problemi che collidono col design attuale: 
//				- il principio cardine della programmazione a oggetti è l'incapuslamento. Se lo tolgo vado verso un design più procedurale. Sarà un male?
//				- per iterare le aree di un edificio sarà necessario iterare le BuildingAreaId salvate in Building eppoi 
//				  recuperarle dal container delle aree vero e proprio, rendendo il codice più verboso.
//				- sarà più complesso separare creazione dell'oggetto Building e costruzione concreta dell'edificio sui tiles.
//			 Non sembrano esserci in punti del codice in cui sarebbe concretamente più comoda la 
//			 seconda soluzione rispetto alla prima. Rivalutare il tutto più avanti.
//TODO: 03:   Lo stesso discorso di cui sopra è applicabile a CityBlock a City. Dovrebbero forse essere tutte incapsulate come una matrioska?
//			 cosa diventerebbe più complicato fare e cosa più semplice?
auto BuildingManager::debug_build_prefabBuilding(PrefabBuilding const& prefab) -> std::pair<BuildingId, Building const*>
{
    if (prefab.is_empty()) { throw std::runtime_error("Trying to add a building that doesn't have any areas."); }


    auto ret = std::pair<BuildingId, Building const*>{0, nullptr};
    auto & [new_bid, new_building] = ret;


    #if BUILDEXP_VISUALDEBUG
        BEdeb.begin_chapter("Building prefab building.");
        BEdeb.focus_onPrefabBuilding(prefab);
    #endif

    auto const bldg_center = prefab.compute_volume().center();

    auto const cid = get_nearestCity(tgm::Utilities::v3i_to_v2f(bldg_center));
    auto & city = m_cities.get_or_throw(cid);

    auto & [cbid, cblock] = m_blocks.create(); // Create a block to contain the building
    city.add_block(cbid);

    // Check that all the areas are buildable
    auto is_building_buildable = true;
    for (auto const& area : prefab.areas())
    {
        auto [is_buildable, replaced_areas] = is_area_buildable(cbid, area.volume.begin(), area.volume.base_dims(), {});

        // Just one unbuildable area is enough to block the construction
        if (!is_buildable)
        {
            is_building_buildable = false;			
            break;
        }
    }

    // If all the areas are buildable, add the building to the map
    if (is_building_buildable)
    {
        #if PLAYERMOVEMENT_VISUALDEBUG
            PMdeb.begin_chapter("Building prefab building."); // Starting a new chapter is necessary to add impassable tiles to PlayerMovementVisualDebug.
        #endif


        auto & new_el = m_buildings.create(buildingExpansionTemplates_nameToId.at(prefab.expansion_template()), cid, cbid);
        new_bid = new_el.id();
        auto & new_bldg = new_el.value;

        cblock.add_building(new_bid);

        
        #if BUILDEXP_VISUALDEBUG
            BEdeb.add_building(new_bid, new_bldg);
        #endif

        
        for (auto const& prefab_area : prefab.areas())
        {
            auto const& [aid, area] = new_bldg.create_area(prefab_area.type, prefab_area.volume.begin(), prefab_area.volume.base_dims(), 
                                                           building_expansionTemplates.at(new_bldg.expTempl_id()));

            build_buildingArea(cbid, new_bid, aid, area, new_bldg);

            cblock.increase_surface(area.volume());


            #if BUILDEXP_VISUALDEBUG
                BEdeb.update_building(new_bid, new_bldg);
            #endif
        }

        #if DYNAMIC_ASSERTS
            if (!is_building_connected(new_bid, new_bldg)) { throw std::runtime_error("The areas of the building aren't connected."); }
        #endif


        // Validate and build doors.
        for (auto const pos : prefab.doors())
        {
            auto const t = m_tiles.get(pos);

            if (!t || !t->is_border())
            {
                throw std::runtime_error("Invalid tile for a PrefabBuilding door.");
            }
            else
            {
                for (auto const info : t->get_borderInfos())
                {
                    if (!info.is_empty() && info.bid() != new_bid) { throw std::runtime_error("Invalid tile for a PrefabBuilding door."); }
                }
            }

            try_build_door(pos);
        }


        new_building = &new_bldg;

                
        #if BUILDEXP_VISUALDEBUG
            BEdeb.new_step("Prefab building built.", 1);
        #endif

        #if PLAYERMOVEMENT_VISUALDEBUG
            PMdeb.end_chapter();
        #endif
    }
    else
    {
        // Destroy the block, it was created only to host the building
        city.remove_block(cbid);		//since cbid is a reference, if I did m_blocks.destroy(cbid) first then its value would change
        m_blocks.destroy(cbid);

        // If the city was created just to host the building, then destroy it
        if (city.empty())
        {
            m_cities.destroy(cid);
        }

        #if BUILDEXP_VISUALDEBUG
            BEdeb.new_step("Impossible to add the prefab building. One or more areas aren't buildable.", 1);
        #endif
    }
            

    #if BUILDEXP_VISUALDEBUG
        BEdeb.end_chapter();
    #endif

    return ret;
}

void BuildingManager::debug_createDestroy_door(Vector3i const pos)
{
    #if BUILDEXP_VISUALDEBUG
        bool debug_shoudCloseChapter = false;
        if (!BEdeb.has_chapter_begun()) 
        { 
            BEdeb.begin_chapter("Create/Destroy door"); 
            debug_shoudCloseChapter = true;
        }
    #endif

    auto const& t = m_tiles.get_existent(pos);

    if (t.is_border())
    {
        if (t.is_door())
        {
            try_unbuild_door(pos.x, pos.y, pos.z);
        }
        else
        {
            try_build_door(pos);
        }
    }

    
    #if BUILDEXP_VISUALDEBUG
        if (debug_shoudCloseChapter) { BEdeb.end_chapter(); }
    #endif
}


bool BuildingManager::is_buildingExpansion_possible(CityBlock const& cblock,
                                                    BuildingId const bid, Building const& building,
                                                    std::vector<AreaType> const& candidate_areas,
                                                    Vector3i & best_position,
                                                    AreaType & selected_area,
                                                    std::vector<BuildingAreaCompleteId> & replaced_areas)
{
    auto alreadyChecked_dims = std::unordered_set<Vector2i>{};

    for (auto const& carea : candidate_areas)
    {
        #if BUILDEXP_VISUALDEBUG
            std::ostringstream oss; oss << "Trying to expand the building with a '" << carea << "' area";
            BEdeb.new_step(oss.str(), 1);
        #endif

        auto const area_dims = area_templates.at(carea).min_dims();

        //If two or more candidate areas have the same dims, take into consideration only the first.
        if (alreadyChecked_dims.find(area_dims) == alreadyChecked_dims.end())
        {
            alreadyChecked_dims.insert(area_dims);
        }
        else
        {
            break;
        }

        if(cblock.has_room_for(area_dims.x * area_dims.y))
        {
            // The candidate area could replace some other areas. Detect those areas in this building.
            auto const replaceable_areas = building.find_replaceableAreas(bid, carea, building_expansionTemplates.at(building.expTempl_id()));

            #if BUILDEXP_VISUALDEBUG
                visualDebug_replaceableAreasStep(replaceable_areas);
            #endif
        

            //--- The candidate area could be built only in specific positions (for building integrity's sake). Collect all those positions.
            auto suitable_positions = std::unordered_set<Vector3i>{};
            compute_suitablePositions_aroundBuilding(bid, building, area_dims, true, false, replaceable_areas, suitable_positions);
    
            #if BUILDEXP_VISUALDEBUG
                BEdeb.new_step("Suitable positions in the building", 2);
                BEdeb.highlight_tiles(suitable_positions.cbegin(), suitable_positions.cend(), Color::Red);
            #endif


            auto const buildable_positions = compute_buildablePositions(building.cbid(), bid, &building, suitable_positions, area_dims, replaceable_areas);

            if (!buildable_positions.empty() && compute_bestPosition(area_dims, 0, buildable_positions, best_position, replaced_areas))
            {
                selected_area = carea;
                return true;
            }
        }
    }

    #if BUILDEXP_VISUALDEBUG
        BEdeb.new_step("No place where a candidate area can be built.", 1);
    #endif

    return false;
}

bool BuildingManager::is_blockExpansion_possible(CityBlockId const cbid,
                                                 CityBlock const& block,
                                                 BuildingRecipe const& recipe,
                                                 std::vector<BuildingAreaCompleteId> const& replaceable_areas,
                                                 Vector3i & best_position,
                                                 std::vector<BuildingAreaCompleteId> & replaced_areas) const
{
    #if BUILDEXP_VISUALDEBUG
        visualDebug_replaceableAreasStep(replaceable_areas);
    #endif


    // The new area could be built only at a certain distance from the other areas of the block. Collect all those positions.
    auto const suitable_poss = compute_suitablePositions_inBlock(block, recipe.startingArea_dims(), replaceable_areas);
    
    auto const buildable_poss = compute_buildablePositions(cbid, suitable_poss, recipe.startingArea_dims(), replaceable_areas);

    if (!buildable_poss.empty() && compute_bestPosition(recipe.startingArea_dims(), 0, buildable_poss, best_position, replaced_areas))
    {
        return true;
    }
    else
    {
        #if BUILDEXP_VISUALDEBUG
            BEdeb.new_step("The area can't be built in this block", 1);
        #endif

        return false;
    }
}

auto BuildingManager::compute_suitablePositions_inBlock(CityBlock const& block, 
                                                       Vector2i const area_dims,
                                                       std::vector<BuildingAreaCompleteId> const& replaceable_areas) const 
    -> std::unordered_set<Vector3i>
{
    auto suitable_positions = std::unordered_set<Vector3i>{};

    for (auto const bid : block.buildings())
    {
        auto const& bldg = m_buildings.get_or_throw(bid);

        compute_suitablePositions_aroundBuilding(bid, bldg, area_dims, false, false, replaceable_areas, suitable_positions);
    }

    #if BUILDEXP_VISUALDEBUG
        BEdeb.new_step("Suitable positions in the block", 2);
        BEdeb.highlight_tiles(suitable_positions.cbegin(), suitable_positions.cend(), Color::Red);
    #endif

    return suitable_positions;
}

bool BuildingManager::is_cityExpansion_possible(City const& city,
                                                CityBlockId const cbid,
                                                BuildingRecipe const& recipe,
                                                std::vector<BuildingAreaCompleteId> const& replaceable_areas,
                                                Vector3i & best_position,
                                                std::vector<BuildingAreaCompleteId> & replaced_areas) const
{
    #if BUILDEXP_VISUALDEBUG
        visualDebug_replaceableAreasStep(replaceable_areas);
    #endif

    auto const ordered_blocks = order_cityBlocks(city);

    // Try to build the new building sarting from the closest block to center of the city
    for (auto const [distance, block] : ordered_blocks)
    {
        #if BUILDEXP_VISUALDEBUG
            BEdeb.new_step("Looking around the highlighted block", 1);
            BEdeb.focus_onPosition(block->center());
            visualDebug_highlightCityBlock(*block, Color::Blue);
        #endif

        // Gather all the suitable positions around each area of the block
        auto suitable_positions = std::unordered_set<Vector3i>{};
    
        for (auto const bid : block->buildings())
        {
            auto const& bld = m_buildings.get_or_throw(bid);

            // If the block is empty, then the new area have to be built at a road of distance from an area of another block.
            compute_suitablePositions_aroundBuilding(bid, bld, recipe.startingArea_dims(), false, true, replaceable_areas, suitable_positions);
        }
    
        #if BUILDEXP_VISUALDEBUG
            BEdeb.new_step("Suitable positions in the block", 2);
            BEdeb.highlight_tiles(suitable_positions.cbegin(), suitable_positions.cend(), Color::Red);
        #endif

            
        auto const buildable_poss = compute_buildablePositions(cbid, suitable_positions, recipe.startingArea_dims(), replaceable_areas);

        if (!buildable_poss.empty() && compute_bestPosition(recipe.startingArea_dims(), sim_settings.map.road_dim + 1, buildable_poss, best_position, replaced_areas))
        {
            return true;
        }
        else
        {
            #if BUILDEXP_VISUALDEBUG
                BEdeb.new_step("The area can't be built in this block", 1);
            #endif
        }
    }
    
    #if BUILDEXP_VISUALDEBUG
        BEdeb.new_step("Impossible to expand the city. No buildable position found", 1);
    #endif

    return false;
}

auto BuildingManager::order_cityBlocks(City const& city) const -> std::map<float, CityBlock const*>
{
    auto ordered_blocks = std::map<float, CityBlock const*>{};
    
    auto const city_center = compute_cityCenter(city);

    for (auto const cbid : city.blocks())
    {
        auto const& block = m_blocks.get_or_throw(cbid);
        ordered_blocks.emplace(distance(city_center, block.center()), &block);
    }
    
    #if BUILDEXP_VISUALDEBUG
        BEdeb.new_step("The blocks have been ordered from the nearest to the farther", 2);
        visualDebug_highlightOrderedCityBlocks(city_center, ordered_blocks);
    #endif

    return ordered_blocks;
}

void BuildingManager::compute_suitablePositions_aroundBuilding(BuildingId const bid, Building const& building,
                                                               Vector2i const area_dims,
                                                               bool const is_expansion,
                                                               bool const is_newBlock,
                                                               std::vector<BuildingAreaCompleteId> const& replaceable_areas,
                                                               std::unordered_set<Vector3i> & suitable_positions)
{
    auto const hor_expFactor = is_expansion ? 2 : 0;
    auto const vert_expFactor = is_expansion ? 1 : 0;

    auto const road_dist = is_newBlock ? sim_settings.map.road_dim + 1 : 0;

    for (auto const& [aid, area] : building.areas_by_ref())
    {
        auto const acid = BuildingAreaCompleteId{ bid, aid };

        //TODO: PERFORMANCE: Since areas and the ids in replaceable_area are both ordered (SURE??) from the smallest to the greatest,
        //					 it's not necessary to start the std::find() every time from the smallest, but one can start from the last found.
        //					 Note: Be sure that replaceable_areas is still ordered when doing this optimization.
        // If "area" doesn't belong to replaceable areas, then add all the around positions
        if (std::find(replaceable_areas.cbegin(), replaceable_areas.cend(), acid) == replaceable_areas.cend())
        {
            #if BUILDEXP_VISUALDEBUG
                BEdeb.new_step("Suitable positions for this area.", 4);
            #endif

            auto const& vol = area.volume();

            auto const horizontal_beg = vol.left - area_dims.y + 1 + hor_expFactor - road_dist;
            auto const horizontal_end = vol.right() + 1			   - hor_expFactor + road_dist;

            auto const northside_x = vol.behind - area_dims.x + 1 - road_dist; //+1 for the common border
            auto const southside_x = vol.front()				  + road_dist;

            // Compute northside and southside suitable positions
            for(auto y = horizontal_beg; y < horizontal_end; ++y)
            {
                auto const north_pos = Vector3i{ northside_x, y, vol.down };
                suitable_positions.insert(north_pos);

                auto const south_pos = Vector3i{ southside_x, y, vol.down };
                suitable_positions.insert(south_pos);

                #if BUILDEXP_VISUALDEBUG
                    BEdeb.highlight_tile(north_pos, Color::Red);
                    BEdeb.highlight_tile(south_pos, Color::Red);
                #endif
            }


            auto const vertical_beg = vol.behind - area_dims.x + 2 + vert_expFactor - road_dist;
            auto const vertical_end = vol.front()				   - vert_expFactor + road_dist;

            auto const westside_y = vol.left - area_dims.y + 1 - road_dist; //+1 for the common border
            auto const eastside_y = vol.right()				   + road_dist;
                
            // Compute westside and eastside suitable positions
            for (auto x = vertical_beg; x < vertical_end; ++x)
            {
                auto const west_pos = Vector3i{ x, westside_y, vol.down };
                suitable_positions.insert(west_pos);

                auto const east_pos = Vector3i{ x, eastside_y, vol.down };
                suitable_positions.insert(east_pos);

                #if BUILDEXP_VISUALDEBUG
                    BEdeb.highlight_tile(west_pos, Color::Red);
                    BEdeb.highlight_tile(east_pos, Color::Red);
                #endif
            }
        }
        // Otherwise, insert only beginning position
        else
        {
            auto const& vol = area.volume();
            suitable_positions.insert({ vol.behind, vol.left, vol.down });
        }
    }
}

auto BuildingManager::compute_buildablePositions(CityBlockId const cbid,
                                                 std::unordered_set<Vector3i> const& suitable_positions,
                                                 Vector2i const area_dims,
                                                 std::vector<BuildingAreaCompleteId> const& replaceable_areas) const -> std::vector<BuildablePosition>
{
    return compute_buildablePositions(cbid, 0, nullptr, suitable_positions, area_dims, replaceable_areas);
}

auto BuildingManager::compute_buildablePositions(CityBlockId const cbid,
                                                 BuildingId const bid, Building const*const bld,
                                                 std::unordered_set<Vector3i> const& suitable_positions,
                                                 Vector2i const area_dims,
                                                 std::vector<BuildingAreaCompleteId> const& replaceable_areas) const -> std::vector<BuildablePosition>
{
    auto buildable_positions = std::vector<BuildablePosition>{};

    // For each suitable tile check if it denotes the begin of a free area.
    for (auto const pos : suitable_positions)
    {
        auto [is_buildable, areas_to_replace] = is_area_buildable(cbid, bid, bld, pos, area_dims, replaceable_areas);
        if (is_buildable)
        {
            buildable_positions.emplace_back(pos, areas_to_replace);
        }
    }

    #if BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS
        visualDebug_buildablePositionsStep(area_dims, buildable_positions);
    #endif

    return buildable_positions;
}

auto BuildingManager::is_area_buildable(CityBlockId const cbid,
                                        BuildingId const bid, Building const*const building,
                                        Vector3i const position, Vector2i const dims, 
                                        std::vector<BuildingAreaCompleteId> const& replaceable_areas) const
    -> std::pair<bool, std::unordered_set<BuildingAreaCompleteId>>
{
    if (dims.x < 3 || dims.y < 3) {	throw std::runtime_error("Area dimensions are too small."); }


    auto ret = std::pair<bool, std::unordered_set<BuildingAreaCompleteId>>{}; //NRVO
    auto & [is_buildable, replaced_areas] = ret;

    #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
        BEdeb.new_step("Examining if the area is buildable", 4);
        BEdeb.highlight_tilesRect({ position.x, position.y, dims.x, dims.y }, position.z, Color{ 125,125,125,50 });
        auto constexpr notBuildable_depth = 4;
    #endif
    
    auto const e_pos = position - Vector3i{sim_settings.map.road_dim + 1, sim_settings.map.road_dim + 1, 0}; //'+1' because the rest of the algorithm supposes that the tile beyond the street exists
    auto const e_dims = dims + Vector2i{sim_settings.map.road_dim + 1, sim_settings.map.road_dim + 1} * 2;

    //--- Check if the enlarged area spans outside the map
    if (!m_tiles.contains(e_pos.x, e_pos.y, e_pos.z, e_dims.x, e_dims.y, 1))
    {
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Not buildable. The area plus its road belt strips would span outisde the map", notBuildable_depth);
        #endif
        is_buildable = false;
        return ret;
    }


    //--- Check if the inner area is free/replaceable
    for (auto y = position.y + 1; y < position.y + dims.y - 1; ++y)
    {
        for (auto x = position.x + 1; x < position.x + dims.x - 1; ++x)
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("Examining if the inner area tile is buildable.", 5);
                BEdeb.highlight_tile({x, y, position.z}, Color(125, 125, 125, 50));
            #endif

            if (!is_tile_buildableWithInnerArea_inBlock(x, y, position.z, replaceable_areas, replaced_areas))
            {
                #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                    BEdeb.new_step("Not buildable. An inner area tile can't be built", notBuildable_depth);
                #endif
                is_buildable = false;
                return ret;
            }
        }
    }


    //--- Check that the borders are free/replaceable or are already borders.

    // Horizontal borders
    for (auto y = position.y; y < position.y + dims.y; ++y)
    {
        auto const x_top = position.x;
        auto const x_bottom = position.x + dims.x - 1;
        
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Examining if the two horizontal border tiles are buildable.", 5);
            BEdeb.highlight_tile({ x_top, y, position.z }, Color(125, 125, 125, 50));
            BEdeb.highlight_tile({ x_bottom, y, position.z }, Color(125, 125, 125, 50));
        #endif

        if (   !is_tile_buildableWithBorder_inBlock(x_top, y, position.z, replaceable_areas, replaced_areas)
            || !is_tile_buildableWithBorder_inBlock(x_bottom, y, position.z, replaceable_areas, replaced_areas))
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("Not buildable. An horizontal border can't be built", notBuildable_depth);
            #endif
            is_buildable = false;
            return ret;
        }
    }

    // Vertical borders (without the corners that have been already checked with horizontal borders)
    for (auto x = position.x + 1; x < position.x + dims.x - 1; ++x)
    {
        auto const left_y  = position.y;
        auto const right_y = position.y + dims.y - 1;

        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Examining if the two vertical border tiles are buildable.", 5);
            BEdeb.highlight_tile({x, left_y, position.z}, Color(125, 125, 125, 50));
            BEdeb.highlight_tile({ x, right_y, position.z }, Color(125, 125, 125, 50));
        #endif

        if(   !is_tile_buildableWithBorder_inBlock(x, left_y , position.z, replaceable_areas, replaced_areas)
           || !is_tile_buildableWithBorder_inBlock(x, right_y, position.z, replaceable_areas, replaced_areas))
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("Not buildable. A vertical border tile can't be built.", notBuildable_depth);
            #endif
            is_buildable = false;
            return ret;
        }
    }


    //--- Check that the enlarged area doesn't hit neighboring blocks
    
    // Iterate through the width of the road belt.
    for(auto i = 1; i <= sim_settings.map.road_dim; ++i)
    {
        auto const rb_pos = position - Vector3i{i, i, 0};
        auto const rb_dims = dims + Vector2i{i * 2, i * 2};

        // Horizontal road belt strip
        for (auto y = rb_pos.y; y < rb_pos.y + rb_dims.y; ++y)
        {
            auto const x_top    = rb_pos.x;
            auto const x_bottom = rb_pos.x + rb_dims.x - 1;
        
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("Examining if two horizontal road belt tiles hit a neighbor block", 5);
                BEdeb.highlight_tile({ x_top,    y, rb_pos.z }, Color(125, 125, 125, 50));
                BEdeb.highlight_tile({ x_bottom, y, rb_pos.z }, Color(125, 125, 125, 50));
            #endif

            if (   !is_tile_blockFree(cbid, x_top,    y, rb_pos.z)
                || !is_tile_blockFree(cbid, x_bottom, y, rb_pos.z))
            {
                #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                    BEdeb.new_step("Not buildable. An horizontal road belt strip hits a neighbor block", notBuildable_depth);
                #endif
                is_buildable = false;
                return ret;
            }
        }

        // Vertical road belt strip (without the corners that have been already checked with horizontal borders)
        for (auto x = rb_pos.x + 1; x < rb_pos.x + rb_dims.x - 1; ++x)
        {
            auto const left_y  = rb_pos.y;
            auto const right_y = rb_pos.y + rb_dims.y - 1;

            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("Examining if two vertical road belt tiles hit a neighbor block", 5);
                BEdeb.highlight_tile({ x, left_y,  rb_pos.z }, Color(125, 125, 125, 50));
                BEdeb.highlight_tile({ x, right_y, rb_pos.z }, Color(125, 125, 125, 50));
            #endif

            if(   !is_tile_blockFree(cbid, x, left_y,  rb_pos.z)
               || !is_tile_blockFree(cbid, x, right_y, rb_pos.z))
            {
                #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                    BEdeb.new_step("Not buildable. A vertical road belt strip hits a neighbor block", notBuildable_depth);
                #endif
                is_buildable = false;
                return ret;
            }
        }
    }



    //--- Check that the areas of the building remain connected after replacing the old area with the new one
    auto const vol = IntParallelepiped{ position.x, position.y, position.z, dims.x, dims.y, 1 };
    if (building && !replaced_areas.empty())
    {
        if (!is_building_connected(bid, *building, vol, replaced_areas))
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("Not buildable. The building wouldn't be connected anymore.", notBuildable_depth);
            #endif
            is_buildable = false;
            return ret;
        }
    }


    //--- Gather all the external doors of the block that this area would occlude

    auto removed_doors = std::unordered_set<Vector3i>{};

    // Gather external doors in replaced areas
    for (auto const acid : replaced_areas)
    {
        auto const b = m_buildings.get_or_throw(acid.bid);
        auto const a = b.areas_by_ref().get_or_throw(acid.aid);
        
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Examining if the replaced area contains external doors.", 4);
            BEdeb.highlight_tilesRect(a.volume().get_base(), position.z, Color::Mint);
        #endif

        for (auto const d : b.external_doors())
        {
            if (a.volume().contains(d))
            {
                removed_doors.insert(d);

                #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                    BEdeb.new_step("External door found.", 5);
                    BEdeb.highlight_tile(d, Color::Red);
                #endif
            }
        }
    }

    // Gather external doors on the shared borders and around the borders of this area.
    gather_adjacent_externalDoors(position, dims, removed_doors);
    
    // Gather external doors that are occluded by this area
    gather_occluded_externalDoors(vol, replaced_areas, removed_doors);

    
    //--- Check if there's any external door that's essential

    #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
        BEdeb.new_step("Doors that would be removed", 4);
        BEdeb.highlight_tiles(removed_doors.cbegin(), removed_doors.cend(), Color::Red);
    #endif

    
    // Compute how many external doors must be removed for each building.
    auto removedDoors_per_building = std::unordered_map<BuildingId, std::size_t>{};		// Number of doors that would be removed for each building
    for (auto const pos : removed_doors)
    {
        auto const& t = m_tiles.get_existent(pos);

        auto const door_bid = t.get_borderInfos()[0].bid();

        ++removedDoors_per_building[door_bid];
    }
    

    for (auto const [door_bid, count] : removedDoors_per_building)
    {
        auto const& b = m_buildings.get_or_throw(door_bid);

        // If the building would have no other external door
        if (b.external_doors().size() < 1 + count)
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("Not buildable. The area would remove the last door of a building.", notBuildable_depth);
            #endif
            is_buildable = false;
            return ret;
        }
    }
    
    
    is_buildable = true;
    return ret;
}


bool BuildingManager::is_tile_buildableWithInnerArea_inBlock(int const x, int const y, int const z,
                                                             std::vector<BuildingAreaCompleteId> const& replaceable_areas,
                                                             std::unordered_set<BuildingAreaCompleteId> & replaced_areas) const
{
    auto const t = m_tiles.get(x, y, z);
    if (!t) { throw std::runtime_error("Trying to check if an unexistent tile is built."); }

    if (t->is_built())
    {
        // If the tile hosts an inner area, then just check that the area is replaceable
        if (t->is_innerArea())
        {
            auto const info = t->get_innerAreaInfo();
            
            if (std::find(replaceable_areas.cbegin(), replaceable_areas.cend(), info.acid()) != replaceable_areas.cend())
            {
                replaced_areas.insert(info.acid());
                return true;
            }
            else
            {
                return false;
            }
        }
        // If the tile hosts a border, then all the areas that share the border must be replaceable
        else if (t->is_border())
        {
            for (auto const info : t->get_borderInfos())
            {
                if (!info.is_empty())
                {
                    // If it doesn't belong to the replaceable areas
                    if (std::find(replaceable_areas.cbegin(), replaceable_areas.cend(), info.acid()) == replaceable_areas.cend())
                    {
                        return false;
                    }
                }
            }

            return true;
        }
        else
        {
            throw std::runtime_error("A built tile must be either an inner area or a border.");
        }
    }
    else
    {
        return true;
    }
}

bool BuildingManager::is_tile_buildableWithBorder_inBlock(int const x, int const y, int const z, 
                                                          std::vector<BuildingAreaCompleteId> const& replaceable_areas,
                                                          std::unordered_set<BuildingAreaCompleteId> & replaced_areas) const
{
    auto const t = m_tiles.get(x, y, z);
    if (!t) { throw std::runtime_error("Trying to check if an unexistent tile is built."); }

    if(t->is_built())
    {
        // If the tile hosts an inner area, then just check that the area is replaceable
        if (t->is_innerArea())
        {
            auto const info = t->get_innerAreaInfo();
            
            if (std::find(replaceable_areas.cbegin(), replaceable_areas.cend(), info.acid()) != replaceable_areas.cend())
            {
                replaced_areas.insert(info.acid());
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (t->is_border())
        {
            // If the tile hosts the maximum number of areas, then at least one of the them has to be replaceable
            if (t->is_fullOfBorders()) 
            {
                //TODO: PERFORMANCE: This is a rare event, but sometimes happens. There are two speculation here:
                //					 1) It happens ONLY when at least one area is replaceable (otherwise the check on the inner area tiles wouldn't have passed)
                //					 2) When it happens the replaceable area has already been added to the set of the replaced areas (because the inner area 
                //					    is checked before the borders)
                //					 Check this connjecture and if it holds, then make this branch directly return true.
                #if DYNAMIC_ASSERTS
                    {
                        // Assert to prove the aforementioned conjecture

                        auto tile_replaceableAreas = std::vector<BuildingAreaCompleteId>{};
                        for (auto const info : t->get_borderInfos())
                        {
                            if (!info.is_empty())
                            {
                                if (std::find(replaceable_areas.cbegin(), replaceable_areas.cend(), info.acid()) != replaceable_areas.cend())
                                {
                                    tile_replaceableAreas.push_back(info.acid());
                                }
                            }
                        }

                        if (tile_replaceableAreas.empty())
                        {
                            throw std::runtime_error("Unexpected situation. The tile hosts the borders of 4 areas and none of them is replaceable. The point 1) of the above conjecture is sadly false...");
                        }

                        auto already_added = false;
                        for (auto const acid : tile_replaceableAreas)
                        {
                            if (replaced_areas.find(acid) != replaced_areas.cend()) 
                            { 
                                already_added = true; 
                                break;
                            }
                        }
                        if (!already_added)
                        {
                            throw std::runtime_error("Unexpected situation. The tile hosts the borders of 4 areas and none of them has been already added to the replaced areas. The point 2) of the above conjecture is sadly false...");
                        }
                    }
                #endif

                return true;
            }
            // If the tile hosts a border but it's not full, then it can also host the border of a new area
            else
            {
                return true;
            }
        }
        else
        {
            throw std::runtime_error("A built tile must be either an inner area or a border.");
        }
    }
    else
    {
        return true;
    }
}

bool BuildingManager::is_tile_blockFree(CityBlockId const cbid, int const x, int const y, int const z) const
{
    auto const t = m_tiles.get(x, y, z);
    if (!t) { throw std::runtime_error("Trying to check if an unexistent tile is block free."); }

    if (t->is_built())
    {
        return t->block() == cbid;
    }
    else
    {
        return true;
    }
}

bool BuildingManager::is_building_connected(BuildingId const bid, Building const& bld, 
                                            IntParallelepiped const& ghost_vol, 
                                            std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const
{
    if (bld.is_empty() || is_building_subsetOf(bid, bld, replaced_areas))
    {
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("The building is either empty or a subset of the replaced areas.", 5);
        #endif
            
        return true;
    }
    else
    {
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Replaced areas in building.", 5);
            for (auto const acid : replaced_areas)
            {
                auto vol =  bld.areas_by_ref().get_or_throw(acid.aid).volume();
                BEdeb.highlight_tilesRect(vol.get_base(), vol.down, Color::Blue);
            }
        #endif

        auto const nodes_number = tgm::Utilities::uint32_to_int32(bld.areas_by_ref().count());

        auto examined_nodes = std::set<BuildingAreaId>{};

        // Take the first area that is not a replaceable area (its existence is guaranteed by the fact that the building is not a subeset of replaced_areas).
        auto it = bld.areas_by_ref().cbegin();
        while (   replaced_areas.find({ bid, it->id() }) != replaced_areas.cend()
               && it != bld.areas_by_ref().cend()									)
        {
            ++it;
        }
        #if DYNAMIC_ASSERTS
            if (it == bld.areas_by_ref().cend()) { throw std::runtime_error("Unexpected situation. The building doesn't contain an area that isn't replaced, although it's not a subset of replaced_areas."); }
        #endif
            
        auto const& [current_aid, current_area] = *it;
        examined_nodes.insert(current_aid);

        explore_areaNeighbors(bid, bld, ghost_vol, replaced_areas, current_area.volume(), examined_nodes);

        return examined_nodes.size() == nodes_number;
    }
}

bool BuildingManager::is_building_subsetOf(BuildingId const bid, Building const& bld, std::unordered_set<BuildingAreaCompleteId> const& replaced_areas)
{
    auto is_subsetOf = true;

    for (auto const& [aid, area] : bld.areas_by_ref())
    {
        if (replaced_areas.find({ bid, aid }) == replaced_areas.cend())
        {
            is_subsetOf = false;
            break;
        }
    }

    return is_subsetOf;
}

void BuildingManager::explore_areaNeighbors(BuildingId const bid, Building const& bld,
                                            IntParallelepiped const& ghost_vol, 
                                            std::unordered_set<BuildingAreaCompleteId> const& replaced_areas,
                                            IntParallelepiped const& current_vol, std::set<BuildingAreaId> & examined_nodes) const
{
    #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
        BEdeb.new_step("Examined areas and current area.", 5);
        for (auto const aid : examined_nodes)
        {
            auto vol = IntParallelepiped{};
            if (aid == 0) {	vol = ghost_vol; }
            else		  { vol = bld.areas_by_ref().get_or_throw(aid).volume(); }
            BEdeb.highlight_tilesRect(vol.get_base(), vol.down, Color{ 0, 0, 255, 123 });
        }
        BEdeb.highlight_tilesRect(current_vol.get_base(), current_vol.down, Color{ 255, 0, 0, 123 });
    #endif

    for (auto const& [aid, area] : bld.areas_by_ref())
    {
        // Skip replaced areas
        if (replaced_areas.find({ bid, aid }) != replaced_areas.cend())
        {
            continue;
        }

        if (examined_nodes.find(aid) == examined_nodes.cend())
        {
            if (are_areas_connected(current_vol, area.volume()))
            {
                examined_nodes.insert(aid);

                explore_areaNeighbors(bid, bld, ghost_vol, replaced_areas, area.volume(), examined_nodes);
            }
        }
    }
    
    // Check also the ghost_vol
    auto const ghost_aid = BuildingAreaId{ 0 };

    if (examined_nodes.find(ghost_aid) == examined_nodes.cend())
    {
        if (are_areas_connected(current_vol, ghost_vol))
        {
            examined_nodes.insert(ghost_aid);

            explore_areaNeighbors(bid, bld, ghost_vol, replaced_areas, ghost_vol, examined_nodes);
        }
    }
}

bool BuildingManager::are_areas_connected(IntParallelepiped const lhs, IntParallelepiped const rhs)
{
    #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
        BEdeb.new_step("Examining if this two areas are connected.", 5);
        BEdeb.highlight_tilesRect(lhs.get_base(), lhs.down, Color::Red);
        BEdeb.highlight_tilesRect(rhs.get_base(), rhs.down, Color::Blue);
    #endif

    if (lhs.down == rhs.down)
    {
        auto const intersection = lhs.intersect(rhs);

        if (    (intersection.length == 1 && intersection.width >= 3)
             || (intersection.length >= 3 && intersection.width == 1)  )
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("Connected.", 5);
            #endif
            return true;
        }
        else if (intersection.length > 1 && intersection.width > 1)
        {
            throw std::runtime_error("Unexpected case.");
        }
        else
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("Not connected.", 5);
            #endif
            return false;
        }
    }
    else
    {
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Not connected.", 5);
        #endif
        return false;
    }
}

void BuildingManager::gather_adjacent_externalDoors(Vector3i const position, Vector2i const dims,
                                                    std::unordered_set<Vector3i> & removed_doors) const
{
    // Horizontal borders
    for (auto y = position.y; y < position.y + dims.y; ++y)
    {
        auto const top_pos    = Vector3i{ position.x			 , y, position.z };
        auto const bottom_pos = Vector3i{ position.x + dims.x - 1, y, position.z };

        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Examining if the two horizontal border tiles contain external doors.", 5); 
            BEdeb.highlight_tile(top_pos,    Color(125, 125, 125, 50));
            BEdeb.highlight_tile(bottom_pos, Color(125, 125, 125, 50));
        #endif

        gather_externalDoor(top_pos,    removed_doors);
        gather_externalDoor(bottom_pos, removed_doors);
    }

    // Vertical borders (without the corners that have been already checked with horizontal borders)
    for (auto x = position.x + 1; x < position.x + dims.x - 1; ++x)
    {
        auto const left_pos  = Vector3i{ x, position.y,			     position.z };
        auto const right_pos = Vector3i{ x, position.y + dims.y - 1, position.z };

        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Examining if the two vertical border tiles contain external doors.", 5);
            BEdeb.highlight_tile(left_pos,  Color(125, 125, 125, 50));
            BEdeb.highlight_tile(right_pos, Color(125, 125, 125, 50));
        #endif
            
        gather_externalDoor(left_pos,  removed_doors);
        gather_externalDoor(right_pos, removed_doors);
    }

    // Horizontal surrounding borders
    for (auto y = position.y - 1; y < position.y + dims.y + 1; ++y)
    {
        auto const top_pos    = Vector3i{ position.x - 1     , y, position.z };
        auto const bottom_pos = Vector3i{ position.x + dims.x, y, position.z };

        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Examining if the two horizontal surrounding tiles contain external doors.", 5);
            BEdeb.highlight_tile(top_pos,    Color(125, 125, 125, 50));
            BEdeb.highlight_tile(bottom_pos, Color(125, 125, 125, 50));
        #endif

        gather_surroundingExternalDoor(top_pos,    false, removed_doors);
        gather_surroundingExternalDoor(bottom_pos, false, removed_doors);
    }

    // Vertical surrounding borders (without the corners that have been already checked with horizontal surrounding borders)
    for (auto x = position.x; x < position.x + dims.x; ++x)
    {
        auto const left_pos  = Vector3i{ x, position.y - 1,		 position.z };
        auto const right_pos = Vector3i{ x, position.y + dims.y, position.z };

        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Examining if the two vertical border tiles contain external doors.", 5);
            BEdeb.highlight_tile(left_pos,  Color(125, 125, 125, 50));
            BEdeb.highlight_tile(right_pos, Color(125, 125, 125, 50));
        #endif
            
        gather_surroundingExternalDoor(left_pos,  true, removed_doors);
        gather_surroundingExternalDoor(right_pos, true, removed_doors);
    }
}

void BuildingManager::gather_externalDoor(Vector3i const pos,
                                          std::unordered_set<Vector3i>& removed_doors) const
{
    auto const& t  = m_tiles.get_existent(pos);

    if (t.is_externalDoor())
    {
        removed_doors.insert(pos);

        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("External door found.", 5);
            BEdeb.highlight_tile(pos, Color::Red);
        #endif
    }
}

void BuildingManager::gather_surroundingExternalDoor(Vector3i const pos, bool const vertical,
                                                     std::unordered_set<Vector3i>& removed_doors) const
{
    auto const& t  = m_tiles.get_existent(pos);

    if (t.is_externalDoor())
    {
        // The door have to be removed only if it faces the border of the area
        if (m_door_manager.is_vertical(t.furniture_id()) == vertical)
        {
            removed_doors.insert(pos);

            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("External door found.", 5);
                BEdeb.highlight_tile(pos, Color::Red);
            #endif
        }
    }
}

void BuildingManager::gather_occluded_externalDoors(IntParallelepiped const& vol,
                                                    std::unordered_set<BuildingAreaCompleteId> const& replaced_areas,
                                                    std::unordered_set<Vector3i> & removed_doors) const
{
    auto const outlines = compute_areaOutlines(vol, replaced_areas);

    for (auto const& outline : outlines)
    {
        // If this is an internal outline, then should be checked that its external doors are essential
        if (outline.is_internal())
        {
            removed_doors.insert(outline.external_doors.cbegin(), outline.external_doors.cend());
        }
    }
}

auto BuildingManager::compute_areaOutlines(IntParallelepiped const& vol, std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const
    -> std::vector<BlockOutline>
{
    auto outlines = std::vector<BlockOutline>{};

    
    //--- Find the new concave angles on the block outline
    auto starting_pivots = std::vector<OutlinePivot>{};

    // Top horizontal borders
    for (auto y = vol.right(); y >= vol.left; --y)
    {
        auto const pos = Vector3i{ vol.behind, y, vol.down };

        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Examining if the tile adjacent to this border could be an outline starting angle.", 5); 
            BEdeb.highlight_tile(pos, Color(125, 125, 125, 50));
        #endif

        auto const external_pos = pos + Vector3i{ -1,  0, 0 };
        auto const fwd_drc = Vector3i{ 0, -1, 0 };

        auto const res = is_outlineConcaveStartingAngle(external_pos, fwd_drc, replaced_areas);
        if (res)
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("New concave starting angle found.", 5); 
                BEdeb.highlight_tile(external_pos, Color::Red);
                BEdeb.highlight_tile(external_pos + fwd_drc, Color::Cyan);
            #endif

            starting_pivots.emplace_back(external_pos, res.value());
        }
    }
    // Left vertical border
    for (auto x = vol.behind; x <= vol.front(); ++x)
    {
        auto const pos = Vector3i{ x, vol.left, vol.down };

        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Examining if the tile adjacent to this border could be an outline starting angle.", 5);
            BEdeb.highlight_tile(pos, Color(125, 125, 125, 50));
        #endif
            
        auto const external_pos = pos + Vector3i{  0, -1, 0 };
        auto const fwd_drc = Vector3i{ 1,  0, 0 };

        auto const res = is_outlineConcaveStartingAngle(external_pos, fwd_drc, replaced_areas);
        if (res)
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("New concave starting angle found.", 5); 
                BEdeb.highlight_tile(external_pos, Color::Red);
                BEdeb.highlight_tile(external_pos + fwd_drc, Color::Cyan);
            #endif

            starting_pivots.emplace_back(external_pos, res.value());
        }
    }
    // Bottom horizontal border
    for (auto y = vol.left; y <= vol.right(); ++y)
    {
        auto const pos = Vector3i{ vol.front(), y, vol.down };

        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Examining if the tile adjacent to this border could be an outline starting angle.", 5);
            BEdeb.highlight_tile(pos, Color(125, 125, 125, 50));
        #endif

        auto const external_pos = pos + Vector3i{  1,  0, 0 };
        auto const fwd_drc = Vector3i{ 0,  1, 0 };

        auto const res = is_outlineConcaveStartingAngle(external_pos, fwd_drc, replaced_areas);
        if (res)
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("New concave starting angle found.", 5); 
                BEdeb.highlight_tile(external_pos, Color::Red);
                BEdeb.highlight_tile(external_pos + fwd_drc, Color::Cyan);
            #endif

            starting_pivots.emplace_back(external_pos, res.value());
        }
    }
    // Right vertical border	
    for (auto x = vol.front(); x >= vol.behind; --x)
    {
        auto const pos = Vector3i{ x, vol.right(), vol.down };

        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Examining if the tile adjacent to this border could be an outline starting angle.", 5);
            BEdeb.highlight_tile(pos, Color(125, 125, 125, 50));
        #endif
            
        auto const external_pos = pos + Vector3i{  0,  1, 0 };
        auto const fwd_drc = Vector3i{ -1,  0, 0 };

        auto const res = is_outlineConcaveStartingAngle(external_pos, fwd_drc, replaced_areas);
        if (res)
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("New concave starting angle found.", 5); 
                BEdeb.highlight_tile(external_pos, Color::Red);
                BEdeb.highlight_tile(external_pos + fwd_drc, Color::Cyan);
            #endif

            starting_pivots.emplace_back(external_pos, res.value());
        }
    }



    //--- Find the new convex angles on the block outlines
    auto const NE_externalPos = vol.begin() + Vector3i{		  -1, vol.width, 0 };
    auto const NW_externalPos = vol.begin() + Vector3i{		  -1,		  -1, 0 };
    auto const SW_externalPos = vol.begin() + Vector3i{ vol.length,		  -1, 0 };
    auto const SE_externalPos = vol.begin() + Vector3i{ vol.length, vol.width, 0 };

    auto const NE_fwdDrc = Vector3i{ -1,  0, 0 };
    auto const NW_fwdDrc = Vector3i{  0, -1, 0 };
    auto const SW_fwdDrc = Vector3i{  1,  0, 0 };
    auto const SE_fwdDrc = Vector3i{  0,  1, 0 };

    if (is_outlineConvexStartingAngle(NE_externalPos, NE_fwdDrc, replaced_areas))
    {
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("New convex starting angle found.", 5); 
            BEdeb.highlight_tile(NE_externalPos, Color::Red);
            BEdeb.highlight_tile(NE_externalPos + NE_fwdDrc, Color::Cyan);
        #endif

        starting_pivots.emplace_back(NE_externalPos, NE_fwdDrc);
    }
    if (is_outlineConvexStartingAngle(NW_externalPos, NW_fwdDrc, replaced_areas))
    {
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("New convex starting angle found.", 5); 
            BEdeb.highlight_tile(NW_externalPos, Color::Red);
            BEdeb.highlight_tile(NW_externalPos + NW_fwdDrc, Color::Cyan);
        #endif

        starting_pivots.emplace_back(NW_externalPos, NW_fwdDrc);
    }
    if (is_outlineConvexStartingAngle(SW_externalPos, SW_fwdDrc, replaced_areas))
    {
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("New convex starting angle found.", 5); 
            BEdeb.highlight_tile(SW_externalPos, Color::Red);
            BEdeb.highlight_tile(SW_externalPos + SW_fwdDrc, Color::Cyan);
        #endif

        starting_pivots.emplace_back(SW_externalPos, SW_fwdDrc);
    }
    if (is_outlineConvexStartingAngle(SE_externalPos, SE_fwdDrc, replaced_areas))
    {
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("New convex starting angle found.", 5); 
            BEdeb.highlight_tile(SE_externalPos, Color::Red);
            BEdeb.highlight_tile(SE_externalPos + SE_fwdDrc, Color::Cyan);
        #endif

        starting_pivots.emplace_back(SE_externalPos, SE_fwdDrc);
    }
    

    #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
        BEdeb.new_step("Outline starting angles.", 4);
        for (auto const [pos, fwd_drc] : starting_pivots)
        {
            BEdeb.highlight_tile(pos, Color::Red);
            BEdeb.highlight_tile(pos + fwd_drc, Color::Cyan);
        }
    #endif


    //--- Trace the outlines	
    return compute_outlines(starting_pivots, vol, replaced_areas);
}

auto BuildingManager::is_outlineConcaveStartingAngle(Vector3i const pos, Vector3i const forward_drc,
                                                     std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const
    -> std::optional<Vector3i>
{
    auto ret = std::optional<Vector3i>{};

    if (!is_tile_builtWithUnreplacedArea(pos, replaced_areas))
    {
        auto const front      = is_tile_builtWithUnreplacedArea(pos + forward_drc, replaced_areas);
        auto const back       = is_tile_builtWithUnreplacedArea(pos - forward_drc, replaced_areas);
        auto const right_side = is_tile_builtWithUnreplacedArea(pos + DirectionUtil::orthogonalRight_planeUnitVector(forward_drc), replaced_areas);

        if      ( front && !back /* && whatever right_side*/)
        {
            ret.emplace(forward_drc);
        }
        else if ( front &&  back /* && whatever right_side*/)
        {
            ret.emplace(DirectionUtil::orthogonalLeft_planeUnitVector(forward_drc));
        }
        //else if (!front && !back && whatever right_side) // do nothing
        else if (!front &&  back && !right_side)
        {
            ret.emplace(DirectionUtil::orthogonalLeft_planeUnitVector(forward_drc));
        }
        else if (!front &&  back &&  right_side)
        {
            ret.emplace(- forward_drc);
        }
    }

    return ret;
}

bool BuildingManager::is_outlineConvexStartingAngle(Vector3i const pos, Vector3i const forward_drc,
                                                    std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const
{
    return !is_tile_builtWithUnreplacedArea(pos, replaced_areas)																// The tile itself
        && !is_tile_builtWithUnreplacedArea(pos + DirectionUtil::orthogonalLeft_planeUnitVector(forward_drc), replaced_areas)	// The next tile
        && !is_tile_builtWithUnreplacedArea(pos - forward_drc, replaced_areas);													// The previous tile
}

auto BuildingManager::compute_outline(OutlinePivot const starting_pivot, BuildingAreaCompleteId const ignored_area) const -> BlockOutline 
{	
    return compute_outlines({ starting_pivot }, {}, {ignored_area}).front(); //exploiting the fact that replaced_areas are ignored when compute the outlines
}

auto BuildingManager::compute_outlines(std::vector<OutlinePivot> starting_pivots, 
                                       IntParallelepiped const& ghost_vol,
                                       std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const
    -> std::vector<BlockOutline>
{
    auto outlines = std::vector<BlockOutline>{};

    while (!starting_pivots.empty())
    {
        auto const starting = starting_pivots.front();
        starting_pivots.erase(starting_pivots.begin());

        auto & outline = outlines.emplace_back();
        
        auto previous = starting;

        auto mock_outline = BlockOutline{}; // The step zero shouldn't save data in the real outline
        auto current = advance_outlinePos(starting.pos, starting.drc, ghost_vol, replaced_areas, mock_outline);
        

        // If the current pivot lies in the middle of a segment, then reach an angle.
        while (current.drc == previous.drc)
        {
            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("Advancing the pivot up to an angle", 5);
                BEdeb.highlight_tile(current.pos, Color::Red);
                BEdeb.highlight_tile(current.pos + current.drc, Color::Cyan);
                BEdeb.highlight_tile(current.pos + DirectionUtil::orthogonalLeft_planeUnitVector(current.drc), Color{ 150, 150, 150, 123 });
            #endif

            previous = current;
            current = advance_outlinePos(current.pos, current.drc, ghost_vol, replaced_areas, mock_outline);
        }
        
        auto const adj_starting = previous;
        auto segment_beg = adj_starting.pos;

        
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            BEdeb.new_step("Outline step 0 - previous", 5);
            BEdeb.highlight_tile(previous.pos, Color::Grey);
            BEdeb.highlight_tile(previous.pos + previous.drc, Color{ 190, 190, 190 });
            BEdeb.new_step("Outline step 0 - current", 5);
            BEdeb.highlight_tile(current.pos, Color::Red);
            BEdeb.highlight_tile(current.pos + current.drc, Color::Cyan);
            BEdeb.highlight_tile(current.pos + DirectionUtil::orthogonalLeft_planeUnitVector(current.drc), Color{ 150, 150, 150, 123 });
        #endif

        do
        {
            previous = current;

            current = advance_outlinePos(current.pos, current.drc, ghost_vol, replaced_areas, outline);

            // If there is a starting angle in this position, then discard it (it would generate the same outline)
            auto it = std::find(starting_pivots.begin(), starting_pivots.end(), current);
            if (it != starting_pivots.end())
            {
                starting_pivots.erase(it);
            }
                
            // If the direction has changed, then register the segment
            if (current.drc != previous.drc)
            {
                auto length = 0;

                if (previous.pos.y == segment_beg.y)			// Horizontal
                {
                    length = std::abs(previous.pos.x - segment_beg.x) + 1;
                }
                else if (previous.pos.x == segment_beg.x)		// Vertical
                {
                    length = std::abs(previous.pos.y - segment_beg.y) + 1;
                }
                else
                {
                    throw std::runtime_error("Unexpected case. A segment should be either vertical or horizontal, not diagonal.");
                }

                outline.segments.push_back({ segment_beg, previous.drc, length });

                segment_beg = previous.pos;
            }

            #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
                BEdeb.new_step("Outline step.", 5);
                BEdeb.highlight_tile(current.pos, Color::Red);
                BEdeb.highlight_tile(current.pos + current.drc, Color::Cyan);
                BEdeb.highlight_tile(current.pos + DirectionUtil::orthogonalLeft_planeUnitVector(current.drc), Color{ 150, 150, 150, 123 });
            #endif

        } while (previous != adj_starting);

        #if DYNAMIC_ASSERTS
            outline.assert_angleCount();
        #endif
        
        #if BUILDEXP_VISUALDEBUG_IS_AREA_BUILDABLE
            std::ostringstream oss; oss << "Outline. Concave #: " << outline.concave_count << ". Convex #: " << outline.convex_count;
            BEdeb.new_step(oss.str(), 4);
            for (auto const& segment : outline.segments) 
            {
                for (auto p = segment.begin; p != segment.begin + segment.drc * segment.length; p += segment.drc)
                {
                    BEdeb.highlight_tile(p, Color::Green);
                }
            }
            BEdeb.highlight_tiles(outline.external_doors.cbegin(), outline.external_doors.cend(), Color::Mint);

            for (auto const& segment : outline.segments)
            {
                std::ostringstream oss; oss << "Outline segment (length: " << segment.length << ", drc: " << segment.drc << ").";
                BEdeb.new_step(oss.str(), 5);
                for (auto p = segment.begin; p != segment.begin + segment.drc * segment.length; p += segment.drc)
                {
                    BEdeb.highlight_tile(p, Color::Green);
                }
            }
        #endif
    }

    return outlines;
}

auto BuildingManager::advance_outlinePos(Vector3i const pos, Vector3i const drc,
                                         IntParallelepiped const& vol,
                                         std::unordered_set<BuildingAreaCompleteId> const& replaced_areas,
                                         BlockOutline & outline) const 
    -> OutlinePivot
{
    auto next = OutlinePivot{};
    auto & next_pos = next.pos;
    auto & next_drc = next.drc;

    auto const ortho_left = DirectionUtil::orthogonalLeft_planeUnitVector(drc);


    auto const left_side = pos + ortho_left;
    auto const& leftSide_t = m_tiles.get_existent(left_side);
    if (!is_tile_builtWithUnreplacedArea(leftSide_t, replaced_areas) && !vol.contains(left_side))
    {
        next_pos = left_side;
        next_drc = ortho_left;

        ++outline.convex_count;
    }
    else
    {
        if (leftSide_t.is_externalDoor()) { outline.external_doors.push_back(left_side); }

        auto const front = pos + drc;
        auto const& front_t = m_tiles.get_existent(front);
        if (!is_tile_builtWithUnreplacedArea(front_t, replaced_areas) && !vol.contains(front))
        {
            next_pos = front;
            next_drc = drc;
        }
        else
        {
            auto const right_side = pos - ortho_left;
            auto const& rightSide_t = m_tiles.get_existent(right_side);
            if (!is_tile_builtWithUnreplacedArea(rightSide_t, replaced_areas) && !vol.contains(right_side))
            {
                next_pos = pos;
                next_drc = - ortho_left;

                ++outline.concave_count;
            }
            else
            {

                next_pos = pos;
                next_drc = - ortho_left;

                ++outline.concave_count;
            }
        }
    }

    return next;
}

bool BuildingManager::is_tile_builtWithUnreplacedArea(Tile const& t, std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const
{
    if (t.is_built())
    {
        auto are_allAreas_replaced = true;

        for (auto const acid : t.areas())
        {
            if (replaced_areas.find(acid) == replaced_areas.cend())
            {
                are_allAreas_replaced = false;
                break;
            }
        }

        return !are_allAreas_replaced;
    }
    else
    {
        return false;
    }
}

bool BuildingManager::is_tile_builtWithUnreplacedArea(Vector3i const pos, std::unordered_set<BuildingAreaCompleteId> const& replaced_areas) const
{
    auto const& t = m_tiles.get_existent(pos);

    return is_tile_builtWithUnreplacedArea(t, replaced_areas);
}


bool BuildingManager::compute_bestPosition(Vector2i const area_dims,
                                           int const road_dist,
                                           std::vector<BuildablePosition> const& buildable_positions, 
                                           Vector3i & best_position,
                                           std::vector<BuildingAreaCompleteId> & replaced_areas) const
{
    #if DYNAMIC_ASSERTS
        if (buildable_positions.empty()) { throw std::runtime_error("Cannot compute the best position if there's no buildable position."); }
    #endif

    // Positions with the greatest number of adjacent points (they are all tied for the same number of shared borders)
    std::vector<std::vector<BuildablePosition>::const_iterator> best_positions;
    // Score of the best position (or the tied for first best positions)
    auto bestPoss_score = 0;


    #if BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS
        // Adjacency points associated to each best position.
        std::vector<std::unordered_set<Vector3i>> debug_bestPoss_adjacencyPointSets;
    #endif

    for (auto it = buildable_positions.cbegin(); it < buildable_positions.cend(); ++it)
    {
        auto adjacent_poss = 0;

        #if BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS
            // Collection of all the adjacency points found for this position. For debug purpose only.
            std::unordered_set<Vector3i> sharedBorders_set;
        #endif

        auto const pos = it->pos;
        auto const& areas_to_replace = it->replaced_areas;

        auto const x_top    = pos.x					  - road_dist;
        auto const x_bottom = pos.x + area_dims.x - 1 + road_dist;
        auto const y_left   = pos.y					  - road_dist;
        auto const y_right  = pos.y + area_dims.y - 1 + road_dist;
        auto const z = pos.z;

        // Check horizontal borders
        for (auto y = y_left; y <= y_right; ++y)
        {
            auto const t_top = m_tiles.get(x_top, y, z); //no need to check if the tile is existent. It's beeen already checked in is_area_buildable()

            if (is_adjacencyPoint(*t_top, areas_to_replace))
            {
                ++adjacent_poss;
                #if BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS
                    sharedBorders_set.insert({ x_top, y, z });
                #endif
            }
            
            auto const t_bottom = m_tiles.get(x_bottom, y, z);

            if (is_adjacencyPoint(*t_bottom, areas_to_replace))
            {
                ++adjacent_poss;
                #if BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS
                    sharedBorders_set.insert({ x_bottom, y, z });
                #endif
            }
        }

        // Check vertical borders (apart the corner tiles, already checked above)
        for (auto x = x_top + 1; x <= x_bottom - 1; ++x)
        {
            auto const t_left = m_tiles.get(x, y_left, z); //no need to check if the tile is existent. It's already beeen checked in is_area_buildable()

            if (is_adjacencyPoint(*t_left, areas_to_replace))
            {
                ++adjacent_poss;
                #if BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS
                    sharedBorders_set.insert({ x, y_left, z });
                #endif
            }
            
            auto const t_right = m_tiles.get(x, y_right, z);

            if (is_adjacencyPoint(*t_right, areas_to_replace))
            {
                ++adjacent_poss;
                #if BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS
                    sharedBorders_set.insert({ x, y_right, z });
                #endif
            }
        }
        
        #if BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS
            {
                std::ostringstream oss;
                oss << "MinDim: " << area_dims << "\t\tShared borders: " << adjacent_poss;
                BEdeb.new_step(oss.str(), 3);
                BEdeb.highlight_tile(it->pos, Color(125, 125, 125), true);
                BEdeb.highlight_tiles(sharedBorders_set.cbegin(), sharedBorders_set.cend(), Color(75, 75, 75));
            }
        #endif			

        // Formula to rank the different buildable positions in order to choose the best one
        auto const pos_score = static_cast<int>(adjacent_poss); //static_cast<int>() because if I use a more complicated formula involving floats, I'll need to convert back to int.

        // If this buildable_position is tied for the best position with the previous ones, then consider all them
        if (pos_score == bestPoss_score)
        {
            best_positions.push_back(it);
            
            #if BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS
                debug_bestPoss_adjacencyPointSets.push_back(sharedBorders_set);
            #endif
        }
        // Otherwise, if this is the only best position, then discard the previous ones and store only this one
        else if (pos_score > bestPoss_score)
        {
            best_positions.clear();
            best_positions.push_back(it);
            bestPoss_score = pos_score;
            #if BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS
                debug_bestPoss_adjacencyPointSets.clear();
                debug_bestPoss_adjacencyPointSets.push_back(sharedBorders_set);
            #endif
        }
        // Otherwise do nothing and consider the next buildable position.
    }

    if (!buildable_positions.empty())
    {
        #if BUILDEXP_VISUALDEBUG_BUILDABLE_POSITIONS
            for (decltype(best_positions)::size_type i = 0; i < best_positions.size(); ++i)
            {
                std::ostringstream oss;
                oss << "Best position (" << i + 1 << "th of " << best_positions.size() << ") ( MinDim: " << area_dims << ") " 
                    << "\nScore: " << bestPoss_score 
                    << "\t\tAdjacent poss: " << debug_bestPoss_adjacencyPointSets[i].size();
                BEdeb.new_step(oss.str(), 2);
                BEdeb.highlight_tile(Vector3i(best_positions[i]->pos), Color::Blue, true);
                BEdeb.highlight_tiles(debug_bestPoss_adjacencyPointSets[i].cbegin(), debug_bestPoss_adjacencyPointSets[i].cend(), Color(75, 75, 75));
            }
        #endif

        // Randomize the positions to avoid that a building will always expand in the same way.
        std::shuffle(best_positions.begin(), best_positions.end(), m_random_generator);
        auto best_pos = best_positions[0];

        #if BUILDEXP_VISUALDEBUG
            BEdeb.new_step("Randomly selected best position", 1);
            BEdeb.highlight_tile(Vector3i(best_pos->pos), Color(255, 110, 199), true);
        #endif

        best_position = best_pos->pos;
        replaced_areas.insert(replaced_areas.cend(), best_pos->replaced_areas.cbegin(), best_pos->replaced_areas.cend());//TODO: PERFORMANCE: Avoid this wasteful copy...
        return true;
    }
    else
    {
        return false;
    }
}


bool BuildingManager::is_adjacencyPoint(Tile const& t, std::unordered_set<BuildingAreaCompleteId> const& areas_to_replace)
{
    if (!t.is_border()) 
    { 
        return false; 
    }
    else
    {
        for (auto const info : t.get_borderInfos())
        {
            if (!info.is_empty() && areas_to_replace.find(info.acid()) == areas_to_replace.cend())
            {
                return true;
            }
        }

        return false;
    }
}





auto BuildingManager::internal_build_building(CityId const cid, CityBlockId const cbid, 
                                              Vector3i const pos, BuildingRecipe const& recipe, 
                                              std::vector<BuildingAreaCompleteId> const& replaced_areas,
                                              CityBlock & cblock)
    -> BuildingId
{
    #if PLAYERMOVEMENT_VISUALDEBUG
        PMdeb.begin_chapter("Building building in the nearest city.", pos); // Necessary because unbuild_replacedArea() and build_buildingArea() change impassable tiles
    #endif
    
    // Unbuild replaced areas from the respective buildings
    for (auto const acid : replaced_areas)
    {
        unbuild_replacedArea(acid, cid, cbid, 0);
    }


    auto & bld_el = m_buildings.create(buildingExpansionTemplates_nameToId.at(recipe.expansion_template()), cid, cbid);
    auto const new_bid = bld_el.id();
    auto & new_building = bld_el.value;

    auto const& [starting_aid, starting_area] = new_building.create_area(recipe.startingArea_type(), pos, recipe.startingArea_dims(),
                                                                         building_expansionTemplates.at(new_building.expTempl_id()));
        
    build_buildingArea(cbid, new_bid, starting_aid, starting_area, new_building);
    
    cblock.add_building(new_bid);
    cblock.increase_surface(starting_area.volume());
    

    #if PLAYERMOVEMENT_VISUALDEBUG
        PMdeb.end_chapter();
    #endif

    #if BUILDEXP_VISUALDEBUG
        BEdeb.add_building(new_bid, new_building);
        BEdeb.new_step("Building built in the city.", 1);
    #endif	

    return new_bid;
}

void BuildingManager::internal_expand_building(BuildingId const bid, 
                                               AreaType const selected_area, Vector3i const best_position,
                                               std::vector<BuildingAreaCompleteId> const& replaced_areas,
                                               CityBlock & cblock, Building & building)
{
    #if PLAYERMOVEMENT_VISUALDEBUG
        PMdeb.begin_chapter("Expanding building");	// Necessary because unbuild_replacedArea() and build_buildingArea() change impassable tiles
    #endif
    
    //TODO: 02: When replacing an area take into account its role as edge in linking the building nodes
    for (auto const acid : replaced_areas)
    {	
        unbuild_replacedArea(acid, building.cid(), building.cbid(), bid);
    }

    auto const selectedArea_minDims = area_templates.at(selected_area).min_dims();

    auto const& [new_aid, new_area] = building.create_area(selected_area, best_position, selectedArea_minDims,
                                                           building_expansionTemplates.at(building.expTempl_id()));
    build_buildingArea(building.cbid(), bid, new_aid, new_area, building);
                
    cblock.increase_surface(new_area.volume());
    

    #if PLAYERMOVEMENT_VISUALDEBUG
        PMdeb.end_chapter();
    #endif

    #if BUILDEXP_VISUALDEBUG
        BEdeb.update_building(bid, building);
        BEdeb.new_step("Building expanded.", 1);
    #endif
}

//TODO: 02: When replacing an area take into account its role as edge in linking the building nodes
void BuildingManager::unbuild_replacedArea(BuildingAreaCompleteId const ra_acid, CityId const newArea_cid, CityBlockId const newArea_cbid, BuildingId const newArea_bid)
{
    auto & b = m_buildings.get_or_throw(ra_acid.bid);
    auto & ra_city = m_cities.get_or_throw(b.cid());
    auto & ra_block = m_blocks.get_or_throw(b.cbid());
    
    auto const& ra_vol = b.getOrThrow_area(ra_acid.aid).volume();
    ra_block.decrease_surface(ra_vol);

    unbuild_buildingArea(ra_acid.bid, b, ra_acid.aid);
    b.remove_area(ra_acid.aid);

    // If this was the last area of this building (and the new area is built in another building), then destroy this building.
    if (ra_acid.bid != newArea_bid && b.is_empty())
    {
        ra_block.remove_building(ra_acid.bid);
            
        // If that was the last building of the block (and the block isn't the same in which the new building have to be built), then destroy the block
        if (b.cbid() != newArea_cbid && ra_block.empty())
        {
            m_blocks.destroy(b.cbid());
            ra_city.remove_block(b.cbid());
                
            // If that was the last block of the city (and the city isn't the same in which the new building have to be built), then destroy the city.
            if (ra_city.empty())
            {
                #if DYNAMIC_ASSERTS
                    // Check for an absurd situation. ra_block wan't the block of the city in which the new building have to be built, but
                    // it was however the only block of the city. So in which block the building should have been built?!
                    if (b.cid() == newArea_cid) { throw std::runtime_error("Unexpected situation: the city doesn't have any block. But it's the same city in which the new building have to be built."); }
                #endif

                m_cities.destroy(b.cid());
            }
        }

        m_buildings.destroy(ra_acid.bid);	//I delete it only here because I need its cid() and cbid() above.
    }
}

void BuildingManager::build_buildingArea(CityBlockId const cbid, BuildingId const bid, BuildingAreaId const aid, BuildingArea const& area, Building & building)
{
    auto const& vol = area.volume();
    auto const& atempl = area_templates.at(area.type());
    auto const& tile_style = atempl.tile_style();
    
    #if BUILDEXP_VISUALDEBUG_DOORS
        BEdeb.new_step("Building BuildingArea", 3);
        BEdeb.focus_onPosition(Vector2f(vol.behind, vol.left));
        BEdeb.highlight_tilesRect(vol.get_base(), vol.down, Color{ 128, 128, 128, 128 });
    #endif


    //--- Remove external doors that would be occluded by this area
    auto const area_pos = area.volume().begin();
    auto const area_dims = area.volume().base_dims();
    auto doors_to_remove = std::unordered_set<Vector3i>{};

    gather_adjacent_externalDoors(area_pos, area_dims, doors_to_remove);

    auto const outlines = compute_areaOutlines(area.volume(), {});
    for (auto const& outline : outlines)
    {
        // If this is an internal outline, then its external doors have to be removed
        if (outline.is_internal()) { doors_to_remove.insert(outline.external_doors.cbegin(), outline.external_doors.cend()); }
    }

    for (auto const d : doors_to_remove)
    {
        try_unbuild_door(d.x, d.y, d.z);
    }

    #if BUILDEXP_VISUALDEBUG_DOORS
        BEdeb.new_step("Removed doors", 5);
        BEdeb.highlight_tiles(doors_to_remove.cbegin(), doors_to_remove.cend(), Color::Red);
    #endif



    // Build inner area
    for (auto y = vol.left + 1; y <= vol.right() - 1; ++y)
    {
        for (auto x = vol.behind + 1; x <= vol.front() - 1; ++x)
        {
            m_tiles.build_innerArea(x, y, vol.down, cbid, bid, aid, tile_style);
        }
    }

    // Build horizontal borders
    for (auto y = vol.left; y <= vol.right(); ++y)
    {
        auto const x_top = vol.behind;
        auto const x_bottom = vol.front();

        //TODO: The borderstyle must be derived from the AreaTemplate
        m_tiles.build_border(x_top, y, vol.down, cbid, bid, aid, BorderStyle::brickWall);
        m_tiles.build_border(x_bottom, y, vol.down, cbid, bid, aid, BorderStyle::brickWall);
    }

    // Build vertical borders (apart the corner tiles, already built above)
    for (auto x = vol.behind + 1; x <= vol.front() - 1; ++x)
    {
        auto const y_left = vol.left;
        auto const y_right = vol.right();

        m_tiles.build_border(x, y_left, vol.down, cbid, bid, aid, BorderStyle::brickWall);
        m_tiles.build_border(x, y_right, vol.down, cbid, bid, aid, BorderStyle::brickWall);
    }


    #if BUILDEXP_DEBUGLOG
        BElog << Logger::nltb << "Built a BuildingArea of volume: " << vol;
    #endif

      


    //--- Find a place for the doors

    auto internal_doorablePoss = std::vector<DoorablePosition>{};
    auto external_doorablePoss = std::vector<DoorablePosition>{};

    // Check top border
    check_borderDoorablePoss({ vol.behind, vol.left + 1,  vol.down },   { vol.behind, vol.right(), vol.down },  Versor3i::E, internal_doorablePoss, external_doorablePoss);
    // Check bottom border
    check_borderDoorablePoss({ vol.front(), vol.left + 1, vol.down },   { vol.front(), vol.right(), vol.down }, Versor3i::E, internal_doorablePoss, external_doorablePoss);
    // Check left border
    check_borderDoorablePoss({ vol.behind + 1, vol.left, vol.down },    { vol.front(), vol.left, vol.down },    Versor3i::S, internal_doorablePoss, external_doorablePoss);
    // Check right border
    check_borderDoorablePoss({ vol.behind + 1, vol.right(), vol.down }, { vol.front(), vol.right(), vol.down }, Versor3i::S, internal_doorablePoss, external_doorablePoss);

    #if BUILDEXP_VISUALDEBUG_DOORS
        visualDebug_doorablePositionsStep("Internal doorable positions for this area", internal_doorablePoss);
        visualDebug_doorablePositionsStep("External doorable positions for this area", external_doorablePoss);
    #endif

    //TODO: 02: Aggiungere adjacent_list ai nodi (aree) del grafo.
    //TODO: 02: Aggiungere porte come membro a quest'area
    for (auto const& dpos : internal_doorablePoss)
    {
        internal_build_internalDoor(dpos.pos, dpos.vertical, tile_style);
    }
    

    remove_blindDoors(outlines, external_doorablePoss);
    
    #if BUILDEXP_VISUALDEBUG_DOORS
        visualDebug_doorablePositionsStep("Non-blind external doorable positions for this area", external_doorablePoss);
    #endif

    //if (building.external_doors().size() < 2 && !external_doorablePoss.empty()) //TODO: 04: Ripristinare questa condizione
    //{
        //std::shuffle(external_doorablePoss.begin(), external_doorablePoss.end(), m_random_generator);

        //auto it = external_doorablePoss.cbegin();

        //internal_build_externalDoor(door->first, door->second, tile_style, building);
    //}
    for (auto const& dpos : external_doorablePoss)
    {
        internal_build_externalDoor(dpos.pos, dpos.vertical, tile_style, building);
    }

    
    //--- Add blind doors to newly created courtyards.
    auto mock_internalDoorablePoss = std::vector<DoorablePosition>{};
    auto blind_doorablePoss = std::vector<DoorablePosition>{};

    for (auto const& ol : outlines)
    {
        if (ol.is_internal())
        {
            for (auto const& seg : ol.segments)
            {
                auto const bdrc = DirectionUtil::orthogonalLeft_planeUnitVector(seg.drc);	//where is the border in relation to the segment

                check_borderDoorablePoss(seg.begin + bdrc, seg.end() + bdrc, seg.drc, mock_internalDoorablePoss, blind_doorablePoss);
            }
        }
    }
    
    #if BUILDEXP_VISUALDEBUG_DOORS
        visualDebug_doorablePositionsStep("Blind external doorable positions related to this area", blind_doorablePoss);
    #endif

    for (auto const& dpos : blind_doorablePoss)
    {
        auto & blindDoor_bld = m_buildings.get_or_throw(dpos.area1.bid);				//the blind doors aren't necessarily owned by the same building of this area 
        internal_build_blindDoor(dpos.pos, dpos.vertical, tile_style, blindDoor_bld);
    }

    
    if(sim_settings.map.generate_roofs)
    {
        // Cut and reshape possibly present roofs in order to make room for the new volume
        reshape_existentRoofs(bid, vol);
    }
    
    //--- Mention the tile changes to the TileGraphicsManager
    m_tgraphics_mediator.record_areaChange(vol);
}

void BuildingManager::check_borderDoorablePoss(Vector3i const beg, Vector3i const end, Vector3i const drc,
                                               std::vector<DoorablePosition> & internal_doorablePoss,
                                               std::vector<DoorablePosition> & external_doorablePoss) const
{
    #if DYNAMIC_ASSERTS
        if      (beg == end)							 { throw std::runtime_error("Wrong input: null segments not accepted."); }
        else if (beg.z != end.z)						 { throw std::runtime_error("Wrong input: non-planar segments not accepted."); }
        else if (beg.x != end.x && beg.y != end.y)		 { throw std::runtime_error("Wrong input: diagonal segments not accepted."); }
        else if (std::abs(drc.x) + std::abs(drc.y) != 1) { throw std::runtime_error("Wrong input: diagonal or null directions not accepted."); }
    #endif

    auto const vertical = drc.x != 0;

    auto const compute_midpoint = [](Vector3i const begin, Vector3i const last) { return (begin + last) / 2; };

    auto previous_type = BorderDoorType::None;
    auto current_area1 = BuildingAreaCompleteId{};		//the first area that currently share the border
    auto current_area2 = BuildingAreaCompleteId{};		//the second area that currently share the border (null in the case of external doors)
    auto bound_begin = Vector3i{};						//first doorable tile var associated to current neighbor area
    auto bound_last = Vector3i{};						//last doorable tile var associated to current neighbor area
    for (auto pos = beg; pos != end; pos += drc)
    {
        auto const door_info = m_tiles.compute_borderDoorType(pos);

        if (door_info.type != previous_type)
        {
            switch (door_info.type)
            {
                case BorderDoorType::None:
                {
                    if (previous_type == BorderDoorType::Internal)
                    {
                        auto const midpoint = compute_midpoint(bound_begin, bound_last);
                        internal_doorablePoss.push_back({ midpoint, current_area1, current_area2, vertical });
                        
                        #if BUILDEXP_VISUALDEBUG_DOORS
                            BEdeb.new_step("Internal doorable position", 5);
                            BEdeb.highlight_tile(midpoint, Color::Red);
                        #endif
                    }
                    else if (previous_type == BorderDoorType::External)
                    {
                        auto const midpoint = compute_midpoint(bound_begin, bound_last);
                        external_doorablePoss.push_back({ midpoint, current_area1, current_area2, vertical });

                        #if BUILDEXP_VISUALDEBUG_DOORS
                            BEdeb.new_step("External doorable position", 5); 
                            BEdeb.highlight_tile(midpoint, Color::Red);
                        #endif
                    }

                    current_area1 = BuildingAreaCompleteId{};
                    current_area2 = BuildingAreaCompleteId{};
                    bound_begin = Vector3i{};
                    bound_last = Vector3i{};

                    break;
                }
                case BorderDoorType::Internal:
                {
                    current_area1 = door_info.area1;
                    current_area2 = door_info.area2;
                    bound_begin = pos;
                    bound_last = pos;

                    break;
                }
                case BorderDoorType::BuildingsLinker:
                {
                    break;
                }
                case BorderDoorType::External:
                {
                    current_area1 = door_info.area1;
                    current_area2 = BuildingAreaCompleteId{};
                    bound_begin = pos;
                    bound_last = pos;

                    break;
                }
                default:
                    throw std::runtime_error("Unexpected case.");
                    break;
            }
        }
        else
        {
            switch (door_info.type)
            {
                case BorderDoorType::None:
                {
                    break;
                }
                case BorderDoorType::Internal:
                {
                    bound_last = pos;

                    break;
                }
                case BorderDoorType::BuildingsLinker:
                {
                    break;
                }
                case BorderDoorType::External:
                {
                    bound_last = pos;

                    break;
                }
                default:
                    throw std::runtime_error("Unexpected case.");
                    break;
            }
        }
        
        
        #if BUILDEXP_VISUALDEBUG_DOORS
            std::ostringstream oss; oss << "DoorInfo: " << door_info;
            BEdeb.new_step(oss.str(), 5);
            BEdeb.highlight_tile(bound_begin, Color::Yellow);
            BEdeb.highlight_tile(pos, Color::Mint);
        #endif


        previous_type = door_info.type;
    }
    
    // Add last area midpoint
    if (previous_type == BorderDoorType::Internal)
    {
        auto const midpoint = compute_midpoint(bound_begin, bound_last);
        internal_doorablePoss.push_back({ midpoint, current_area1, current_area2, vertical });

        #if BUILDEXP_VISUALDEBUG_DOORS
            BEdeb.new_step("Internal doorable position", 5);
            BEdeb.highlight_tile(midpoint, Color::Red);
        #endif
    }
    else if (previous_type == BorderDoorType::External)
    {
        auto const midpoint = compute_midpoint(bound_begin, bound_last);
        external_doorablePoss.push_back({ midpoint, current_area1, current_area2, vertical });

        #if BUILDEXP_VISUALDEBUG_DOORS
            BEdeb.new_step("External doorable position", 5);
            BEdeb.highlight_tile(midpoint, Color::Red);
        #endif
    }
}

bool BuildingManager::is_verticalDoor(Vector3i const pos) const
{
    return m_tiles.get_existent(pos + Versor3i::N).is_border();
}

bool BuildingManager::is_externalBlindDoor(Vector3i const pos, BuildingAreaCompleteId const ignored_area) const
{
    auto pivot = OutlinePivot{};

    auto const vertical = is_verticalDoor(pos);

    if (vertical)
    {
        auto const& W_t = m_tiles.get_existent(pos + Versor3i::W);
        auto const& E_t = m_tiles.get_existent(pos + Versor3i::E);

        if      ( W_t.is_built() && !E_t.is_built()) { pivot.pos = pos + Versor3i::E; pivot.drc = Versor3i::N; }
        else if (!W_t.is_built() &&  E_t.is_built()) { pivot.pos = pos + Versor3i::W; pivot.drc = Versor3i::S; }
        else    { throw std::runtime_error("Unexpected case."); }
    }
    else
    {
        auto const& N_t = m_tiles.get_existent(pos + Versor3i::N);
        auto const& S_t = m_tiles.get_existent(pos + Versor3i::S);

        if      ( N_t.is_built() && !S_t.is_built()) { pivot.pos = pos + Versor3i::S; pivot.drc = Versor3i::E; }
        else if (!N_t.is_built() &&  S_t.is_built()) { pivot.pos = pos + Versor3i::N; pivot.drc = Versor3i::W; }
        else	{ throw std::runtime_error("Unexpected case."); }
    }

    return compute_outline(pivot, ignored_area).is_internal();
}

void BuildingManager::remove_blindDoors(std::vector<BlockOutline> const& outlines, std::vector<DoorablePosition> & poss)
{
    for (auto const& outline : outlines)
    {
        if (outline.is_internal())
        {
            for (auto const& segment : outline.segments)
            {
                auto const left = DirectionUtil::orthogonalLeft_planeUnitVector(segment.drc);
                for (auto p = segment.begin + left; p != segment.end() + left; p += segment.drc)
                {
                    #if BUILDEXP_VISUALDEBUG
                        BEdeb.new_step("Check internal outline border for blind external doorable positions", 5);
                        BEdeb.highlight_tile(p, Color::Red);
                    #endif
                        
                    poss.erase(std::remove_if(poss.begin(), poss.end(), [p](auto const& dpos) { return p == dpos.pos; }), poss.end());
                }
            }
        }
    }
}

void BuildingManager::internal_build_internalDoor(Vector3i const pos, bool const vertical, TileType const tile_style)
{
    auto const did = m_door_manager.create_door(pos, vertical);
    m_tiles.build_internalDoor(pos.x, pos.y, pos.z, did, tile_style);

    //area.add_internalConnection(other_aid, doors.size() - 1);

    m_tgraphics_mediator.record_borderChange(pos);
}

void BuildingManager::internal_build_externalDoor(Vector3i const pos, bool const vertical, TileType const tile_style, Building & bldg)
{
    auto const did = m_door_manager.create_door(pos, vertical);
    m_tiles.build_externalDoor(pos.x, pos.y, pos.z, did, tile_style);

    bldg.add_externalDoor(pos);

    m_tgraphics_mediator.record_borderChange(pos);
}

void BuildingManager::internal_build_blindDoor(Vector3i const pos, bool const vertical, TileType const tile_style, Building & bld)
{
    auto const did = m_door_manager.create_door(pos, vertical);
    m_tiles.build_externalDoor(pos.x, pos.y, pos.z, did, tile_style);

    bld.add_blindDoor(pos);
    
    m_tgraphics_mediator.record_borderChange(pos);
}


void BuildingManager::reshape_existentRoofs(BuildingId const bid, IntParallelepiped const& vol)
{
    #if BUILDEXP_VISUALDEBUG_ROOF_GENERATION
        BEdeb.new_step("Reshaping existent roofs to make room for a the new area", 2);
        BEdeb.highlight_tilesRect(IntRect{ vol.behind, vol.left, vol.length, vol.width }, vol.down, Color::Blue);
        if(vol.down - 1 > 0) 
        {
            BEdeb.highlight_tilesRect(IntRect{ vol.behind, vol.left, vol.length, vol.width }, vol.down - 1, Color::Blue);
        }
    #endif

    //--- Remove roofs from the nearby/underlying areas.

    // Gather all the roofs that cover this area and the nearby ones
    auto roofs_to_remove = std::unordered_set<RoofId>{};
    for (auto y = vol.left; y <= vol.right(); ++y)							// Check horizontal borders
    {
        auto const& t_top = m_tiles.get_existent(vol.behind, y, vol.down);
        auto const& t_bottom = m_tiles.get_existent(vol.front(), y, vol.down);

        for (auto const rinfo : t_top.roof_infos())
        {
            if(rinfo.bid == bid) { roofs_to_remove.insert(rinfo.roof_id); }
        }

        for (auto const rinfo : t_bottom.roof_infos())
        {
            if(rinfo.bid == bid) { roofs_to_remove.insert(rinfo.roof_id); }
        }
    }
    for (auto x = vol.behind + 1; x <= vol.front() - 1; ++x)				// Check vertical borders (apart the corner tiles, already built above)
    {
        auto const& t_left = m_tiles.get_existent(x, vol.left, vol.down);
        auto const& t_right = m_tiles.get_existent(x, vol.right(), vol.down);
        
        for (auto const rinfo : t_left.roof_infos())
        {
            if(rinfo.bid == bid) { roofs_to_remove.insert(rinfo.roof_id); }
        }

        for (auto const rinfo : t_right.roof_infos())
        {
            if(rinfo.bid == bid) { roofs_to_remove.insert(rinfo.roof_id); }
        }
    }


    // Destroy each of the gathered roofs.
    for (auto const rid : roofs_to_remove)
    {
        auto & r = m_roofs.get(rid);

        // Unbuild the roof from each covered tile
        for (auto const pos : r.roofed_poss)
        {
            m_tiles.unbuild_roof(bid, pos);
        }

        m_roofs.destroy(rid);

        
        // Notify the removal to RoofGraphicsManager
        m_rgraphics_mediator.record_roofRemoval(rid);
    }


    //--- Try to rebuild a roof in the tiles that were the border of the area
    for (auto y = vol.left; y <= vol.right(); ++y)
    {
        auto const x_top    = vol.behind;
        auto const x_bottom = vol.front();

        try_buildRoof(x_top,    y, vol.down);
        try_buildRoof(x_bottom, y, vol.down);
    }
    for (auto x = vol.behind + 1; x <= vol.front() - 1; ++x)
    {
        auto const y_left  = vol.left;
        auto const y_right = vol.right();
        
        try_buildRoof(x, y_left,  vol.down);
        try_buildRoof(x, y_right, vol.down);
    }
}

void BuildingManager::try_buildRoof(int const x, int const y, int const z)
{
    #if BUILDEXP_VISUALDEBUG_ROOF_GENERATION
        BEdeb.new_step("Trying to generate roof starting from this tile", 4);
        BEdeb.highlight_tile({ x, y, z }, Color::Blue);
    #endif

    auto const t = m_tiles.get(x, y, z);

    if (t && t->is_built())
    {
        std::set<BuildingId> roofable_buildings;

        if (t->is_innerArea())
        {
            auto const& info = t->get_innerAreaInfo();
            
            if (!t->is_roofed_for(info.bid()))
            {
                auto const& b = m_buildings.get_or_throw(info.bid());
                auto const& a = b.getOrThrow_area(info.aid());

                if (area_templates.at(a.type()).is_roofable())
                {
                    roofable_buildings.insert(info.bid());
                }
            }
        }
        else if (t->is_border())
        {
            for (auto const info : t->get_borderInfos())
            {
                if (!info.is_empty() && !t->is_roofed_for(info.bid()))
                {
                    auto const& b = m_buildings.get_or_throw(info.bid());
                    auto const& a = b.getOrThrow_area(info.aid());

                    if (area_templates.at(a.type()).is_roofable())
                    {
                        roofable_buildings.insert(info.bid());
                    }
                }
            }
        }
        else
        {
            throw std::runtime_error("A built tile must be either an inner area or a border.");
        }

        
        for (auto const bid : roofable_buildings)
        {
            auto const roofable_poss = RoofAlgorithm::compute_roofablePositions_fromTile(bid, { x, y, z }, m_buildings, m_tiles);

            if(!roofable_poss.empty()) { create_roof(bid, roofable_poss); }
        }
    }
}

void BuildingManager::create_roof(BuildingId const bid, std::unordered_set<Vector3i> const& roofed_poss)
{
    #if DYNAMIC_ASSERTS
        if(roofed_poss.empty()) { throw std::runtime_error("Unexpected argument. There should be at least one roofed position when creating a new roof."); }
    #endif

    auto & [rid, roof] = m_roofs.create();
    

    // Assign the roof id to each roofed tile.
    for (auto const pos : roofed_poss)
    {
        m_tiles.build_roof(bid, pos, rid);
    }

    roof.roofed_poss.insert(roof.roofed_poss.end(), roofed_poss.cbegin(), roofed_poss.cend());


    // Notify the addition to the RoofGraphicsManager, in order to compute the 3D shape of the roof
    m_rgraphics_mediator.record_roofAddition(rid);
}

void BuildingManager::unbuild_building(BuildingId const bid)
{
    auto const pb = m_buildings.weak_get(bid);

    // It is allowed that @bid refers to an expired/deleted building
    if (pb)
    {
        #if BUILDEXP_VISUALDEBUG
            BEdeb.begin_chapter("Building unbuilding");
            BEdeb.focus_onBuilding(*pb);
        #endif
        #if PLAYERMOVEMENT_VISUALDEBUG
            PMdeb.begin_chapter("Building unbuilding."); // Starting a new chapter is necessary to remove impassable tiles from PlayerMovementVisualDebug.
        #endif


        auto & city = m_cities.get_or_throw(pb->cid());
        auto & cblock = m_blocks.get_or_throw(pb->cbid());
        
        for (auto const& [aid, area] : pb->areas_by_ref())
        {
            cblock.decrease_surface(area.volume());

            unbuild_buildingArea(bid, *pb, aid);
        }

        cblock.remove_building(bid);

        //TODO: It's possible that erasing a building the block is split in two and the buildings aren't adjacent anymore. Address this case.
        // If that was the last building of the block, then destroy the block
        if (cblock.empty())
        {
            m_blocks.destroy(pb->cbid());
            city.remove_block(pb->cbid());
                
            // If that was the last block of the city, then destroy the city.
            if (city.empty())
            {
                m_cities.destroy(pb->cid());
            }
        }

        m_buildings.destroy(bid);	//I delete it only here because I need its cid() and cbid() above.


        #if BUILDEXP_VISUALDEBUG
            BEdeb.remove_building(bid);
            BEdeb.end_chapter();
        #endif
        #if PLAYERMOVEMENT_VISUALDEBUG
            PMdeb.end_chapter();
        #endif
    }
}

void BuildingManager::unbuild_buildingArea(BuildingId const bid, Building const& building, BuildingAreaId const aid)
{
    auto const vol = building.getOrThrow_area(aid).volume();

    building.getOrThrow_area(aid).type();

    //--- Unbuild doors if present
    {
        // Check for horizontal doors
        for (int y = vol.left; y <= vol.right(); ++y)
        {
            int x_top = vol.behind;
            int x_bottom = vol.front();

            try_unbuild_door(x_top, y, vol.down);
            try_unbuild_door(x_bottom, y, vol.down);
        }

        // Check for vertical doors (apart the corner tiles, already unbuilt above)
        for (int x = vol.behind + 1; x <= vol.front() - 1; ++x)
        {
            int y_left = vol.left;
            int y_right = vol.right();

            try_unbuild_door(x, y_left, vol.down);
            try_unbuild_door(x, y_right, vol.down);
        }

        // Find the blind doors in the courtyards around this area
        auto occluded_externalDoors = std::unordered_set<Vector3i>{};
        gather_occluded_externalDoors(vol, {}, occluded_externalDoors);

        #if BUILDEXP_VISUALDEBUG_DOORS
            BEdeb.new_step("Blind doors that could be not blind anymore after the removal of this area.", 4);
            BEdeb.highlight_tiles(occluded_externalDoors.cbegin(), occluded_externalDoors.cend(), Color::Red);
        #endif
            
        // Check if the doors would remain occluded/blind also when this area will be removed
        for (auto const d : occluded_externalDoors)
        {
            if (!is_externalBlindDoor(d, {bid, aid})) { try_unbuild_door(d.x, d.y, d.z); }
        }
    }


    // Unbuild horizontal border
    for (int y = vol.left; y <= vol.right(); ++y)
    {
        int x_top = vol.behind;
        int x_bottom = vol.front();

        m_tiles.unbuild_border(x_top, y, vol.down, bid, aid);
        m_tiles.unbuild_border(x_bottom, y, vol.down, bid, aid);
    }

    // Unbuild vertical border (apart the corner tiles, already unbuilt above)
    for (int x = vol.behind + 1; x <= vol.front() - 1; ++x)
    {
        int y_left = vol.left;
        int y_right = vol.right();

        m_tiles.unbuild_border(x, y_left, vol.down, bid, aid);
        m_tiles.unbuild_border(x, y_right, vol.down, bid, aid);
    }

    // Unbuild inner area
    for (int y = vol.left + 1; y <= vol.right() - 1; ++y)
    {
        for (int x = vol.behind + 1; x <= vol.front() - 1; ++x)
        {
            m_tiles.unbuild_innerArea(x, y, vol.down, bid, aid);
        }
    }
    
    //--- Generate the roof
    if (sim_settings.map.generate_roofs)
    {
        // Cut and reshape possibly present roofs
        reshape_existentRoofs(bid, vol);
    }

    //--- Mention the changes to the preparation managers
    m_tgraphics_mediator.record_areaChange(vol);

}

void BuildingManager::try_build_door(Vector3i const pos)
{
    auto const t = m_tiles.get(pos);

    if (t && t->is_border() && !t->is_door())
    {
        auto const info = t->get_borderInfos()[0];

        auto door_position = m_tiles.compute_borderDoorType(pos);

        auto const vertical = is_verticalDoor(pos);
        auto & bldg = m_buildings.get_or_throw(info.bid());
        auto const tile_style = area_templates.at( bldg.getOrThrow_area(info.aid()).type() ).tile_style();

        switch (door_position.type)
        {
            case BorderDoorType::None:
                std::cout << "\nCan't build a door here." << std::endl;
                break;

            case BorderDoorType::Internal:
                internal_build_internalDoor(pos, vertical, tile_style);
                break;

            case BorderDoorType::BuildingsLinker:
                std::cout << "\nCan't build a door connecting two different buildings." << std::endl;
                break;

            case BorderDoorType::External:
            {
                if (is_externalBlindDoor(pos))
                {
                    internal_build_blindDoor(pos, vertical, tile_style, bldg);
                }
                else
                {
                    internal_build_externalDoor(pos, vertical, tile_style, bldg);
                }

                break;
            }

            default:
                throw std::runtime_error("Unexpected case.");
                break;
        }
    }
}

void BuildingManager::try_unbuild_door(int const x, int const y, int const z)
{
    auto const& t = m_tiles.get_existent(x, y, z);

    if (t.is_door())
    {
        // Remove the external door from the building
        if (t.is_externalDoor())
        {
            auto const info = t.get_borderInfos()[0];	//an external door can be built only if the border is not shared. 

            if (is_externalBlindDoor(Vector3i{ x, y, z })) 
            {
                #if BUILDEXP_VISUALDEBUG_DOORS
                    BEdeb.new_step("Removing blind door.", 5);
                    BEdeb.highlight_tile({ x, y, z }, Color::Red);
                #endif

                m_buildings.get_or_throw(info.bid()).remove_blindDoor({ x, y, z });
            }
            else
            {
                #if BUILDEXP_VISUALDEBUG_DOORS
                    BEdeb.new_step("Removing external door.", 5);
                    BEdeb.highlight_tile({ x, y, z }, Color::Red);
                #endif

                m_buildings.get_or_throw(info.bid()).remove_externalDoor({ x, y, z });
            }
        }

        m_door_manager.destroy_door(t.furniture_id());
        m_tiles.unbuild_door(x, y, z);

        m_tgraphics_mediator.record_borderChange(x, y, z);
    }
}



#pragma warning(disable: 4100)
void BuildingManager::visualDebug_highlightOrderedCityBlocks(Vector2f const city_center, std::map<float, CityBlock const*> const& ordered_blocks) const
{
    #if BUILDEXP_VISUALDEBUG
        BEdeb.focus_onPosition(city_center);
        auto debug_counter = std::uint8_t{ 255 };
        for (auto const [distance, block] : ordered_blocks)
        {
            visualDebug_highlightCityBlock(*block, Color{ 0, 0, 255, debug_counter });
            if (debug_counter >= 10) { debug_counter -= 10; }
        }
    #endif

    #pragma warning(default: 4100)
}

#pragma warning(disable: 4100)
void BuildingManager::visualDebug_highlightCityBlock(CityBlock const& cblock, Color const color) const
{
    #if BUILDEXP_VISUALDEBUG
        for (auto const bid : cblock.buildings())
        {
            auto const& b = m_buildings.get_or_throw(bid);

            visualDebug_highlightBuilding(b, color);
        }
    #endif

    #pragma warning(default: 4100)
}

#pragma warning(disable: 4100)
void BuildingManager::visualDebug_highlightBuilding(Building const& b, Color const color) const
{
    #if BUILDEXP_VISUALDEBUG
        for (auto const& [aid, a] : b.areas_by_ref())
        {
            auto const vol = a.volume();
            BEdeb.highlight_tilesRect({vol.behind, vol.left, vol.length, vol.width}, vol.down, color);
        }
    #endif

    #pragma warning(default: 4100)
}

#pragma warning(disable: 4100)
void BuildingManager::visualDebug_replaceableAreasStep(std::vector<BuildingAreaCompleteId> const& replaceable_areas) const
{
    #if BUILDEXP_VISUALDEBUG
        if (!replaceable_areas.empty())
        {
            BEdeb.new_step("Replaceable areas in the block", 2);
            for (auto const repl_acid : replaceable_areas)
            {
                auto const vol = get_area(repl_acid).volume();
                BEdeb.highlight_tilesRect({vol.behind, vol.left, vol.length, vol.width}, vol.down, Color::Mint);
            }
        }
        else
        {
            BEdeb.new_step("No replaceable areas", 2);
        }
    #endif
        
    #pragma warning(default: 4100)
}

#pragma warning(disable: 4100)
void BuildingManager::visualDebug_buildablePositionsStep(Vector2i const area_dims, std::vector<BuildablePosition> const& buildable_positions) const
{
    #if BUILDEXP_VISUALDEBUG
        auto constexpr step_depth = 3;
        if (visualDebug_runtime_maxRecordableDepth >= step_depth)
        {
            std::ostringstream oss;	oss << "Buildable positions in block -  area dims: " << area_dims;
            BEdeb.new_step(oss.str(), step_depth);
            std::vector<Vector3i> debug_bposs;
            auto lambda = [](BuildablePosition const& bp) { return bp.pos; };
            std::transform(buildable_positions.cbegin(), buildable_positions.cend(), std::back_inserter(debug_bposs), lambda);
            BEdeb.highlight_tiles(debug_bposs.cbegin(), debug_bposs.cend(), Color::Red, true);
        }
    #endif
        
    #pragma warning(default: 4100)
}

#pragma warning(disable: 4100)
void BuildingManager::visualDebug_doorablePositionsStep(std::string const& title, std::vector<DoorablePosition> const& doorable_poss) const
{
    #if BUILDEXP_VISUALDEBUG_DOORS
        auto constexpr step_depth = 4;
        if (visualDebug_runtime_maxRecordableDepth >= step_depth)
        {
            BEdeb.new_step(title, step_depth);
            auto debug_doorablePoss = std::vector<Vector3i>{};
            auto lambda = [](DoorablePosition const& dp) { return dp.pos; };
            std::transform(doorable_poss.cbegin(), doorable_poss.cend(), std::back_inserter(debug_doorablePoss), lambda);
            BEdeb.highlight_tiles(debug_doorablePoss.cbegin(), debug_doorablePoss.cend(), Color::Mint);
        }
    #endif
        
    #pragma warning(default: 4100)
}



} //namespace tgm