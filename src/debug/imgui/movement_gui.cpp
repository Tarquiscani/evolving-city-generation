#include "movement_gui.hh"


#include <sstream>
#include <iomanip>

#include "debug/logger/debug_printers.hh"


namespace tgm
{



void MovementGui::update(Camera const& camera)
{
	++m_frames_from_lastMovement;


	if (GSet::TEST_playerSpritePosition != m_old_spritePos)
	{
		// Add another point to the graph of the movement durations (in frames).
		m_movement_durations.push_back(static_cast<float>(m_frames_from_lastMovement));

		if (m_movement_durations.size() > max_dataPoints)
		{
			m_movement_durations.erase(m_movement_durations.begin());
		}


		// The movement has already happened so reset this counter
		m_frames_from_lastMovement = 0;


		auto const newPos_x = GSet::units_to_screenPixels(GSet::TEST_playerSpritePosition.x, camera.pixel_dim());
		auto const oldPos_x = GSet::units_to_screenPixels(m_old_spritePos.x, camera.pixel_dim());
		m_movement_distances_x.push_back(std::abs(newPos_x - oldPos_x));

		if (m_movement_distances_x.size() > max_dataPoints)
		{
			m_movement_distances_x.erase(m_movement_distances_x.begin());
		}

		
		auto const newPos_y = GSet::units_to_screenPixels(GSet::TEST_playerSpritePosition.y, camera.pixel_dim());
		auto const oldPos_y = GSet::units_to_screenPixels(m_old_spritePos.y, camera.pixel_dim());
		m_movement_distances_y.push_back(std::abs(newPos_y - oldPos_y));

		if (m_movement_distances_y.size() > max_dataPoints)
		{
			m_movement_distances_y.erase(m_movement_distances_y.begin());
		}



		// Set che current position as the old position
		m_old_spritePos = GSet::TEST_playerSpritePosition;
	}


	if (GSet::TEST_cameraTargetPosition != m_old_targetPos)
	{
		auto dist_x = GSet::TEST_playerSpritePosition.x - GSet::TEST_cameraTargetPosition.x;
		auto dist_y = GSet::TEST_playerSpritePosition.y - GSet::TEST_cameraTargetPosition.y;

        auto dist = ::sqrtf(dist_x * dist_x + dist_y * dist_y); //TODO: 11: It should be std::sqtrf. It's been changed to being compatible with g++


		m_spriteCameraTarget_distances.push_back(dist);

		if (m_spriteCameraTarget_distances.size() > max_dataPoints)
		{
			m_spriteCameraTarget_distances.erase(m_spriteCameraTarget_distances.begin());
		}


		m_old_targetPos = GSet::TEST_cameraTargetPosition;
	}
}

void MovementGui::generate_layout(PlayerManager const& pmgr, Camera const& camera)
{
	if (m_open)
	{
		update(camera);


		std::ostringstream tp_oss; tp_oss << std::setfill(' ') << std::setw(3) << pmgr.debug_getPlayerPosition_inTiles();
		std::ostringstream mp_oss; mp_oss << std::fixed << std::setprecision(2) << std::setfill(' ') << std::setw(5) << pmgr.debug_getPlayerPosition_inUnits();
		auto const p_volumeBase = pmgr.debug_getPlayerVolume().base();
		std::ostringstream vb_oss; vb_oss << p_volumeBase; // Vector2f{ p_volumeBase.length, p_volumeBase.width };
		std::ostringstream sp_oss; sp_oss << std::setfill(' ') << std::setw(5) << GSet::TEST_playerSpritePosition;
		std::ostringstream vel_oss; vel_oss << pmgr.debug_getPlayerVelocity();
		std::ostringstream fmtv_oss; fmtv_oss << format_velocity(pmgr.debug_getPlayerVelocity(), camera);
		std::ostringstream mdur_oss; mdur_oss << m_movement_durations.back();
		std::ostringstream mdisx_oss; mdisx_oss << m_movement_distances_x.back();
		std::ostringstream mdisy_oss; mdisy_oss << m_movement_distances_y.back();
		std::ostringstream ctp_oss; ctp_oss << std::setfill(' ') << std::setw(5) << GSet::TEST_cameraTargetPosition;


		ImGui::Begin("Movement Gui", &m_open, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SetWindowFontScale(GSet::imgui_scale());


		ImGui::Text("Position (map RS - in tiles):");						ImGui::SameLine(350.f * GSet::imgui_scale()); ImGui::Text("%s", tp_oss.str().data());
		ImGui::Text("Position (map RS - in units):");						ImGui::SameLine(350.f * GSet::imgui_scale()); ImGui::Text("%s", mp_oss.str().data());
		ImGui::Text("Volume base dimensions (map RS - in units):");			ImGui::SameLine(350.f * GSet::imgui_scale()); ImGui::Text("%s", vb_oss.str().data());
		ImGui::Text("Sprite position (world space RS - in units):");		ImGui::SameLine(350.f * GSet::imgui_scale()); ImGui::Text("%s", sp_oss.str().data());
	
		ImGui::Text("Velocity (in units):");								ImGui::SameLine(350.f * GSet::imgui_scale()); ImGui::Text("%s", vel_oss.str().data());
		ImGui::Text("Velocity (in screen pixels):");						ImGui::SameLine(350.f * GSet::imgui_scale()); ImGui::Text("%s", fmtv_oss.str().data());
	
		ImGui::Text("Last movement duration:");								ImGui::SameLine(200.f * GSet::imgui_scale()); ImGui::Text("%s", mdur_oss.str().data());
		ImGui::PlotLines("Movement durations (historical data)", m_movement_durations.data(), static_cast<int>(m_movement_durations.size()));

		ImGui::Text("Last movement distance (world x-axis):");				ImGui::SameLine(300.f * GSet::imgui_scale()); ImGui::Text("%s", mdisx_oss.str().data());
		ImGui::Text("Last movement distance (world y-axis):");				ImGui::SameLine(300.f * GSet::imgui_scale()); ImGui::Text("%s", mdisy_oss.str().data());
		ImGui::PlotLines("Movement distances (historical data)", m_movement_distances_x.data(), static_cast<int>(m_movement_distances_x.size()));
		ImGui::PlotLines("Movement distances (historical data)", m_movement_distances_y.data(), static_cast<int>(m_movement_distances_y.size()));

		ImGui::NewLine();
		ImGui::Text("Target position (world space RS - in units):");		ImGui::SameLine(350.f * GSet::imgui_scale()); ImGui::Text("%s", ctp_oss.str().data());
		ImGui::PlotLines("Target-sprite distances (historical data)", m_spriteCameraTarget_distances.data(), static_cast<int>(m_spriteCameraTarget_distances.size()));


		ImGui::End();
	}
}

auto MovementGui::format_velocity(float const v, Camera const& camera) -> std::string
{
	std::ostringstream oss;

	auto const pix_dim = camera.pixel_dim();

	auto const screen_pixels = GSet::units_to_screenPixels(v, pix_dim);

	if (screen_pixels >= 1.f)
	{
		oss << std::setprecision(3) << std::setw(4) << screen_pixels << " pixels / frame (pixel dim: " << pix_dim << "; upt: " << GSet::upt << ")" << std::endl;
	}
	else
	{
		auto const reciprocal = 1.f / screen_pixels;

		oss << "(1/" << std::setprecision(3) << std::setw(4) << reciprocal << ") pixels / frame (pixel dim: " << pix_dim << "; upt: " << GSet::upt << ")" << std::endl;
	}

	return oss.str();
}



}