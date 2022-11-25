#ifndef GM_CAMERA_CONTROLLER_HH
#define GM_CAMERA_CONTROLLER_HH

#include "graphics/camera.hh"


namespace tgm
{
	


class CameraController
{
	public:
		CameraController() = default;
		CameraController(CameraController const&) = delete;
		CameraController & operator=(CameraController const&) = delete;

		void add_phi_input(float const input) noexcept { m_phi_input += input; }
		void add_theta_input(float const input) noexcept { m_theta_input += input; }

		////
		//  Update the angles of @camera in the prospective mode
		////
		void update_camera(float const delta_time, Camera & camera);

	private:
		float m_phi_input = 0.f;
		float m_theta_input = 0.f;
		float m_azimuthal_velocity = 0.f;
		float m_polar_velocity = 0.f;

		////
		//	Determine the new angle taking into account the angular velocity, the angular accelaration applied by the player and the braking force. 
		////
		auto apply_input_to_angle(float const delta_time, float const angle, float & input, float & velocity) -> float;
};



} //namespace tgm
using namespace tgm;


#endif //GM_CAMERA_CONTROLLER_HH