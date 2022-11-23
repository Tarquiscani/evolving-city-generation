#include "main_loop_analyzer_gui.hh"


#include <sstream>
#include <iomanip>
#include <algorithm>

#include <imgui.h>


namespace tgm
{



void MainLoopAnalyzerGui::generate_layout(TimedCounter const& fps_counter, TimedCounter const& ups_counter, MainLoopData const& mainLoop_data)
{
	if (m_open)
	{
		auto fps_oss = std::ostringstream{}; fps_oss << std::setprecision(0) << fps_counter.perSecond_average();
		auto fc_oss = std::ostringstream{}; fc_oss << std::setfill(' ') << std::setw(5) << fps_counter();

		auto ups_oss = std::ostringstream{}; ups_oss << std::setprecision(2) << ups_counter.perSecond_average();
		auto ss_oss = std::ostringstream{}; ss_oss << std::setfill(' ') << std::setw(5) << ups_counter();
		
		if (!m_frozen)
		{
			static auto lambda = [](int const v) -> float { return static_cast<float>(v); };

			auto max = m_max;
			static auto lambda_with_max = [&max](int const v) -> float { 
				auto vf = static_cast<float>(v);
				if (vf > max) { max = vf; }; 
				return vf; 
			};

			fps_history.clear();
			std::transform(fps_counter.history().cbegin(), fps_counter.history().cend(), std::back_inserter(fps_history), lambda);

			ups_history.clear();
			std::transform(ups_counter.history().cbegin(), ups_counter.history().cend(), std::back_inserter(ups_history), lambda);
			
			// The tick time history set the scale of all the histograms
			tickT_history.clear();
			std::transform(mainLoop_data.tickT_hst().cbegin(), mainLoop_data.tickT_hst().cend(), std::back_inserter(tickT_history), lambda_with_max);
			m_max = max;
			
			updateT_history.clear();
			std::transform(mainLoop_data.updateT_hst().cbegin(), mainLoop_data.updateT_hst().cend(), std::back_inserter(updateT_history), lambda);
			
			renderingT_history.clear();
			std::transform(mainLoop_data.renderingT_hst().cbegin(), mainLoop_data.renderingT_hst().cend(), std::back_inserter(renderingT_history), lambda);
			
			swapT_history.clear();
			std::transform(mainLoop_data.swapT_hst().cbegin(), mainLoop_data.swapT_hst().cend(), std::back_inserter(swapT_history), lambda);

			inputT_history.clear();
			std::transform(mainLoop_data.inputT_hst().cbegin(), mainLoop_data.inputT_hst().cend(), std::back_inserter(inputT_history), lambda);
		}



		ImGui::Begin("Main-loop Analyzer", &m_open, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SetWindowFontScale(GSet::imgui_scale());


		ImGui::Text("Fps:");	 ImGui::SameLine(50.f * GSet::imgui_scale()); ImGui::Text("%s", fps_oss.str().data());
		ImGui::Text("Frame:");   ImGui::SameLine(50.f * GSet::imgui_scale()); ImGui::Text("%s", fc_oss.str().data());

		ImGui::Text("Ups:");	 ImGui::SameLine(50.f * GSet::imgui_scale()); ImGui::Text("%s", ups_oss.str().data());
		ImGui::Text("Step:");    ImGui::SameLine(50.f * GSet::imgui_scale()); ImGui::Text("%s", ss_oss.str().data());
		
		auto const graph_size = ImVec2{ 600.f * GSet::imgui_scale(), 100.f * GSet::imgui_scale() };
		ImGui::PlotHistogram("Fps history (hz)", fps_history.data(), static_cast<int>(fps_history.size()), 0, "", FLT_MAX, FLT_MAX, graph_size);
		ImGui::PlotHistogram("Ups history (hz)", ups_history.data(), static_cast<int>(ups_history.size()), 0, "", FLT_MAX, FLT_MAX, graph_size);
		
		ImGui::PlotHistogram("Tick time history (mcs)",		 tickT_history.data(),		static_cast<int>(tickT_history.size()),		 0, "", 0.f, m_max, graph_size);
		ImGui::PlotHistogram("Update time history (mcs)",	 updateT_history.data(),	static_cast<int>(updateT_history.size()),	 0, "", 0.f, m_max, graph_size);
		ImGui::PlotHistogram("Rendering time history (mcs)", renderingT_history.data(), static_cast<int>(renderingT_history.size()), 0, "", 0.f, m_max, graph_size);
		ImGui::PlotHistogram("Swap time history (mcs)",		 swapT_history.data(),		static_cast<int>(swapT_history.size()),		 0, "", 0.f, m_max, graph_size);
		ImGui::PlotHistogram("Input time history (mcs)",	 inputT_history.data(),		static_cast<int>(inputT_history.size()),	 0, "", 0.f, m_max, graph_size);
		
		auto const button_dim = ImVec2{ (m_frozen ? 65.f : 50.f) * GSet::imgui_scale(), 30.f * GSet::imgui_scale() };
		if (ImGui::Button(m_frozen ? "Unfreeze" : "Freeze", button_dim)) { m_frozen = !m_frozen; }


		ImGui::End();
	}
}



}