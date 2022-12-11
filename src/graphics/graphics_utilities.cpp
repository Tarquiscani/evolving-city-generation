#include "graphics_utilities.hh"


namespace tgm
{



namespace GraphicsUtilities
{

    auto compute_dynamicSprite(FloatParallelepiped const volume, DynamicSubimage const& subimage, float const pixel_dim, Direction const drc, bool const round_to_nearest_pixel) -> WorldParallelepiped
    {

        auto const subimage_width  = GSet::texels_to_units(std::abs(subimage.width()) );  //std::abs() takes into account negative width
        auto const subimage_height = GSet::texels_to_units(std::abs(subimage.height()));


        auto const floor_worldZ = ::floorf(volume.down) * GSet::floorsSpacing_ratio; //TODO: 11: It should be "std::floorf", but g++ doesn't allow it.


        // World space z-axis postions of the sprite.
        auto base_worldZ = 0.f;
        auto top_worldZ = 0.f;

        //each kind of sprite has a different world z layer, so that they compete in the correct way along the z-axis
        switch (subimage.layer())
        {
            case GraphicLayer::Mats:
                base_worldZ = floor_worldZ + GSet::matsLayer_raising;
                top_worldZ = floor_worldZ + GSet::matsLayer_raising;
                break;

            case GraphicLayer::Volumes:
                base_worldZ = floor_worldZ + GSet::volumesLayer_raising;
                top_worldZ = floor_worldZ + GSet::volumesLayer_raising + subimage_height;
                break;
        }


        // World y sliding
        auto const wy_sliding = volume.down * GSet::wySliding_ratio();


        // Compute world space position of the the sprite
        auto const unrounded_wleft = volume.left - (subimage_width - volume.width) / 2,
                   unrounded_wbottom = -volume.front() + wy_sliding;								//x-axis of world space goes in the opposite direction compared to the x-axis of map



        // Round position of the sprite to the nearest movement unit (i.e. to the nearest pixel, a part with UltraHD resolution where the 
        // movement unit corresponds to 2 pixels).
        // If the movement is diagonal, the rounding is made along a special "diagonal reference system", so that it's possible 
        // to round both x-coordinate and y-coordinate together. This techinique is especially important when the velocity is a fraction 
        // of a pixel. Otherwise, if the classic rounding were made both for diagonal and for straight movements, then during diagonal 
        // movements the image would be arbitrarily rounded the first time along x-axis, the second time along y-axis and so on. The 
        // graphical effect is ugly, it produces a sort of flickering, especially at lower screen resolution.

        //TODO: When changing from a diagonal movement to a straight movement and (and vice versa), the sprite abruptly change position.
        //		That's because of the different kind of rounding between the two methods. It would be nice if this change could be less
        //		evident.

        auto wleft = 0.f,
             wbottom = 0.f;

        #if GSET_ROUND_TO_NEAREST_PIXEL
        if (round_to_nearest_pixel)
        {
            // From "world reference system" to "screen reference system"
            auto const unroundedWleft_screenPixels   = GSet::units_to_screenPixels(unrounded_wleft, pixel_dim);
            auto const unroundedWbottom_screenPixels = GSet::units_to_screenPixels(unrounded_wbottom, pixel_dim);	

            auto wleft_screenPixels  = 0.f;
            auto wbottom_screenPixels = 0.f;
            

            // When moving along the diagonal direction the rounding is different depending on which half-pixel triangle the point lies.
            if (DirectionUtil::is_diagonal(drc))
            {
                auto wleft_whole = 0.f;
                auto wbottom_whole = 0.f;
                auto const wleft_decimal = std::modf(unroundedWleft_screenPixels, &wleft_whole);
                auto const wbottom_decimal = std::modf(unroundedWbottom_screenPixels, &wbottom_whole);

                if (drc == Direction::NE || drc == Direction::SW)
                {
                    if (wleft_decimal < wbottom_decimal)	 // Upper half-pixel
                    {
                        wleft_screenPixels  = wleft_whole;
                        wbottom_screenPixels = wbottom_whole + 1.f;
                    }
                    else									 // Lower half-pixel
                    {
                        wleft_screenPixels  = wleft_whole;
                        wbottom_screenPixels = wbottom_whole;
                    }
                }
                else if (drc == Direction::SE || drc == Direction::NW)
                {
                    if (wleft_decimal + wbottom_decimal > 1) // Upper half-square
                    {
                        wleft_screenPixels  = wleft_whole + 1.f;
                        wbottom_screenPixels = wbottom_whole;
                    }
                    else									 // Lower half-square
                    {
                        wleft_screenPixels  = wleft_whole;
                        wbottom_screenPixels = wbottom_whole;
                    }
                }
                else
                {
                    throw std::runtime_error("Unexpected direction.");
                }
            }
            else        
            {
                wleft_screenPixels = std::floor(unroundedWleft_screenPixels);
                wbottom_screenPixels = std::floor(unroundedWbottom_screenPixels);
            }
            
            // From "screen reference system" to "world reference system"
            wleft   = GSet::screenPixels_to_units(wleft_screenPixels, pixel_dim);
            wbottom = GSet::screenPixels_to_units(wbottom_screenPixels, pixel_dim);
        }
        else
        {
            wleft = unrounded_wleft;
            wbottom = unrounded_wbottom;
        }
        #else
            wleft = unrounded_wleft;
            wbottom = unrounded_wbottom;
        #endif



        return { wleft, wbottom, base_worldZ, subimage_width, subimage_height, top_worldZ - base_worldZ };
    }

}



} //namespace tgm
