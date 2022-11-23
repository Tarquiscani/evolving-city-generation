#include "main_menu_gui.hh"


#include <regex>

#include <imgui.h>

#include "settings/game_state_settings.hh"
#include "utilities/filesystem_utilities.hh"


namespace tgm
{



void MainMenuGui::generate_layout(Vector2i const framebuffer_size)
{
	if (m_open)
	{
		auto const width = 300.f * GSet::imgui_scale();
		auto const height = 300.f * GSet::imgui_scale();
			
		ImGui::SetNextWindowPos(ImVec2{ (framebuffer_size.x - width) / 2.f, (framebuffer_size.y - height) / 2.f });
		ImGui::SetNextWindowSize({ width, height });


		ImGui::Begin("Main Menu", &m_open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		ImGui::SetWindowFontScale(GSet::imgui_scale());
			
		auto const button_dim = ImVec2{ 50.f * GSet::imgui_scale(), 30.f * GSet::imgui_scale() };
		auto const offset = (width - button_dim.x) / 2.f;
			
		ImGui::SetCursorPosY(70.f * GSet::imgui_scale());
		ImGui::NewLine(); ImGui::SameLine(offset); ; 
            
        if (ImGui::Button("Save", button_dim)) { ImGui::OpenPopup("SaveModal"); }
        generate_saveModal_layout();

		ImGui::NewLine(); 
		ImGui::NewLine(); ImGui::SameLine(offset); 
			
		if (ImGui::Button("Load", button_dim)) { ImGui::OpenPopup("LoadModal"); }
		generate_loadModal_layout();
			
		ImGui::NewLine(); 
		ImGui::NewLine(); ImGui::SameLine(offset); 

		if (ImGui::Button("Options", button_dim)) { ImGui::OpenPopup("OptionModal"); }
		generate_optionModal_layout();

		ImGui::NewLine(); 
		ImGui::NewLine(); ImGui::SameLine(offset); 

		if (ImGui::Button("Exit", button_dim)) { m_gui_events.push<ExitEv>(); }


		ImGui::End();
	}
}

void MainMenuGui::generate_saveModal_layout()
{
    if (ImGui::BeginPopupModal("SaveModal", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
    {
		ImGui::SetWindowFontScale(GSet::imgui_scale());

		static auto constexpr max_inputSize = 64;
        static char filename_input[max_inputSize] = "";

        // Child window: list of filenames
        {
            auto existent_saves = FsUtil::get_directoryFilenames(GStateSet::saves_folder);

			ImGui::BeginChild("Child1", {childModal_width, childModal_height}, true, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::SetWindowFontScale(GSet::imgui_scale());
                
            for (auto const& fn : existent_saves)
            {
				// Remove the extension
				auto const target = std::regex{ "(" + GStateSet::saves_ext + ")" };
				auto const fn_trimmed = std::regex_replace(fn, target, "");

				if (ImGui::Selectable(fn_trimmed.c_str())) 
				{  
					auto const str_end = max_inputSize < fn_trimmed.size() ? max_inputSize - 1 : fn_trimmed.size();

					fn_trimmed.copy(filename_input, str_end);
					filename_input[str_end] = '\0';

					for (auto i = str_end + 1; i < max_inputSize; ++i) { filename_input[i] = 0; }
				}                    
            }

            ImGui::EndChild();
        }
			
        ImGui::InputText("Filename", filename_input, max_inputSize);
			
		if (ImGui::Button("Save")) 
		{ 
			// Remove the extension and special characters.
			auto const fn = std::string{ filename_input };
			auto const target = std::regex{ "(" + GStateSet::saves_ext + ")|[^a-zA-Z0-9]" };
			auto const fn_trimmed = std::regex_replace(fn, target, "");

			m_gui_events.push<SaveWorldEv>(fn_trimmed);
			ImGui::CloseCurrentPopup();
			for (auto i = 0; i < max_inputSize; ++i) { filename_input[i] = 0; }		//erase the selected string
		}
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 100.f * GSet::imgui_scale());
		if (ImGui::Button("Cancel")) 
		{ 
			ImGui::CloseCurrentPopup(); 
			for (auto i = 0; i < max_inputSize; ++i) { filename_input[i] = 0; }
		}

        ImGui::EndPopup();
    }
}

void MainMenuGui::generate_loadModal_layout()
{
    if (ImGui::BeginPopupModal("LoadModal", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
    {
		ImGui::SetWindowFontScale(GSet::imgui_scale());
			
		static auto selected_file = std::string{};

        // Child window: list of filenames
        {

            auto existent_saves = FsUtil::get_directoryFilenames(GStateSet::saves_folder);

			ImGui::BeginChild("Child1", {childModal_width, childModal_height}, true, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::SetWindowFontScale(GSet::imgui_scale());

            for (auto const& fn : existent_saves)
            {
				// Remove the extension
				auto const target = std::regex{ "(" + GStateSet::saves_ext + ")" };
				auto const fn_trimmed = std::regex_replace(fn, target, "");

				if (ImGui::Selectable(fn_trimmed.c_str(), fn == selected_file)) 
				{ 
					selected_file = fn; 
				}
            }

            ImGui::EndChild();
        }

		if (ImGui::Button("Load") && !selected_file.empty()) 
		{
			// Remove the extension.
			auto const fn = std::string{ selected_file };
			auto const target = std::regex{ "(" + GStateSet::saves_ext + ")" };
			auto const fn_trimmed = std::regex_replace(fn, target, "");

			m_gui_events.push<LoadWorldEv>(fn_trimmed);
			ImGui::CloseCurrentPopup(); 
			selected_file.erase();
		}
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 100.f * GSet::imgui_scale());
		if (ImGui::Button("Cancel")) 
		{ 
			ImGui::CloseCurrentPopup(); 
			selected_file.erase();
		}

        ImGui::EndPopup();
    }
}

void MainMenuGui::generate_optionModal_layout()
{
    if (ImGui::BeginPopupModal("OptionModal", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
		ImGui::SetWindowFontScale(GSet::imgui_scale());
			
		if (ImGui::Button("Main loop Analyzer")) 
		{
			m_gui_events.push<MainLoopAnalyzerEv>();
			ImGui::CloseCurrentPopup(); 
			switch_state();
		}

		if (ImGui::Button("Movement Analyzer")) 
		{
			m_gui_events.push<MovementAnalyzerEv>();
			ImGui::CloseCurrentPopup(); 
			switch_state();
		}

		if (ImGui::Button("Controls")) 
		{
			m_gui_events.push<ControlPanelEv>();
			ImGui::CloseCurrentPopup(); 
			switch_state();
		}

        ImGui::EndPopup();
    }
}



} //namespace tgm