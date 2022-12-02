#ifndef GM_CAMERA_HH
#define GM_CAMERA_HH

#include <array>

#include "settings/graphics_settings.hh"
#include "system/vector3.hh"
#include "system/parallelepiped.hh"
#include "map/direction.h"
#include "dynamic_subimage.hh"


namespace tgm
{
	


enum class Projection
{
	Orthogonal,
	Perspective
};


struct NoRoofInfos
{
	unsigned noRoofRects_size;
	Vector2f const* noRoofRects_ptr;
	
	unsigned noRoofIntersections_size;
	Vector2f const* noRoofIntersections_ptr;
};

class Camera
{
	public:
		static auto constexpr phi_min = -100000.f;		// We set reasonably big min and max limits to user input just to avoid underflows and overflows
		static auto constexpr phi_max = 100000.f;		
		static auto constexpr theta_min = 1.f;
		static auto constexpr theta_max = 89.f;
		static auto constexpr zoom_speed_default = 1.f;
		static auto constexpr zoom_speed_fast = 10.f;
		// Maximum number of noRoofRects
		static auto constexpr max_noRoofRects = 4u;
		static auto constexpr max_noRoofIntersections = 2u;


		Camera() = default;
		Camera(Camera const&) = delete;
		Camera & operator=(Camera const&) = delete;


		////
		//	@return: (OpenGL world space coordinates)
		////
		auto target() const noexcept -> Vector3f { return m_target; }
		auto zoom() const noexcept -> float { return m_zoom; }
		auto phi() const noexcept -> float { return m_phi; }
		auto theta() const noexcept -> float { return m_theta; }
		
		auto zoom_level() const noexcept { return m_zoom; }
		bool is_zoom_speed_fast() const noexcept { return m_zoom_speed == zoom_speed_fast; }

		////
		//	@return: Distance from the target (in pixels -- world space reference system).
		////
		auto distance() const noexcept -> float;

		////
		//	@return: Dimension of a pixel (in units -- world space reference system).
		////
		auto pixel_dim() const noexcept -> float;

		auto projection() const noexcept { return m_projection; }

		bool are_noRoofRects_changed() const noexcept { return m_noRoofRects_changed; }
		auto noRoof_infos() -> NoRoofInfos
		{
			return { m_noRoofIntersections_size, m_noRoofRects, m_noRoofIntersections_size, m_noRoofIntersections };
		}
		auto noRoofRects_size() const noexcept { return m_noRoofRects_size; }
		auto noRoofRects_ptr() const noexcept -> Vector2f const* { return m_noRoofRects; }
		auto noRoofIntersections_size() const noexcept { return m_noRoofIntersections_size; }
		auto noRoofIntersections_ptr() const noexcept -> Vector2f const* { return m_noRoofIntersections; }
		void noRoofRects_flushed() noexcept { m_noRoofRects_changed = false; }


		////
		//	@target_vol: Volume of the target (in units -- map reference system).
		//	@target_subimage: Subimage associated to the sprite of the target.
		//	@target_drc: Direction towards which the target is moving.
		//	@noRoofArea_volume: Volumes of the areas in which the player currently resides (in tiles -- map reference system).
		////
		void set_target(FloatParallelepiped const target_vol, DynamicSubimage const& target_subimage, Direction const target_drc, std::vector<IntParallelepiped> const& noRoofArea_volumes);


		void shift_zoom(float shift) noexcept
		{
			auto const new_zoom = m_zoom + shift * m_zoom_speed;
			m_zoom = std::roundf(new_zoom * 10.f) * 0.1f;	// Round the zoom to the first decimal digit
			if (m_zoom < 0.1f) { m_zoom = 0.1f; }
		}
		void change_zoomSpeed() noexcept
		{
			m_zoom_speed = (m_zoom_speed == zoom_speed_default) ? zoom_speed_fast : zoom_speed_default;
		}
		
		void set_phi(float const new_phi) noexcept { m_phi = new_phi; }
		void set_theta(float const new_theta) noexcept { m_theta = new_theta; }

		//void decrease_phi() noexcept { m_phi -= 5.f; }
		//void increase_phi() noexcept { m_phi += 5.f; }
		//void decrease_theta() noexcept
		//{
		//	m_theta -= 5.f;
		//	if (m_theta < 0.001f) {	m_theta = 0.001f; }
		//}
		//void increase_theta() noexcept
		//{
		//	m_theta += 5.f;
		//	if (m_theta >= 90.f) { m_theta = 90.f; }
		//}

		void switch_projection() noexcept 
		{ 
			m_projection = (m_projection == Projection::Orthogonal) ? Projection::Perspective : Projection::Orthogonal;
		}

	private:
		Vector3f m_target{};
		float m_zoom = 1.f;
		float m_phi = 270.f;
		float m_theta = 60.f;

		Projection m_projection = Projection::Orthogonal;

		float m_zoom_speed = 1.f;
		

		std::vector<IntParallelepiped> m_old_noRoofAreasVolumes;
		bool m_noRoofRects_changed = false;
		unsigned m_noRoofRects_size = 0u;
		Vector2f m_noRoofRects[max_noRoofRects * 2]{};
		unsigned m_noRoofIntersections_size = 0u;
		Vector2f m_noRoofIntersections[max_noRoofIntersections * 2]{};

};



} //namespace tgm
using namespace tgm;


#endif //GM_CAMERA_HH