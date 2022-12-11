#ifndef GM_PLAYER_MANAGER_HH
#define GM_PLAYER_MANAGER_HH


#include "characters/mobile.h"
#include "map/map_forward_decl.hh"
#include "mediators/queues/player_ev.hh"
#include "mediators/queues/mobile_ev.hh"
#include "system/vector2.hh"

#include "settings/debug/debug_settings.hh"


namespace tgm
{



class PlayerManager
{
    public:
        PlayerManager(PlayerEventQueues & player_events, MobileBody & player_body) :
            m_player_events(player_events), m_player_body(player_body) {}
        

        void update();

        ////
        //	@return: Tile occupied by the center of the player body volume (in tiles - map reference system).
        ////
        auto debug_getPlayerPosition_inTiles() const -> Vector3i
        {
            auto center = m_player_body.volume().center();

            return { GSet::units_to_tiles(center.x), 
                     GSet::units_to_tiles(center.y),
                     GSet::units_to_tiles(center.z) };
        }

        ////
        //	@return: Center of the player body volume (in units - map reference system).
        ////
        auto debug_getPlayerPosition_inUnits() const -> Vector3f
        {
            return m_player_body.volume().center();
        }

        ////
        //	@return: Player body volume (in units - map reference system).
        ////
        auto debug_getPlayerVolume() const -> FloatParallelepiped
        {
            return m_player_body.volume();
        }

        auto debug_getPlayerVelocity() const { return m_player_body.velocity(); }


        ////
        //	@feet_pos, @feet_dim: Map reference system (in units)
        ////
        void debug_setPlayerFeetSquare(Vector2f const feet_pos, float const feet_dim)
        {
            m_player_body.set_feetPosition(feet_pos.x, feet_pos.y);
            m_player_body.debug_setFeetDim(feet_dim);
        }
        void debug_setPlayerVelocity(float const v)
        {
            m_player_body.set_velocity(v);
        }

    private:
        PlayerEventQueues & m_player_events;
        MobileBody & m_player_body;
};


} // namespace tgm



#endif //GM_PLAYER_MANAGER_HH