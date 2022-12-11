#ifndef GM_roof_graphics_manager_HH
#define GM_roof_graphics_manager_HH


#include <unordered_map>

#include "mediators/roof_graphics_mediator.hh"
#include "map/gamemap.h"
#include "graphics/roof_vertices.hh"
#include "graphics/roof_graphics.hh"


namespace tgm
{



class RoofGraphicsManager
{
    public:
        RoofGraphicsManager(RoofGraphicsMediator & mediator, RoofVertices & roof_vertices) : m_mediator{ mediator }, m_roof_vertices{ roof_vertices } {}
        RoofGraphicsManager(RoofGraphicsManager const&) = delete;
        auto operator=(RoofGraphicsManager const&) -> RoofGraphicsManager & = delete;

        void prepare(GameMap const& simulation);

    private:
        RoofGraphicsMediator & m_mediator;
        RoofVertices & m_roof_vertices;

        std::unordered_map<RoofId, RoofGraphics> m_roof_graphics;
};



} //namespace tgm


#endif //GM_roof_graphics_manager_HH