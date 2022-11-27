#include "tutorial_panel.hh"


#include "settings/graphics_settings.hh"
#include "systems/tutorial/demo_tutorial.hh"


namespace tgm
{



void TutorialPanel::generate_layout(Vector2i const framebuffer_size)
{
	if (m_open)
	{
		if (!demo_tutorial().is_over())
		{
			ImGui::Begin("Tutorial Panel", &m_open, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::SetWindowFontScale(GSet::imgui_scale());

			auto const curr_tutorial_step = demo_tutorial().get_current_step();
			assert(curr_tutorial_step.has_value());

			ImGui::Text("%s", curr_tutorial_step->message.c_str());
		

			ImGui::End();
		}
	}
}



} // namespace tgm