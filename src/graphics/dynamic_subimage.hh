#ifndef GM_DYNAMIC_SUBIMAGE_HH
#define GM_DYNAMIC_SUBIMAGE_HH


#include "graphics/gimp_square.hh"
#include "settings/graphics_settings.hh"
#include "texture_subimage.hh"


namespace tgm
{



enum class GraphicLayer
{
    Mats,
    Volumes
};


struct DynamicSubimage
{
    public:
        DynamicSubimage(float const GIMP_left, float const GIMP_top, Texture2D const& texture) :
            DynamicSubimage(GIMP_left, GIMP_top, GraphicsSettings::pptxu(), GraphicsSettings::pptxu(), GraphicLayer::Volumes, texture) {}
        
        DynamicSubimage(float const GIMP_left, float const GIMP_top, float const GIMP_width, float const GIMP_height, Texture2D const& texture) :
            DynamicSubimage(GIMP_left, GIMP_top, GIMP_width, GIMP_height, GraphicLayer::Volumes, texture) {}

        DynamicSubimage(float const GIMP_left, float const GIMP_top, float const GIMP_width, float const GIMP_height, GraphicLayer const layer, Texture2D const& texture) :
            m_width(GIMP_width), m_height(GIMP_height), m_layer(layer), m_tex_subimage(GIMP_left, GIMP_top, GIMP_width, GIMP_height, texture) {}



        DynamicSubimage(GIMPSquare const gimp, Texture2D const& texture) :
            DynamicSubimage(gimp, GraphicLayer::Volumes, texture) {}

        DynamicSubimage(GIMPSquare const gimp, GraphicLayer const layer, Texture2D const& texture) :
            m_width(gimp.width), m_height(gimp.height), m_layer(layer), m_tex_subimage(gimp.left, gimp.top, gimp.width, gimp.height, texture) {}



        auto width() const noexcept -> float { return m_width; }
        auto height() const noexcept -> float { return m_height; }
        auto layer() const noexcept -> GraphicLayer { return m_layer; }
        auto tex_subimage() const noexcept -> TextureSubimage const& { return m_tex_subimage; }

    private:
        float m_width = 0.f;
        float m_height = 0.f;
        GraphicLayer m_layer = GraphicLayer::Volumes;
        TextureSubimage m_tex_subimage;


    friend inline auto operator<<(Logger & lgr, DynamicSubimage const ds) -> Logger &
    {
        lgr << "DynamicSubimage {"
            << Logger::addt
            << Logger::nltb << "width: " << ds.m_width
            << Logger::nltb << "height: " << ds.m_height
            << Logger::nltb << "tex_subimage: " << ds.m_tex_subimage
            << Logger::remt
            << Logger::nltb << " }";

        return lgr;
    }
};



} // namespace tgm


#endif //GM_DYNAMIC_SUBIMAGE_HH