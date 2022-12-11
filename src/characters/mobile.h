#ifndef GM_MOBILE_H
#define GM_MOBILE_H


#include "map/direction.h"
#include "map/map_forward_decl.hh"
#include "settings/graphics_settings.hh"
#include "system/parallelepiped.hh"


namespace tgm
{



enum class MobileStyle
{
    None,
    Builder,
    FatRich,
    Fireman,
};

class MobileBody
{
    public:
        ////
        //	@feet_dim, @starting_feetPos: (in units - map r.s.)
        //	@z_pos: (in tiles - map r.s.)
        ////
        MobileBody(float const feet_dim, Vector2f const starting_feetPos, int const z_pos, MobileStyle const style) :
            m_volume(starting_feetPos.x, starting_feetPos.y, GSet::tiles_to_units(z_pos), feet_dim, feet_dim, GSet::upt /*TODO: Explicitly ask an height in constructor*/),
            m_style(style)
        { }

        ////
        //	@return: (in units - map reference system)
        ////
        auto volume() const noexcept -> FloatParallelepiped { return m_volume; }

        ////
        //	@return: The quad rapresenting the feet of the player (in units - map reference system)
        ////
        auto feet_square() const noexcept -> FloatRect { return m_volume.base(); }
        ////
        //	@top, left: (in units - map reference system)
        ////
        auto set_feetPosition(float const top, float const left) { m_volume.behind = top; m_volume.left = left; }

        auto z_floor() const noexcept -> int { return GSet::units_to_tiles(m_volume.down); }
        void set_zFloor(int const z_pos) { m_volume.down = GSet::tiles_to_units(z_pos); }

        auto get_moveDirection() const noexcept -> Direction { return m_move_drc; }
        void set_moveDirection(Direction const drc) noexcept { m_move_drc = drc; }
        void add_move_direction(Direction const drc);

        //TODO: Make diagonal velocity different from orthogonal velocity
        auto velocity() const noexcept -> float { return DirectionUtil::is_diagonal(m_move_drc) ? m_velocity : m_velocity; }


        void set_velocity(float const v) noexcept { m_velocity = v; }

        auto style() const noexcept -> MobileStyle { return m_style; }

        auto sprite_id() const -> SpriteId 
        { 
            if (m_sprite_id == 0)
                throw std::runtime_error("The mobile has no associated sprite.");

            return m_sprite_id; 
        }
        void set_spriteId(SpriteId spid) noexcept { m_sprite_id = spid; }


        void debug_setFeetDim(float const dim) 
        {
            m_volume.length = dim; 
            m_volume.width = dim; 
        }

    private:
        FloatParallelepiped m_volume;
        Direction m_move_drc = Direction::none;
        //Vector2f versor_drc;
        float m_velocity = GSet::upt / 50.f;
        bool movement_paused = false;

        MobileStyle m_style = MobileStyle::None;
        SpriteId m_sprite_id = 0;
};



} // namespace tgm


#endif //GM_MOBILE_H