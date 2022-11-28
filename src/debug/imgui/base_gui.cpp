#include "base_gui.hh"


#include "graphics/textures/texture_2d.hh"


namespace tgm
{


auto BaseGui::load_image(std::string const& img_name) -> GuiImage
{
    assert(!img_name.empty());

    auto img = GuiImage{};
    if (m_images.find(img_name) == m_images.end())
    {
        img = load_image(("media/images/" + img_name + "_ld.png").c_str(),
                         ("media/images/" + img_name + "_hd.png").c_str(),
                         ("media/images/" + img_name + "_uhd.png").c_str());
        m_images.emplace(img_name, img);
    }
    else
    {
        img = m_images[img_name];
    }

    return img;
}


auto BaseGui::load_image(char const* low_def_path, char const* hd_path, char const* ultra_hd_path) -> GuiImage
{
    auto demo_tutorial_image = Texture2D{ low_def_path, hd_path, ultra_hd_path };

    auto image_id = GLuint{ 0 };

    // Create a OpenGL texture identifier
    glGenTextures(1, &image_id);
    glBindTexture(GL_TEXTURE_2D, image_id);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, demo_tutorial_image.width(), demo_tutorial_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, demo_tutorial_image.data());
    demo_tutorial_image.free();


    return { image_id, demo_tutorial_image.width(), demo_tutorial_image.height() };
}



} // namespace tgm