#include "player_movement_visual_debug.h"

#if PLAYERMOVEMENT_VISUALDEBUG


#include "map/gamemap.h"


namespace tgm
{



auto operator<<(Logger& lgr, const PlayerMovementVisualDebugChange& pmc) -> Logger&
{
	lgr << Logger::addt
		<< Logger::nltb << "change_id: " << pmc.change_id
		<< Logger::nltb << "removed_impassableTiles: " << pmc.removed_impassableTiles
		<< Logger::nltb << "added_impassableTiles: " << pmc.added_impassableTiles
		<< Logger::remt;

	return lgr;
}

void PlayerMovementVisualDebug::start(TileSet const& map_tiles)
{
	copy_mapImpassableTiles(map_tiles);

	custom_originalState = PlayerMovementVisualDebugState(impassable_tiles);


	base_start(map_tiles.length(), map_tiles.width(), map_tiles.height());
}

void PlayerMovementVisualDebug::copy_mapImpassableTiles(TileSet const& map_tiles)
{
	for (int x = 0; x < map_tiles.length(); ++x)
		for (int y = 0; y < map_tiles.width(); ++y)
			for (int z = 0; z < map_tiles.height(); ++z)
			{
				auto t = map_tiles.get(x, y, z);
				if (t && t->is_impassable())
					impassable_tiles.insert(t->get_coordinates());
			}
}

void PlayerMovementVisualDebug::custom_stop()
{
	custom_changes.clear();
	impassable_tiles.clear();
}

void PlayerMovementVisualDebug::init_newChange(int const current_st)
{
	custom_changes.emplace_back(current_st);
}

void PlayerMovementVisualDebug::custom_goToPreviousStep(int const change_id)
{
	auto ch = custom_changes[change_id];

	#if VISUALDEBUG_DEBUGLOG
		VDlog << Logger::nltb << "Custom change #" << change_id << ": " << ch;
	#endif

	for (const auto& pos : ch.get_addedImpassableTiles())
	{
		if (impassable_tiles.erase(pos) < 1)
			throw std::runtime_error("PlayerMovementVisualDebug: Cannot remove an impassable_tile when going to the previous step.");
	}

	for (const auto& pos : ch.get_removedImpassableTiles())
	{
		const auto[it, success] = impassable_tiles.insert(pos);
		if (!success)
			throw std::runtime_error("PlayerMovementVisualDebug: Cannot add an impassable_tile when going to the previous step.");
	}
}

void PlayerMovementVisualDebug::custom_goToNextStep(int const change_id)
{
	auto ch = custom_changes[change_id];

	#if VISUALDEBUG_DEBUGLOG
		VDlog << Logger::nltb << "Custom change #" << change_id << ": " << ch;
	#endif

	for (const auto& pos : ch.get_removedImpassableTiles())
	{
		if (impassable_tiles.erase(pos) < 1)
			throw std::runtime_error("PlayerMovementVisualDebug: Cannot remove an impassable_tile when going to the next step.");
	}

	for (const auto& pos : ch.get_addedImpassableTiles())
	{
		const auto[it, success] = impassable_tiles.insert(pos);
		if (!success)
			throw std::runtime_error("PlayerMovementVisualDebug: Cannot add an impassable_tile when going to the next step.");
	}
}

void PlayerMovementVisualDebug::focus_movement(const IntRect & orig_tilesSquare, const IntRect & dest_tilesSquare)
{
	int top = std::min(orig_tilesSquare.top, dest_tilesSquare.top),
		left = std::min(orig_tilesSquare.left, dest_tilesSquare.left),
		length = std::max(orig_tilesSquare.bottom(), dest_tilesSquare.bottom()) - top + 1,
		width = std::max(orig_tilesSquare.right(), dest_tilesSquare.right()) - left + 1;

	// Enlarge the frame area of 4 tiles to better show what is happening during the movement
	top -= 4;
	left -= 4;
	length += 8;
	width += 8;

	//TODO: This was to make frame not overstep rhw map, but it's no longer used. Decide whether to remove or reuse it.
	//if (top < 0) //shortens both top and length of the same amount
	//{			 
	//	length += top; 
	//	top = 0;
	//}

	//if (left < 0)
	//{
	//	width += left;
	//	left = 0;
	//}

	//if (top + length > map_length())
	//	length = map_length() - top;

	//if (left + width > map_width())
	//	width = map_width() - left;


	#if VISUALDEBUG_DEBUGLOG
		VDlog << Logger::nltb << "Focus Movement";

		#if VISUALDEBUG_DEBUGLOG_FRAMEINFOS
			int original_top = std::min(orig_tilesSquare.top, dest_tilesSquare.top),
				original_left = std::min(orig_tilesSquare.left, dest_tilesSquare.left),
				original_length = std::max(orig_tilesSquare.bottom(), dest_tilesSquare.bottom()) - top + 1,
				original_width = std::max(orig_tilesSquare.right(), dest_tilesSquare.right()) - left + 1;

			VDlog << Logger::addt
				  << Logger::nltb << "top_shifting:" << top - original_top << "     left_shifting:" << left - original_left
								  << "     bottom_shifting:" << length - original_length << "     right_shifting:" << width - original_width;
		 #endif
	#endif


	VisualDebug::change_frame(IntParallelepiped(top, left, 0, length, width, map_height()));


	#if VISUALDEBUG_DEBUGLOG & VISUALDEBUG_DEBUGLOG_FRAMEINFOS
		VDlog << Logger::remt;
	#endif
}

void PlayerMovementVisualDebug::add_impassableTile(const Vector3i & pos)
{
	validate_inChapterAction();

	auto [it, success] = impassable_tiles.insert(pos);

	if (!success)
		throw std::runtime_error("The impassable tile was already added.");

	custom_changes.back().record_impassableTileAddition(pos);
}

void PlayerMovementVisualDebug::remove_impassableTile(const Vector3i & pos)
{
	validate_inChapterAction();

	if (impassable_tiles.erase(pos) != 1)
		throw std::runtime_error("There was no such an impassable tile.");

	custom_changes.back().record_impassableTileRemoval(pos);
}

void PlayerMovementVisualDebug::custom_pushVertices(DebugVertices & vertices) const
{
	for (auto const& pos : impassable_tiles)
	{
		if (pos.z == cursor_pos().z && frame().contains(pos))
		{
			vertices.push_tile(pos.x, pos.y, 1.f, Color::Black);
		}
	}
}



} // namespace tgm


#endif //PLAYERMOVEMENT_VISUALDEBUG