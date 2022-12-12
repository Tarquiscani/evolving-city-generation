#include "trail_system.hh"


#include <iomanip>

#include "std_extensions/hash_functions.hh"
#include "system/clock.hh"
#include "utilities.hh"

#include "debug/visual/player_movement_stream.hh"


namespace tgm
{



//TODO: 14: Rifattorizza in modo da sostituire Direction con Versor2i
//TODO: 14: Aggiungi namespace tgm

////
//	For a given position, it finds if there is any obstacle in that position and if that is the case then return the nearest position to that obstacle.
//
//  @return: (1) A float indicating how much backward should the dest_square go to be adjacent to an impassable tile (if the tile is not impassable return 0.f).
//			 (2) A bool indicating whether the tile is a closed door.
////
static auto check_frontPos(FloatRect const& dest_square, int const z_pos, Direction const move_drc, Vector2i const pos, TileSet const& tiles)
    -> std::pair<float, bool>
{

    std::pair<float, bool> ret;
    auto & [units_backward, is_closed_door] = ret;
    units_backward = 0.f;
    is_closed_door = false;


    auto const tile = tiles.get(pos.x, pos.y, z_pos);

    #if PLAYERMOVEMENT_VISUALDEBUG
        std::ostringstream oss;
        oss << "Checking if the tile " << pos << " is impassable...";
        PMdeb.new_step(oss.str());
        PMdeb.highlight_tile(pos, z_pos, Color(255, 192, 203, 60), true);
    #endif

    bool const impassable = !tile || tile->is_impassable();			//the tile is outside the map or impassable
    is_closed_door = impassable && tile && tile->is_door();		//the tile is a closed door

    // if impassable, then the feet_rect will be brought to the nearest pixel towards that tile
    if (impassable)
    {
        FloatRect const tile_square{ GSet::tiles_to_units(pos.x) - 0.0001f,		//The impassable tile is enlarged by a 0.0001f because otherwise the feet_rect would be
                                     GSet::tiles_to_units(pos.y) - 0.0001f,		//adjusted to an integer (e.g. 17.0000), but that number is part of the next tile and that
                                     GSet::upt + 0.0001f,						//would produce a bug. Instead the adjustment can be done up to a number near to the integer
                                     GSet::upt + 0.0001f };						//but lower than it (e.g. 16.9999).

        #if PLAYERMOVEMENT_DEBUGLOG
            PMlog << Logger::nltb << "Impassable tile: "
                  << Logger::nltb << "dest_square:     " << dest_square
                  << Logger::nltb << "impassable tile: " << tile_square << std::endl;
        #endif

        switch (move_drc)
        {
            case Direction::N:
                units_backward = tile_square.bottom() - dest_square.top;
                break;
            case Direction::NE:
                units_backward = std::min(tile_square.bottom() - dest_square.top, dest_square.right() - tile_square.left);
                break;
            case Direction::E:
                units_backward = dest_square.right() - tile_square.left;
                break;
            case Direction::SE:
                units_backward = std::min(dest_square.bottom() - tile_square.top, dest_square.right() - tile_square.left);
                break;
            case Direction::S:
                units_backward = dest_square.bottom() - tile_square.top;
                break;
            case Direction::SW:
                units_backward = std::min(dest_square.bottom() - tile_square.top, tile_square.right() - dest_square.left);
                break;
            case Direction::W:
                units_backward = tile_square.right() - dest_square.left;
                break;
            case Direction::NW:
                units_backward = std::min(tile_square.bottom() - dest_square.top, tile_square.right() - dest_square.left);
                break;
        }

        if (units_backward <= 0.f)
        {
            units_backward = 0.f;
        }
        else
        {
            #if PLAYERMOVEMENT_DEBUGLOG
                PMlog << Logger::nltb << "new_square would penetrate the impassable tile of " << units_backward << " units"
                      << Logger::nltb << "units_backward:" << units_backward << std::endl;
            #endif
        }
    }

    return ret;
}


////
//	Check, substep by substep, if the trail contains impassable tiles or non-openable doors.
//
//	@return: If there's no impassable tile return @dest_square. Otherwise return the nearest square to the first impassable tile touched by the trail.
////
static auto check_trail(FloatRect const& dest_square, int const z_pos, Direction const move_drc, std::vector<std::vector<Vector2i>> const& trail, TileSet const& tiles,
                        std::unordered_set<Vector2i> ignored_tiles, float const debug_velocity)
    -> std::pair< FloatRect, std::vector<DoorId> >
{
    std::pair< FloatRect, std::vector<DoorId> > ret;
    auto & [adjusted_destSquare, doors_to_open] = ret;
    adjusted_destSquare = dest_square;


    // For each "front" of the trail
    for (auto const& front : trail)
    {
        auto max_unitsBackward = 0.f;
        //If there is a non-door tile that block the movement before a door can be reached, then that door mustn't be opened.
        auto max_unitsBackward_forNonDoorTile = 0.f; 
        std::vector<std::pair<float, Vector2i>> closedDoors_in_front; //closed door and the associated units backwards

        // For each tile in the "front"
        for (auto const pos : front)
        {
            // Skip if this is an ignored tile
            if (ignored_tiles.find(pos) != ignored_tiles.cend())
                continue;


            auto const [units_back, closed_door] = check_frontPos(adjusted_destSquare, z_pos, move_drc, pos, tiles);
            if (units_back != 0.f && units_back >= max_unitsBackward)
            {
                max_unitsBackward = units_back;
                if (!closed_door) //can't open a door farther than this position, since this impassable tile would block the movement
                    max_unitsBackward_forNonDoorTile = units_back;			


                #if PLAYERMOVEMENT_DEBUGLOG
                    auto debug1_adjDestSquare = DirectionUtil::compute_newRect(dest_square, DirectionUtil::invert(move_drc), units_back);

                    PMlog << Logger::nltb << "Impassable tile - It'd require " << units_back << "units backward. Related adjusted_destSquare:" << debug1_adjDestSquare << std::endl;

                    if (units_back > debug_velocity)
                        PMlog << bug_bigNotification() << std::endl;
                #endif

                #if PLAYERMOVEMENT_VISUALDEBUG
                    auto debug2_adjDestSquare = DirectionUtil::compute_newRect(dest_square, DirectionUtil::invert(move_drc), units_back);
                    std::ostringstream oss;	oss << "Impassable tile - " << units_back << " units backward. adjusted_destSquare:";
                    PMdeb.new_step(oss.str());
                    PMdeb.highlight_rect(debug2_adjDestSquare, z_pos, Color(0, 255, 255));
                #endif
            }

            if (closed_door)
                closedDoors_in_front.push_back({ units_back, pos });
        }

        if (max_unitsBackward > 0.f)
        {
            adjusted_destSquare = DirectionUtil::compute_newRect(dest_square, DirectionUtil::invert(move_drc), max_unitsBackward);
            
            #if PLAYERMOVEMENT_VISUALDEBUG
                std::vector<Vector2i> debug_doorsToOpenPositions; 	
            #endif

            for (auto const& [units_back, d_pos] : closedDoors_in_front)
            {
                // Check that the door is actually reached and that there are no impassable tile between the mobile and the door.
                if (units_back >= max_unitsBackward_forNonDoorTile + GSet::mu)		//"+ GSet::mu" because if an impassable tile and a door are at the same 
                {																	//distance, then I don't want the door to be opened.
                    doors_to_open.push_back(tiles.get_existent(d_pos, z_pos).furniture_id());

                    #if PLAYERMOVEMENT_VISUALDEBUG
                        debug_doorsToOpenPositions.push_back(d_pos);
                    #endif
                }
            }

            #if PLAYERMOVEMENT_DEBUGLOG
                PMlog << Logger::nltb << "max_unitsBackward: " << max_unitsBackward << "  -  Definitive adjusted_destSquare: " << adjusted_destSquare << std::endl;
            #endif

            #if PLAYERMOVEMENT_VISUALDEBUG
                PMdeb.new_step("Impassable front - Definitive adjusted_destSquare:");
                PMdeb.highlight_rect(adjusted_destSquare, z_pos, Color(0, 255, 255));

                if (!debug_doorsToOpenPositions.empty())
                {
                    PMdeb.new_step("Doors to open:");
                    PMdeb.highlight_tiles(debug_doorsToOpenPositions.cbegin(), debug_doorsToOpenPositions.cend(), z_pos, Color(128, 0, 0));
                }
            #endif

            break;
        }
    }


    #if PLAYERMOVEMENT_DEBUGLOG
        PMlog << Logger::remt;
    #endif

    return ret;
}

////
//	Moves the front tracing the whole trail in @trail.
////
static void trace_trail(std::unordered_set<Vector2i> const& beg_front, Vector2i const unit_vector,
                        IntRect const dest_tilesRect, int const traversed_tiles, std::vector<std::vector<Vector2i>>& trail, int const debug_zPos)
{
    for (int i = 1; i < traversed_tiles; ++i)
    {
        auto & shifted_front = trail.emplace_back();
        for (auto const pos : beg_front)
            shifted_front.push_back(pos + i * unit_vector);
    }


    // Special behavior for the last front movement. It must be prevented that the trail surpass the dest_tilesRect.

    auto & last_front = trail.emplace_back();

    auto const rhX = unit_vector.x == 1 ? dest_tilesRect.bottom() : dest_tilesRect.top,
               rhY = unit_vector.y == 1 ? dest_tilesRect.right()  : dest_tilesRect.left;


    for (auto const pos : beg_front)
    {
        auto const new_pos = pos + traversed_tiles * unit_vector;
        //both the sides are multiplied by unit_vector, so that in case of a negative direction it's as if the comparison became ">="
        if (   new_pos.x * unit_vector.x <= rhX * unit_vector.x
            && new_pos.y * unit_vector.y <= rhY * unit_vector.y)
        {
            last_front.push_back(new_pos);
        }
    }


    #if PLAYERMOVEMENT_DEBUGLOG
        PMlog << Logger::nltb << "trail positions: " << trail << std::endl;
    #endif
    #if PLAYERMOVEMENT_VISUALDEBUG
        PMdeb.new_step("Trail");
        std::vector<Vector2i> debug_trail;
        for (auto const& front : trail)
            debug_trail.insert(debug_trail.cend(), front.cbegin(), front.cend());
        PMdeb.highlight_tiles(debug_trail.cbegin(), debug_trail.cend(), debug_zPos, Color(75, 75, 125, 125));
    #endif
}

////
//  Computes the set of tiles that moving will generate the entire trail. Also computes how many times the front have to move
//	to trace the whole trail.
//	@uv: The unit vector associated to @drc.
////
static auto compute_trailFrontAndShiftings(FloatRect const& orig_square, IntRect const& orig_tilesRect, IntRect const& dest_tilesRect, 
                                           Direction const drc, Vector2i const uv, float const velocity, int const debug_zPos)
    -> std::pair<std::unordered_set<Vector2i>, int>
{
    std::pair<std::unordered_set<Vector2i>, int> ret; //declaring it here pave the way to NRVO
    auto & [front, traversed_tiles] = ret;

    // Distances of the borders of the orig_square from the respective borders of the orig_tilesRect (in units)
    auto const d_top    = orig_square.top - GSet::tiles_to_units(orig_tilesRect.top) + 0.0001f,			//Here d_top and d_left require an additional 0.0001f because otherwise
               d_right  = GSet::tiles_to_units(orig_tilesRect.right() + 1) - orig_square.right(),		//the projection can't reach the next tile. On the contrary d_right and
               d_bottom = GSet::tiles_to_units(orig_tilesRect.bottom() + 1) - orig_square.bottom(),		//d_bottom are directly computed using the next tile so they don't need
               d_left   = orig_square.left - GSet::tiles_to_units(orig_tilesRect.left) + 0.0001f;		//any addition.

    // Vertex of the orig_square that will be projected towards @drc and will generate the border tile of the horizontal front.
    Vector2f origHor_units;
    // How many units origHor_units has to be moved towards @drc
    auto d_hor = 0.f;

    // Vertex of the orig_square that will be projected towards @drc and will generate the border tile of the vertical front.
    Vector2f origVert_units;
    auto d_vert = 0.f;

    switch (drc)
    {
        case Direction::N:
            origHor_units = { orig_square.top, orig_square.right() };
            d_hor = d_top;
            origVert_units = { /*any*/ orig_square.bottom(), orig_square.left };
            d_vert = 0.f;
            break;

        case Direction::E:
            origHor_units = { orig_square.bottom(), /*any*/ orig_square.left };
            d_hor = 0.f;
            origVert_units = { orig_square.top, orig_square.right() };
            d_vert = d_right;
            break;

        case Direction::S:
            origHor_units = { orig_square.bottom(), orig_square.left };
            d_hor = d_bottom;
            origVert_units = { /*any*/ orig_square.top, orig_square.right() };
            d_vert = 0.f;
            break;

        case Direction::W:
            origHor_units = { orig_square.top, /*any*/ orig_square.right() };
            d_hor = 0.f;
            origVert_units = { orig_square.bottom(), orig_square.left };
            d_vert = d_left;
            break;

        case Direction::NE:
            origHor_units = { orig_square.top, orig_square.left };
            d_hor = d_top;
            origVert_units = { orig_square.bottom(), orig_square.right() };
            d_vert = d_right;
            break;

        case Direction::SE:
            origHor_units = { orig_square.bottom(), orig_square.left };
            d_hor = d_bottom;
            origVert_units = { orig_square.top, orig_square.right() };
            d_vert = d_right;
            break;

        case Direction::SW:
            origHor_units = { orig_square.bottom(), orig_square.right() };
            d_hor = d_bottom;
            origVert_units = { orig_square.top, orig_square.left };
            d_vert = d_left;
            break;

        case Direction::NW:
            origHor_units = { orig_square.top, orig_square.right() };
            d_hor = d_top;
            origVert_units = { orig_square.bottom(), orig_square.left };
            d_vert = d_left;
            break;

        case Direction::none:
        default:
            throw std::runtime_error("Unexpected direction.");
            break;
    }

    #if PLAYERMOVEMENT_DEBUGLOG
        PMlog << Logger::nltb << "unit vector: " << uv
              << Logger::nltb << "d_top:    " << d_top
              << Logger::nltb << "d_right:  " << d_right
              << Logger::nltb << "d_bottom: " << d_bottom
              << Logger::nltb << "d_left:   " << d_left << std::endl;
    #endif



    // The projection can't be greater than velocity, otherwise the front would shift forward also if orig_square didn't change tile. 
    d_hor = std::min(velocity, d_hor);		// Note that velocity it's the same for both diagonal and straight movements. So it's ok
    d_vert = std::min(velocity, d_vert);	// to compare an y-axis distance with a possibly diagonal distance.



    //Projection of origHor_units along the movement direction.
    auto const hor_units = origHor_units + d_hor * Vector2f{ uv };
    //Tile containing the original vertex
    auto const orig_hor = Util::units_to_tiles(origHor_units);
    //Tile containing the projected vertex. It's the starting tile of the horizontal front 
    auto const hor = Util::units_to_tiles(hor_units);		



    auto const vert_units = origVert_units + d_vert * Vector2f{ uv };
    auto const orig_vert = Util::units_to_tiles(origVert_units);
    //Tile containing the projected vertex. It's the starting tile of the vertical front 
    auto const vert = Util::units_to_tiles(vert_units);



    #if PLAYERMOVEMENT_DEBUGLOG
        PMlog << Logger::nltb << "orig_vert: " << orig_vert
              << Logger::nltb << "d_vert:    " << d_vert
              << Logger::nltb << "vert:	     " << vert
              << Logger::nltb << "orig_hor:  " << orig_hor
              << Logger::nltb << "d_hor:     " << d_hor
              << Logger::nltb << "hor:	     " << hor << std::endl;
    #endif

    // Coefficient used to indicate the versus of the front tracing. Negative k means: trace from here backward. Positive k means: trace from here forward.
    // For E and W directions, since they have a zeroed uv.x, k is computed in another way.
    auto const k = uv.x != 0 ? uv.x : uv.y;
    // The vertical front is traced only if the border tile is different from the tile of the original vertex used to do the projection.
    if (vert != orig_vert)
    {
        for (int x = vert.x; x * k <= hor.x * k; x += k)
        {
            front.insert({ x, vert.y });
        }
    }

    // Same observations as above
    auto const h = uv.y != 0 ? uv.y : uv.x;
    if (hor != orig_hor)
    {
        for (int y = hor.y; y * h <= vert.y * h; y += h)
        {
            front.insert({ hor.x, y });
        }
    }


    switch (drc)
    {
        case Direction::N:
            traversed_tiles = hor.x - dest_tilesRect.top;
            break;

        case Direction::E:
            traversed_tiles = dest_tilesRect.right() - vert.y;
            break;

        case Direction::S:
            traversed_tiles = dest_tilesRect.bottom() - hor.x;
            break;

        case Direction::W:
            traversed_tiles = vert.y - dest_tilesRect.left;
            break;

        case Direction::NE:
            traversed_tiles = std::max(hor.x - dest_tilesRect.top, dest_tilesRect.right() - vert.y);
            break;

        case Direction::SE:
            traversed_tiles = std::max(dest_tilesRect.bottom() - hor.x, dest_tilesRect.right() - vert.y);
            break;

        case Direction::SW:
            traversed_tiles = std::max(dest_tilesRect.bottom() - hor.x, vert.y - dest_tilesRect.left);
            break;

        case Direction::NW:
            traversed_tiles = std::max(hor.x - dest_tilesRect.top, vert.y - dest_tilesRect.left);
            break;

        case Direction::none:
        default:
            throw std::runtime_error("Unexpected direction.");
            break;
    }

    #if PLAYERMOVEMENT_DEBUGLOG
        PMlog << Logger::nltb << "traversed_tiles: " << traversed_tiles << std::endl;
    #endif
    #if PLAYERMOVEMENT_VISUALDEBUG
        PMdeb.new_step("New front hor");
        PMdeb.highlight_polygon({ origHor_units, hor_units }, debug_zPos, Color::Red);
        PMdeb.highlight_tile(hor, debug_zPos, Color(75, 75, 75, 125));
        PMdeb.new_step("New front vert");
        PMdeb.highlight_polygon({ origVert_units, vert_units }, debug_zPos, Color::Red);
        PMdeb.highlight_tile(vert, debug_zPos, Color(75, 75, 75, 125));
        PMdeb.new_step("New front");
        PMdeb.highlight_tiles(front.cbegin(), front.cend(), debug_zPos, Color(75, 75, 75, 125));
        std::ostringstream oss; oss << "Traversed tiles: " << traversed_tiles;
        PMdeb.new_step(oss.str());
    #endif

    return ret;
}
    
////
//	@feet_square: (in units -- map reference system).
//
//	@return: The smallest square of tiles that contains the @feet_square (in tiles -- map reference system).
////
static auto compute_tileSquareCircumscribedToFeetSquare(FloatRect const feet_square) noexcept -> IntRect
{
    return IntRect{	GSet::units_to_tiles(feet_square.top ), 
                    GSet::units_to_tiles(feet_square.left),
                    GSet::units_to_tiles(feet_square.bottom()) + 1 - GSet::units_to_tiles(feet_square.top ),
                    GSet::units_to_tiles(feet_square.right() ) + 1 - GSet::units_to_tiles(feet_square.left)		};
}


static void debug_internalCompareMoveAlgorithms(FloatRect const orig_square, int const z_pos, Direction const direction, float const velocity, TileSet const& tiles)
{
    static auto test_id = 0;

    Clock clock;


    for (int i = 0; i < 10000; ++i)
        TrailSystem::compute_nearestPosition(orig_square, z_pos, direction, velocity, tiles);

    auto t1 = clock.restart();


    for (int i = 0; i < 10000; ++i)
        TrailSystem::debug_brute_computeNearestPosition(orig_square, z_pos, direction, velocity, tiles);

    auto t2 = clock.getElapsedTime();


    Logger lgr{ g_log };
    lgr << Logger::addt 
            << Logger::nltb << "test " << test_id++ << ": "
            << Logger::addt
                << Logger::nltb	<< "orig_square: " << orig_square
                << Logger::nltb	<< "direction:   " << direction
                << Logger::nltb	<< "velocity:    " << velocity
                << Logger::nltb	<< "trail algorithm:          " << std::setw(10) << t1.asMicroseconds() << "ms (~" << std::setw(10) << t1.asSeconds() << "s)"
                << Logger::nltb	<< "brute movement algorithm: " << std::setw(10) << t2.asMicroseconds() << "ms (~" << std::setw(10) << t2.asSeconds() << "s)";
}

namespace TrailSystem
{
    //TODO: Thoroughly test the trail system (especially after floatization)
    auto compute_nearestPosition(FloatRect const orig_square, int const z_pos, Direction const drc, float const velocity, TileSet const& tiles) 
        -> std::pair< FloatRect, std::vector<DoorId> >
    {
        auto const dest_square = DirectionUtil::compute_newRect(orig_square, drc, velocity);

        #if PLAYERMOVEMENT_DEBUGLOG
            PMlog << "\n\n"
                << Logger::add_title("compute_nearestPosition")
                << Logger::addt
                << Logger::nltb << "player velocity: " << velocity
                << Logger::nltb << "orig_square:     " << orig_square
                << Logger::nltb << "dest_square:     " << dest_square << std::endl;
        #endif

        //Rectangle of tiles surrounding the orig_square
        auto const orig_tilesRect = compute_tileSquareCircumscribedToFeetSquare(orig_square);
        //Rectangle of tiles surrounding the dest_square
        auto const dest_tilesRect = compute_tileSquareCircumscribedToFeetSquare(dest_square);

        #if PLAYERMOVEMENT_DEBUGLOG
            PMlog << Logger::nltb << "orig_tilesRect: " << orig_tilesRect
                  << Logger::nltb << "dest_tilesRect: " << dest_tilesRect << std::endl;
        #endif
        #if PLAYERMOVEMENT_VISUALDEBUG
            PMdeb.begin_chapter("Movement begin.", { orig_tilesRect.get_NWvertex().x, orig_tilesRect.get_NWvertex().y, z_pos });
            PMdeb.focus_movement(orig_tilesRect, dest_tilesRect);
            PMdeb.highlight_tilesRect(orig_tilesRect, z_pos, Color(127, 224, 255), true);
            PMdeb.highlight_tilesRect(dest_tilesRect, z_pos, Color(0, 255, 0, 125), true);
            PMdeb.highlight_rect(orig_square, z_pos, Color(255, 153, 255), true);
            PMdeb.highlight_rect(dest_square, z_pos, Color(0, 255, 255), true);
        #endif



        std::vector<std::vector<Vector2i>> trail;
        auto const unit_vector = DirectionUtil::planeUnitVector(drc);

        auto const [front, traversed_tiles] = compute_trailFrontAndShiftings(orig_square, orig_tilesRect, dest_tilesRect, drc, unit_vector, velocity, z_pos);


        trail.emplace_back(front.cbegin(), front.cend());
        trace_trail(front, unit_vector, dest_tilesRect, traversed_tiles, trail, z_pos);


        #if !PLAYERMOVEMENT_DEBUGLOG && !PLAYERMOVEMENT_VISUALDEBUG
            return check_trail(dest_square, z_pos, drc, trail, tiles, {}, velocity);
        #else
            auto ret = check_trail(dest_square, z_pos, drc, trail, tiles, {}, velocity);

            #if PLAYERMOVEMENT_DEBUGLOG
                PMlog << "\n\n";
            #endif
            #if PLAYERMOVEMENT_VISUALDEBUG
                PMdeb.end_chapter();
            #endif

            return ret;
        #endif
    }



