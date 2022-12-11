#include "camera_controller.hh"


#include <algorithm>

#include "utilities.hh"


namespace tgm
{


void CameraController::add_zoom_input(float const input)
{	
    auto new_target = m_zoom_target + input * m_zoom_speed;
    new_target = std::roundf(new_target * 10.f) * 0.1f;		// Round the zoom target to the first decimal digit
    m_zoom_target = std::max(0.1f, new_target);
}

void CameraController::update_camera(float const delta_time, Camera & camera)
{
    auto const new_phi = apply_input_to_angle(delta_time, camera.phi(), m_phi_input, m_azimuthal_velocity);
    camera.set_phi(std::clamp(new_phi, camera.phi_min, camera.phi_max));

    auto const new_theta = apply_input_to_angle(delta_time, camera.theta(), m_theta_input, m_polar_velocity);
    camera.set_theta(std::clamp(new_theta, camera.theta_min, camera.theta_max));

    // The relative tolerances empirically found to minimize the flickering when zooming in/out are:
    // * 0.001f    for m_zoom_target = 10.f
    // * 0.000001f for m_zoom_target =  1.f
    // Hence the following function that linearly interpolates the data.
    auto const rel_tolerance = 0.000111f * m_zoom_target - 0.00011f;
    auto const new_zoom = Utilities::interp(camera.zoom_level(), m_zoom_target, delta_time, 1.f, rel_tolerance);
    camera.set_zoom_level(new_zoom);
}


auto CameraController::apply_input_to_angle(float const delta_time, float const previous_angle, float & input, float & velocity) -> float
{
    auto new_angle = previous_angle;

    if (input != 0.f || velocity != 0.f)
    {
        static auto max_velocity = 70.f;			// degree/s
        static auto acceleration_per_input = 800.f; // (degree/s^2) / input
        static auto deceleration_abs = 300.f;		// degree/s^2

        auto braking_velocity = 0.f;
        if (velocity > 0.f)
        {
            braking_velocity = std::clamp(- deceleration_abs * delta_time, - velocity, 0.f);
        }
        else
        {
            braking_velocity = std::clamp(deceleration_abs * delta_time, 0.f, - velocity);
        }

        auto acceleration = input * acceleration_per_input;
            
        //std::cout << "velocity:" << velocity << "- input: " << input << " - delta_time: " << delta_time << " - braking_velocity: " << braking_velocity << " - accel_velocity:" << acceleration * delta_time << std::endl;

        velocity += braking_velocity;				// v = v0 + at
        velocity += acceleration * delta_time;

        velocity = std::clamp(velocity, -max_velocity, max_velocity);
        
        //std::cout << "velocity (after clamp):" << velocity << std::endl;

        new_angle = previous_angle + velocity * delta_time;				// x = x0 + vt
        
        //std::cout << "previous_angle:" << previous_angle << "- new_angle: " << new_angle << std::endl;

        input = 0.f;	// Reset the input
    }

    return new_angle;
}



} //namespace tgm