#ifndef GM_DOOR_HH
#define GM_DOOR_HH

#include "system/vector2.hh"
#include "system/vector3.hh"
#include "map/map_forward_decl.hh"

#include "settings/debug/debug_settings.hh"
#include "debug/logger/logger.h"


class Door
{
	public:
		Door(Vector3i const& position, bool const vertical) :
			m_position(position), m_vertical(vertical) {}

		bool is_open() const noexcept { return m_open; }
		auto position() const noexcept -> Vector3i { return m_position; }
		bool vertical() const noexcept { return m_vertical; }

		void do_open()
		{
			#if DYNAMIC_ASSERTS
				if (m_open)
					throw std::runtime_error("Opening an already open door.");
			#endif

			m_open = true;
		}

		void close()
		{
			#if DYNAMIC_ASSERTS
				if (!m_open)
					throw std::runtime_error("Closing and already closed door.");
			#endif

			m_open = false;
		}

		auto sprite_id() const noexcept -> SpriteId { return m_sprite_id; }
		void set_spriteId(SpriteId spid) noexcept { m_sprite_id = spid; }


		void open_if_closed()
		{
			if (!m_open)
				do_open();
		}

		void close_if_open()
		{
			if (m_open)
				close();
		}

	private:
		bool m_open = false;
		Vector3i m_position;
		bool m_vertical = false;
		SpriteId m_sprite_id = 0;
};

inline auto operator<<(Logger & lgr, Door const&) -> Logger &
{
	lgr << "door";

	return lgr;
}

#endif //GM_DOOR_HH