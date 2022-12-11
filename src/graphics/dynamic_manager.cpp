#include "graphics/dynamic_manager.hh"


#include "graphics/graphics_utilities.hh"

namespace tgm
{



auto DynamicManager::create(FloatParallelepiped const volume, DynamicSubimage const& subimage, bool const round_to_nearest_pixel) -> SpriteId
{
    auto const wvolume = GUtil::compute_dynamicSprite(volume, subimage, m_camera.pixel_dim(), Direction::none, round_to_nearest_pixel);

    return m_dynamic_vertices.create_sprite(wvolume, subimage.tex_subimage());
}
        
void DynamicManager::modify(SpriteId const id, FloatParallelepiped const volume, DynamicSubimage const& subimage, Direction const drc, bool const round_to_nearest_pixel)
{
    auto const wvolume = GUtil::compute_dynamicSprite(volume, subimage, m_camera.pixel_dim(), drc, round_to_nearest_pixel);
    
    //TODO: When there will be other sprites moving each frame, then this global variable won't reflect the player srpite position anymore.
    //		Find an alternative.
    GSet::TEST_playerSpritePosition = Vector3f{ wvolume.left, wvolume.front, wvolume.up() };

    m_dynamic_vertices.set_sprite(id, wvolume, subimage.tex_subimage());
}

void DynamicManager::destroy(SpriteId const id)
{
    m_dynamic_vertices.destroy(id);
}



}