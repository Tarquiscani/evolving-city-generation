#ifndef GM_MOBILE_SUBIMAGE_SET_HH
#define GM_MOBILE_SUBIMAGE_SET_HH


#include <array>

#include "dynamic_subimage.hh"
#include "gimp_square.hh"
#include "map/direction.h"


namespace tgm 
{



class MobileSubimageSet
{
    public:
        MobileSubimageSet(GIMPSquare const gimp, Texture2D const& texture);

        auto width() const noexcept -> float { return m_width; }
        auto height() const noexcept -> float { return m_height; }

        auto pick_subimage(Direction const drc) const -> DynamicSubimage const&;

    private:
        float m_width = 0.f;
        float m_height = 0.f;
        std::array<DynamicSubimage, 4> m_subimages;  //N, E, S, W
};



} // namespace tgm


#endif //GM_MOBILE_SUBIMAGE_SET_HH