#ifndef GM_BUILDING_RECIPE_HH
#define GM_BUILDING_RECIPE_HH


#include "system/vector2.hh"
#include "map/buildings/area_template.hh"

#include "settings/debug/debug_settings.hh"


namespace tgm
{



////
//	This is the input for the BuildingManager, providing all the essential informations to choose an appropriate position in the map.
////
class BuildingRecipe
{
    public:
        BuildingRecipe(Vector2f const proposed_position, 
                       AreaType const startingArea_type, Vector2i const startingArea_dims,
                       std::string const& expansionTemplate_name) :
            m_proposed_position(proposed_position),
            m_startingArea_type(startingArea_type), m_startingArea_dims(startingArea_dims),
            m_expansion_template{ expansionTemplate_name }
        {
            #if DYNAMIC_ASSERTS
                auto const min_dims = area_templates.at(startingArea_type).min_dims();
                if(startingArea_dims.x < min_dims.x || startingArea_dims.y < min_dims.y)
                {
                    throw std::runtime_error("An area cannot be smaller of the corresponding AreaTemplate::min_dims()."); 
                }
            #endif
        }

        auto proposed_position() const -> Vector2f { return m_proposed_position; }
        auto startingArea_type() const { return m_startingArea_type; }
        auto startingArea_dims() const -> Vector2i { return m_startingArea_dims; }
        auto expansion_template() const { return m_expansion_template; }

    private:
        Vector2f m_proposed_position;
        AreaType m_startingArea_type;
        Vector2i m_startingArea_dims;
        std::string m_expansion_template;
};



} //namespace tgm


#endif //GM_BUILDING_RECIPE_HH