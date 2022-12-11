#ifndef GM_MOVEMENT_GUI_HH
#define GM_MOVEMENT_GUI_HH


#include "graphics/camera.hh"
#include "systems/player_manager.hh"
#include "debug/imgui/base_gui.hh"


namespace tgm
{



class MovementGui : public BaseGui
{
    public:
        void generate_layout(PlayerManager const& pmgr, Camera const& camera);

    private:
        static inline auto const max_dataPoints = 20;

        std::vector<float> m_movement_durations = { 0.f }; //last movement durations (in frames)
        int m_frames_from_lastMovement = 0;


        std::vector<float> m_movement_distances_x = { 0.f }; //last distances covered by a single movement on the world x-axis (in pixels)
        std::vector<float> m_movement_distances_y = { 0.f }; //last distances covered by a single movement on the world y-axis (in pixels)


        Vector3f m_old_spritePos;
        Vector3f m_old_targetPos;


        std::vector<float> m_spriteCameraTarget_distances;


        void update(Camera const& camera);

        auto format_velocity(float const v, Camera const& camera) -> std::string;
};



}


#endif //GM_MOVEMENT_GUI_HH