#ifndef GM_PLAYER_MOVEMENT_VISUAL_DEBUG_H
#define GM_PLAYER_MOVEMENT_VISUAL_DEBUG_H


#include "settings/debug/playermovement_visualdebug.hh"
#if PLAYERMOVEMENT_VISUALDEBUG


#include <unordered_set>

#include "std_extensions/hash_functions.hh"

#include "debug/visual/base/visual_debug.hh"


namespace tgm
{



class PlayerMovementVisualDebugState
{
    public:
        PlayerMovementVisualDebugState() = default;

        PlayerMovementVisualDebugState(const std::unordered_set<Vector3i>& impassable_tiles_) :
            impassable_tiles(impassable_tiles_) {	}

    private:
        std::unordered_set<Vector3i> impassable_tiles;
};

class PlayerMovementVisualDebugChange
{
    public:
        PlayerMovementVisualDebugChange(int change_id_) noexcept :
            change_id(change_id_) { }

        auto get_removedImpassableTiles() const noexcept -> const std::unordered_set<Vector3i>& { return removed_impassableTiles; }
        auto get_addedImpassableTiles() const noexcept -> const std::unordered_set<Vector3i>& { return added_impassableTiles; }

        void record_impassableTileRemoval(const Vector3i& pos)
        {
            bool removed = removed_impassableTiles.count(pos) == 1;
            bool added = added_impassableTiles.count(pos) == 1;

            if (removed && added) //error 
            {
                throw std::runtime_error("PlayerMovementVisualDebugChange: The same tile was recorded both as added and removed.");
            }
            else if (removed && !added) //error 
            {
                throw std::runtime_error("PlayerMovementVisualDebugChange: The tile was already removed.");
            }
            else if (!removed && added) //remove the recorded addition
            {
                added_impassableTiles.erase(pos);
            }
            else if (!removed && !added) //record the removal
            {
                removed_impassableTiles.insert(pos);
            }
        }

        void record_impassableTileAddition(const Vector3i& pos)
        {
            bool removed = removed_impassableTiles.count(pos) == 1;
            bool added = added_impassableTiles.count(pos) == 1;

            if (removed && added) //error 
            {
                throw std::runtime_error("PlayerMovementVisualDebugChange: The same tile was recorded both as added and removed.");
            }
            else if (removed && !added) //remove the recorded removal 
            {
                removed_impassableTiles.erase(pos);
            }
            else if (!removed && added) //error
            {
                throw std::runtime_error("PlayerMovementVisualDebugChange: The tile was already added.");
            }
            else if (!removed && !added) //record the addition
            {
                added_impassableTiles.insert(pos);
            }
        }

    private:
        int change_id = 0;
        std::unordered_set<Vector3i> removed_impassableTiles;
        std::unordered_set<Vector3i> added_impassableTiles;

    friend auto operator<<(Logger& lgr, const PlayerMovementVisualDebugChange& pmc) -> Logger&;
};

class PlayerMovementVisualDebug final : public VisualDebug
{
    public:
        void start(TileSet const& map_tiles);

        void focus_movement(const IntRect& orig_tilesSquare, const IntRect& dest_tilesSquare);

        void add_impassableTile(const Vector3i& pos);
        void remove_impassableTile(const Vector3i& pos);

    private:
        PlayerMovementVisualDebugState custom_originalState;
        std::vector<PlayerMovementVisualDebugChange> custom_changes;

        std::unordered_set<Vector3i> impassable_tiles;
        //TODO: std::unordered_set<IntRect> impassable_mobiles;

        void copy_mapImpassableTiles(TileSet const& map_tiles);

        
        virtual bool custom_has_been_activated() const noexcept override { return visualDebug_runtime_openWindowForPlayerMovement; }
        virtual void custom_deactivate() const noexcept override { visualDebug_runtime_openWindowForPlayerMovement = false; }
        virtual void init_chapterAlias() override { m_chapter_alias = "movement"; }

        virtual void custom_stop() override;

        virtual auto window_title() const noexcept -> std::string override { return "Player Movement Visual Debug"; }

        virtual void init_newChange(int const current_st) override;

        virtual void custom_goToPreviousStep(int const change_id) override;
        virtual void custom_goToNextStep(int const change_id) override;

        virtual void custom_pushVertices(DebugVertices & vertices) const override;
};



} // namespace tgm


#endif //PLAYERMOVEMENT_VISUALDEBUG


#endif //GM_PLAYER_MOVEMENT_VISUAL_DEBUG_H
