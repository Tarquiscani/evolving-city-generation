#include "tutorial_panel.hh"


#include <glad/glad.h>

#include "settings/graphics_settings.hh"
#include "systems/tutorial/demo_tutorial.hh"


namespace tgm
{



void centered_text(std::string const& text) 
{
    float win_width = ImGui::GetWindowContentRegionMax().x;
    float text_width = ImGui::CalcTextSize(text.c_str()).x;

    // calculate the indentation that centers the text on one line, relative
    // to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
    float text_indentation = (win_width - text_width) * 0.5f;

    // if text is too long to be drawn on one line, `text_indentation` can
    // become too small or even negative, so we check a minimum indentation
    float min_indentation = 20.0f;
    if (text_indentation <= min_indentation) {
        text_indentation = min_indentation;
    }

    ImGui::SameLine(text_indentation);
    ImGui::PushTextWrapPos(win_width - text_indentation);
    ImGui::TextWrapped(text.c_str());
    ImGui::PopTextWrapPos();
}


void TutorialPanel::generate_layout(Vector2i const framebuffer_size)
{
	if (m_open)
	{
		if (!demo_tutorial().is_over())
		{
			auto const width_min = 400.f * GSet::imgui_scale();
			auto const height_min = 350.f * GSet::imgui_scale();

			ImGui::SetNextWindowPos({ framebuffer_size.x * 0.5f, framebuffer_size.y * 0.5f }, ImGuiCond_Once, { 0.5f,0.5f });
			ImGui::SetNextWindowSizeConstraints({ width_min, height_min }, { framebuffer_size.x * 1.f, framebuffer_size.y * 1.f });

			ImGui::Begin("Tutorial Panel", &m_open/*, ImGuiWindowFlags_AlwaysAutoResize*/);
			ImGui::SetWindowFontScale(GSet::imgui_scale());

			auto const curr_tutorial_step = demo_tutorial().get_current_step();
			assert(curr_tutorial_step.has_value());
			
			
            // Main window
            bool clear = ImGui::Button("Previous");
            ImGui::SameLine();
            bool copy = ImGui::Button("Next");
            
            auto const& img_name = curr_tutorial_step->image_name;
            if (!img_name.empty())
            {
                auto const img = load_image(img_name);
                ImGui::Separator();
                ImGui::Image((void*)(intptr_t)img.id, ImVec2(img.width, img.height), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            }
            ImGui::Separator();
            ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            ImGui::NewLine();
            centered_text(curr_tutorial_step->message);
            ImGui::PopStyleVar();

            ImGui::EndChild();


			ImGui::End();
		}
	}
}



} // namespace tgm