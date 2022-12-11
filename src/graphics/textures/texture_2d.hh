#ifndef GM_TEXTURE_2D_HH
#define GM_TEXTURE_2D_HH


#include <exception>
#include <iostream>

#include "settings/graphics_settings.hh"


namespace tgm
{



class Texture2D
{
    public:
        Texture2D(char const* lowDef_path, char const* highDef_path, char const* ultraHD_path);
        Texture2D(Texture2D const&) = delete;
        Texture2D & operator=(Texture2D const&) = delete;
        // TODO: Add a destructor that frees the texture automatically

        
        auto data() const -> unsigned char *
        {
            if(m_freed)
                throw std::runtime_error("Cannot get data of a freed texture.");

            return m_data;
        }


        void free();
        

        auto width() const noexcept -> int { return m_width; }
        auto height() const noexcept -> int { return m_height; }


    private:
        bool m_freed = false;

        unsigned char *m_data;
        int m_width = 0;
        int m_height = 0;
};


#if FREE_ASSETS

    #if !GSET_TILESET_TEXARRAY
        inline Texture2D default_texture_tileset{"_DATA/free_assets/free_tileset.png", 
                                                 "media/free_assets/free_tileset.png", 
                                                 "media/free_assets/free_tileset.png" };
    #endif

    inline Texture2D default_texture_dynamics{"media/free_assets/free_dynamics.png", 
                                              "media/free_assets/free_dynamics.png", 
                                              "media/free_assets/free_dynamics.png" };
#else

    #if !GSET_TILESET_TEXARRAY
        inline Texture2D default_texture_tileset{"media/textures/default_texture_tileset_ld.png", 
                                                 "media/textures/default_texture_tileset_hd.png", 
                                                 "media/textures/default_texture_tileset_uhd.png" };
    #endif

    inline Texture2D default_texture_dynamics{"media/textures/default_texture_dynamics_ld.png", 
                                              "media/textures/default_texture_dynamics_hd.png", 
                                              "media/textures/default_texture_dynamics_uhd.png" };
#endif


//TODO: I should use different resolution textures for the roof_texture, but the problem is that using a lower resolution textures
//		for the roofs (even with lower screen resolutions) produce an ugly effect. That's in part because 
//		of the poor quality of the present texture and in part because the roofVertex_wySliding_ratio 
//		is applied after the computation of the texture.
//		Thus the texture (already being low in quality) is also stretched along the map x-axis, producing an 
//		even lower-quality output. Maybe I should use 4 different textures (pre-stretched) for each roof side..
inline Texture2D roof_texture{ "media/textures/roof_texture_ld.png", "media/textures/roof_texture_hd.png", "media/textures/roof_texture_uhd.png" };;



} //namespace tgm


#endif //GM_TEXTURE_2D_HH
