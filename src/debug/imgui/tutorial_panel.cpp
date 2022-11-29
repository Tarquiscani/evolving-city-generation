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
		if (!m_demo_tutorial.is_over())
		{
			auto const width_min = 400.f * GSet::imgui_scale();
			auto const height_min = 350.f * GSet::imgui_scale();

			ImGui::SetNextWindowPos({ framebuffer_size.x * 0.5f, framebuffer_size.y * 0.5f }, ImGuiCond_Once, { 0.5f,0.5f });
			ImGui::SetNextWindowSizeConstraints({ width_min, height_min }, { framebuffer_size.x * 1.f, framebuffer_size.y * 1.f });

			ImGui::Begin("Tutorial Panel", &m_open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::SetWindowFontScale(GSet::imgui_scale());

			auto const curr_tutorial_step = m_demo_tutorial.get_current_step();
			assert(curr_tutorial_step.has_value());
			
			
            // Main window
            bool previous_pressed = ImGui::Button("Previous");
            ImGui::SameLine();
            centered_text(curr_tutorial_step->title);
            if (!curr_tutorial_step->can_be_skipped)
            {
                ImGui::BeginDisabled();
            }
            auto const next_button_width = 50.f * GSet::imgui_scale();
            ImGui::SameLine(ImGui::GetWindowWidth() - next_button_width);
            bool next_pressed = ImGui::Button("Next");
            if (!curr_tutorial_step->can_be_skipped)
            {
                ImGui::EndDisabled();
            }
            ImGui::Separator();

            if (previous_pressed)
            {
                Tutorial::add_event<TutorialGoBackEv>(m_demo_tutorial.name());
            }

            if (next_pressed)
            {
                Tutorial::add_event<TutorialGoAheadEv>(m_demo_tutorial.name());
            }
            
            auto const& img_name = curr_tutorial_step->image_name;
            if (!img_name.empty())
            {
                auto const img = load_image(img_name);
                ImGui::Image((void*)(intptr_t)img.id, ImVec2(img.width, img.height), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            }

            auto const min_text_height = 100.f * GSet::imgui_scale();
            ImGui::BeginChild("scrolling", ImVec2(0, min_text_height), false, ImGuiWindowFlags_HorizontalScrollbar);

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