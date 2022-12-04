#ifndef GM_CAMERA_CONTROLLER_HH
#define GM_CAMERA_CONTROLLER_HH


#include "graphics/camera.hh"


namespace tgm
{
	


class CameraController
{
	private:
		static auto constexpr zoom_speed_default = 1.f;
		static auto constexpr zoom_speed_fast = 10.f;

	public:
		CameraController() = default;
		CameraController(CameraController const&) = delete;
		CameraController & operator=(CameraController const&) = delete;
		
		auto zoom_target() const noexcept { return m_zoom_target; }
		bool is_zoom_speed_fast() const noexcept { return m_zoom_speed == zoom_speed_fast; }

		void add_phi_input(float const input) noexcept { m_phi_input += input; }
		void add_theta_input(float const input) noexcept { m_theta_input += input; }
		void add_zoom_input(float const input);

		void toggle_zoom_speed() noexcept
		{
			m_zoom_speed = (m_zoom_speed == zoom_speed_default) ? zoom_speed_fast : zoom_speed_default;
		}

		////
		//  Update the angles of @camera in the prospective mode
		////
		void update_camera(float const delta_time, Camera & camera);

	private:
		float m_phi_input = 0.f;
		float m_theta_input = 0.f;
		float m_azimuthal_velocity = 0.f;
		float m_polar_velocity = 0.f;
		
		float m_zoom_target = 1.f;
		float m_zoom_speed = 1.f;

		////
		//	Determine the new angle taking into account the angular velocity, the angular accelaration applied by the player and the braking force. 
		////
		auto apply_input_to_angle(float const delta_time, float const angle, float & input, float & velocity) -> float;
};



} //namespace tgm


#endif //GM_CAMERA_CONTROLLER_HH