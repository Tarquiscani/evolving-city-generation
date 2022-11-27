#include "imgui_panels.hh"



#include <cmath>

#include "window/glfw_wrapper.hh" //must be included before imgui_impl_*

#include "imgui_impl_custom/imgui_impl_opengl3.h"
#include "imgui_impl_custom/imgui_impl_glfw.h"
#include "utilities.hh"

#include "debug/logger/debug_printers.h"


namespace tgm
{
	


GuiManager::GuiManager(Window const& window, GuiEventQueues & gui_events)
	: tile_gui{ gui_events }, cityBlock_gui{ gui_events }, mainMenu_gui{ gui_events }, m_fbo_size{ window.fbo_size() }
{
	if (!window.is_open()) { throw std::runtime_error("The Window is closed."); }

	if (!control_gui.is_open()) { control_gui.switch_state(); }
	if (!tutorial_panel.is_open()) { tutorial_panel.switch_state(); }
}


void GuiManager::generate_layout(GameMap const& map, Camera const& camera, TimedCounter const& fps_counter, TimedCounter const& ups_counter, MainLoopData const& mainLoop_data, TimedCounter const& input_counter)
{
	if (!m_hide)
	{
		basic_gui.generate_layout(m_fbo_size, map.debug_getPlayerManager(), camera, fps_counter, ups_counter, input_counter);
		movement_gui.generate_layout(map.debug_getPlayerManager(), camera);
		tile_gui.generate_layout();
		cityBlock_gui.generate_layout();
		mainMenu_gui.generate_layout(m_fbo_size);
		mainLoopAnalyzer_gui.generate_layout(fps_counter, ups_counter, mainLoop_data);
		control_gui.generate_layout(m_fbo_size);
		tutorial_panel.generate_layout(m_fbo_size);
	}
}

//TODO: NOW: Rimuovi input_counter dopo i test
void BasicGui::generate_layout(Vector2i const framebuffer_size, PlayerManager const& pmgr, Camera const& camera, TimedCounter const& fps_counter, TimedCounter const& ups_counter, TimedCounter const& input_counter)
{
	auto fps_oss = std::ostringstream{}; fps_oss << std::setprecision(0) << fps_counter.perSecond_average();
	auto fc_oss = std::ostringstream{}; fc_oss << std::setfill(' ') << std::setw(5) << fps_counter();

	auto ups_oss = std::ostringstream{}; ups_oss << std::setprecision(2) << ups_counter.perSecond_average();
	auto ss_oss = std::ostringstream{}; ss_oss << std::setfill(' ') << std::setw(5) << ups_counter();

	auto ips_oss = std::ostringstream{}; ips_oss << std::setprecision(2) << input_counter.perSecond_average();
	auto i_oss = std::ostringstream{}; i_oss << input_counter();

	std::ostringstream tp_oss; tp_oss << std::setfill(' ') << std::setw(3) << pmgr.debug_getPlayerPosition_inTiles();
	std::ostringstream zm_oss; zm_oss << camera.zoom();
	std::ostringstream pd_oss; pd_oss << camera.pixel_dim();


	ImGui::SetNextWindowPos(ImVec2{ framebuffer_size.x - 145.f * GSet::imgui_scale(), 5.f * GSet::imgui_scale() });

	ImGui::Begin("Basic Gui", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowFontScale(GSet::imgui_scale());

	ImGui::Text("Fps:");	 ImGui::SameLine(50.f * GSet::imgui_scale()); ImGui::Text("%s", fps_oss.str().data());
	ImGui::Text("Frame:");   ImGui::SameLine(50.f * GSet::imgui_scale()); ImGui::Text("%s", fc_oss.str().data());

	ImGui::Text("Ups:");	 ImGui::SameLine(50.f * GSet::imgui_scale()); ImGui::Text("%s", ups_oss.str().data());
	ImGui::Text("Step:");    ImGui::SameLine(50.f * GSet::imgui_scale()); ImGui::Text("%s", ss_oss.str().data());

	ImGui::Text("Ips:");	 ImGui::SameLine(50.f * GSet::imgui_scale()); ImGui::Text("%s", ips_oss.str().data());
	ImGui::Text("Step:");    ImGui::SameLine(50.f * GSet::imgui_scale()); ImGui::Text("%s", i_oss.str().data());

	ImGui::Text("%s", tp_oss.str().data());

	ImGui::Text("Zoom:");	 ImGui::SameLine(50.f * GSet::imgui_scale()); ImGui::Text("%s", zm_oss.str().data());
	ImGui::Text("Pix dim:"); ImGui::SameLine(70.f * GSet::imgui_scale()); ImGui::Text("%s", pd_oss.str().data());

	ImGui::End();
}


void TileGui::generate_layout()
{
	static auto open = false;

	// If "m_t" is "null", then don't open the panel
	open = m_t;

	if (open)
	{
		auto oss10 = std::ostringstream{}; oss10 << m_t->get_coordinates();
		std::ostringstream oss2; oss2 << std::boolalpha << m_t->is_impassable();
		std::ostringstream oss8; oss8 << human_did(m_t->block());
		auto oss11 = std::ostringstream{}; oss11 << m_t->is_innerArea();
		std::ostringstream oss3; oss3 << m_t->borders_count();
		std::ostringstream oss9; oss9 << m_t->m_roof_count;
		std::ostringstream oss4; oss4 << std::boolalpha << m_t->is_door();
		std::ostringstream oss5; oss5 << std::boolalpha << m_t->door_open;
		std::ostringstream oss6; oss6 << m_t->hosted_mobiles;
		


		auto oss_binfos = std::vector<std::ostringstream>{};
		if (m_t->is_built()) 
		{ 
			if (m_t->is_innerArea())
			{
				auto const& binfo = m_t->get_innerAreaInfo();
				auto & oss = oss_binfos.emplace_back();
				oss << "bid: " << human_did(binfo.bid()) << "\taid: " << human_did(binfo.aid());
			}
			else	//is a border
			{
				for (auto i = 0u; i < m_t->borders_count(); ++i)
				{
					auto const& binfo = m_t->get_borderInfos()[i];
					auto & oss = oss_binfos.emplace_back();
					oss << "bid: " << human_did(binfo.bid()) << "\taid: " << human_did(binfo.aid());
				}
			}
		}
		
		std::vector<std::ostringstream> oss_rinfos(m_t->m_roof_count);
		for (auto i = 0u; i < m_t->m_roof_count; ++i)
		{
			auto & rinfo = m_t->roof_infos()[i];
			oss_rinfos[i] << "bid: " << human_did(rinfo.bid) << "\trid: " << human_did(rinfo.roof_id);
		}

		ImGui::Begin("Tile Gui", &open, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SetWindowFontScale(GSet::imgui_scale());


		auto const offset = 120.f * GSet::imgui_scale();

		ImGui::Text("Position:");	ImGui::SameLine(offset); ImGui::Text("%s", oss10.str().data());
		ImGui::Text("Impassable:"); ImGui::SameLine(offset); ImGui::Text("%s", oss2.str().data());
		ImGui::Text("Borders #:");	ImGui::SameLine(offset); ImGui::Text("%s", oss3.str().data());
		if (oss_binfos.size() != 0u && ImGui::TreeNode("BuildingInfos: "))
		{
			for (auto const& oss : oss_binfos)
			{
				ImGui::Text("%s", oss.str().c_str());
			}
			ImGui::TreePop();
		}
		if (m_t->m_roof_count != 0 && ImGui::TreeNode("RoofInfos: "))
		{
			for (auto i = 0u; i < m_t->m_roof_count; ++i)
			{
				ImGui::Text("%s", oss_rinfos[i].str().c_str());
			}
			ImGui::TreePop();
		}
		if (ImGui::Selectable("CityBlock:")) 
		{ 
			m_gui_events.push<RetrieveCityBlockEv>(true, m_t->block()); 
		}
									ImGui::SameLine(offset); ImGui::Text("%s", oss8.str().data());
		ImGui::Text("Door:");		ImGui::SameLine(offset); ImGui::Text("%s", oss4.str().data());
		ImGui::Text("Door open:");	ImGui::SameLine(offset); ImGui::Text("%s", oss5.str().data());
		ImGui::Text("Mobiles:");	ImGui::SameLine(offset); ImGui::Text("%s", oss6.str().data());


		ImGui::End();

		
		ImGui::ShowDemoWindow();
	}


	// If the panel have been closed, then dissociate the current Tile
	if (!open) 
	{ 
		m_t = nullptr;
	}
}



void CityBlockGui::generate_layout()
{
	auto const [cbid, cb] = check_events();

	if (cbid != 0)
	{
		auto open = true;

		std::ostringstream cbid_oss; cbid_oss << human_did(cbid);
		std::ostringstream oss;  oss << cb->center();


		ImGui::Begin("CityBlock Gui", &open, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SetWindowFontScale(GSet::imgui_scale());


		auto const offset = 120.f * GSet::imgui_scale();
			
		ImGui::Text("Id:");			ImGui::SameLine(offset); ImGui::Text("%s", cbid_oss.str().c_str());
		ImGui::Text("Center:");		ImGui::SameLine(offset); ImGui::Text("%s", oss.str().c_str());


		ImGui::End();



		if (open)
		{
			m_gui_events.push<RetrieveCityBlockEv>(false, cbid);
		}
	}
}


auto CityBlockGui::check_events() -> std::pair<CityBlockId, CityBlock const*>
{
	auto ret = std::pair<CityBlockId, CityBlock const*>{};
	auto & [cbid, cb] = ret;


	auto & ocbg_queue = m_gui_events.get<OpenCityBlockGuiEv>();
		
	#if DYNAMIC_ASSERTS
		if (ocbg_queue.size() > 2) { throw std::runtime_error("It should contain at most 2 events: one related to the update of the current block and one if the user has asked for a new block."); }
	#endif

	if (!ocbg_queue.empty())
	{
		auto & first_e = ocbg_queue.front();

		cbid = first_e.cbid;
		cb = first_e.city_block;
		
		ocbg_queue.pop();

		// If there are two events, then prefer the one that comes from the user over the one related to the update
		if (!ocbg_queue.empty())
		{
			auto & second_e = ocbg_queue.front();

			if (second_e.user_request)
			{ 
				cbid = second_e.cbid;
				cb = second_e.city_block;
			}

			ocbg_queue.pop();
		}
		
		#if DYNAMIC_ASSERTS
			if (!cb) { throw std::runtime_error("Must be a non-null pointer."); }
		#endif
	}

	return ret;
}


void ControlGui::generate_layout(Vector2i const framebuffer_size)
{
	if (m_open)
	{
		ImGui::Begin("Controls", &m_open, ImGuiWindowFlags_NoSavedSettings);
		ImGui::SetWindowFontScale(GSet::imgui_scale());

		auto const offset = 180.f * GSet::imgui_scale();


		ImGui::Text("WASD:");					ImGui::SameLine(offset); ImGui::Text("Vertical and horizontal movement");
		ImGui::Text("Mouse Left Button:");		ImGui::SameLine(offset); ImGui::Text("Teleport the character to that point");
		ImGui::Text("Mouse wheel:");			ImGui::SameLine(offset); ImGui::Text("Zoom in / Zoom out");
		ImGui::Text("Shift + Mouse Wheel:");	ImGui::SameLine(offset); ImGui::Text("Faster zoom in / zoom out");
		ImGui::Text("V:");						ImGui::SameLine(offset); ImGui::Text("Decrease character's velocity");
		ImGui::Text("B:");						ImGui::SameLine(offset); ImGui::Text("Increase character's velocity");
		ImGui::NewLine();
		ImGui::Text("Esc:");					ImGui::SameLine(offset); ImGui::Text("In-game menu");
		ImGui::NewLine();
		ImGui::Text("Space Bar:");				ImGui::SameLine(offset); ImGui::Text("Add a plain building to the map");
		ImGui::Text("Backspace:");				ImGui::SameLine(offset); ImGui::Text("Remove a building");
		ImGui::NewLine();
		ImGui::Text("I:");						ImGui::SameLine(offset); ImGui::Text("Hide HUD");
		ImGui::Text("O:");						ImGui::SameLine(offset); ImGui::Text("Toggle all doors");
		ImGui::Text("Mouse Middle Button:");	ImGui::SameLine(offset); ImGui::Text("Open information panel about that tile");
		ImGui::NewLine();
		ImGui::Text("P:");						ImGui::SameLine(offset); ImGui::Text("Change projection mode");
		ImGui::Text("Arrow Keys:");				ImGui::SameLine(offset); ImGui::Text("Rotate camera in the perspective projection mode");
		ImGui::NewLine();
		ImGui::Text("Y:");						ImGui::SameLine(offset); ImGui::Text("Toggle between the depths of the Visual Debug algorithm inspector");
		ImGui::Text("1 (not numeric keypad):");	ImGui::SameLine(offset); ImGui::Text("Toggle the opening of the Visual Debug for the city expansion algorithm");
		ImGui::Text("3 (not numeric keypad):");	ImGui::SameLine(offset); ImGui::Text("Toggle the opening of the Visual Debug for the roof generation algorithm");
		ImGui::NewLine();
		ImGui::Text("IN THE VISUAL DEBUG WINDOW");
		ImGui::Text("B:");			ImGui::SameLine(offset); ImGui::Text("Go to the previous step of the algorithm");
		ImGui::Text("N:");			ImGui::SameLine(offset); ImGui::Text("Go to the next step of the algorithm");
		ImGui::Text("H:");			ImGui::SameLine(offset); ImGui::Text("Navigate quickly through the previous steps of the algorithm");
		ImGui::Text("J:");			ImGui::SameLine(offset); ImGui::Text("Navigate quickly through the next steps of the algorithm");
		ImGui::Text("N:");			ImGui::SameLine(offset); ImGui::Text("Navigate to the previous time in which the algorithm was used");
		ImGui::Text("M:");			ImGui::SameLine(offset); ImGui::Text("Navigate to the next time in which the algorithm was used");
		ImGui::Text("ESCAPE:");		ImGui::SameLine(offset); ImGui::Text("Close the Visual Debug window and deactivate the debug mode");
		ImGui::Text("U:");			ImGui::SameLine(offset); ImGui::Text("Close the Visual Debug window and go to the next chapter");

		ImGui::End();
	}
}


} //namespace tgm