    auto debug_brute_computeNearestPosition(FloatRect const orig_square, int const z_pos, Direction const drc, float const velocity, TileSet const& tiles) -> FloatRect
    {
        #if PLAYERMOVEMENT_DEBUGLOG
            PMlog << "\n"
                  << Logger::add_title("debug_bruteComputeNearestPosition")
                  << Logger::addt
                  << Logger::nltb << "orig_square: " << orig_square
                  << Logger::nltb << "velocity: " << velocity << std::endl;
        #endif

        auto previous_square = orig_square;


        auto mu_to_cover = static_cast<int>(velocity / GSet::mu);

        for (int s = 0; s < mu_to_cover; ++s)
        {
            auto new_square = DirectionUtil::compute_newRect(previous_square, drc, GSet::mu);
            auto tiles_rect = compute_tileSquareCircumscribedToFeetSquare(new_square);

            for(int x = tiles_rect.top; x <= tiles_rect.bottom(); ++x)
            {
                for (int y = tiles_rect.left; y <= tiles_rect.right(); ++y)
                {
                    auto const tile = tiles.get(x, y, z_pos);
                
                    //if tile is occupied or outside the map, then the feet_square will be brought to the nearest pixel towards that tile
                    if (!tile || tile->is_impassable())
                    {
                        #if PLAYERMOVEMENT_DEBUGLOG
                            PMlog << Logger::remt;
                        #endif
                        return previous_square;
                    }
                }
            }

            previous_square = new_square;
        }
    
        #if PLAYERMOVEMENT_DEBUGLOG
            PMlog << Logger::nltb << "dest_square: " << previous_square
                  << Logger::remt << "\n";
        #endif

        return previous_square;
    }

    #pragma warning(disable: 4702)
    void debug_compareMoveAlgorithms(float const x, float const y, int const z_floor, Direction const drc, TileSet const& tiles)
    {
        #if DEBUGLOG || VISUALDEBUG || TESTLOG
            throw std::runtime_error("Strongly deactivate DEBUGLOG, VISUALDEBUG and TESTLOG macros before perfoming a speed comparison.");
        #endif

        g_log << "Comparison: trail algorithm VS brute movement algorithm:";

        auto const small_size  = 0.32f * GSet::upt;
        auto const medium_size = 1.60f * GSet::upt;
        auto const large_size  = 8.76f * GSet::upt;
        FloatRect const small_fSquare{  x - small_size  / 2.f, y - small_size  / 2.f, small_size,  small_size };
        FloatRect const medium_fSquare{ x - medium_size / 2.f, y - medium_size / 2.f, medium_size, medium_size };
        FloatRect const large_fSquare{  x - large_size  / 2.f, y - large_size  / 2.f, large_size,  large_size };

        auto slow_v   =   0.05f * GSet::upt;
        auto medium_v =	  0.33f * GSet::upt;
        auto fast_v   =  15.63f * GSet::upt;


        debug_internalCompareMoveAlgorithms(small_fSquare,  z_floor, drc, slow_v,   tiles);
        debug_internalCompareMoveAlgorithms(medium_fSquare, z_floor, drc, slow_v,   tiles);
        debug_internalCompareMoveAlgorithms(large_fSquare,  z_floor, drc, slow_v,   tiles);

        debug_internalCompareMoveAlgorithms(small_fSquare,  z_floor, drc, medium_v, tiles);
        debug_internalCompareMoveAlgorithms(medium_fSquare, z_floor, drc, medium_v, tiles);
        debug_internalCompareMoveAlgorithms(large_fSquare,  z_floor, drc, medium_v, tiles);

        debug_internalCompareMoveAlgorithms(small_fSquare,  z_floor, drc, fast_v,   tiles);
        debug_internalCompareMoveAlgorithms(medium_fSquare, z_floor, drc, fast_v,   tiles);
        debug_internalCompareMoveAlgorithms(large_fSquare,  z_floor, drc, fast_v,   tiles);


        g_log << std::endl;

        #pragma warning(default: 4702)
    }


    auto compute_tilesFromVolume(FloatRect const volume_base, int const z_pos) -> std::vector<Vector3i>
    {
        std::vector<Vector3i> ret;

        auto const tiles_rect = compute_tileSquareCircumscribedToFeetSquare(volume_base);

        for (int x = tiles_rect.top; x <= tiles_rect.bottom(); ++x)
        {
            for (int y = tiles_rect.left; y <= tiles_rect.right(); ++y)
            {
                ret.push_back({x, y, z_pos});
            }
        }

        return ret;
    }
} // namespace TrailSystem



} // namespace tgm