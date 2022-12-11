#include "hip_roof_microtile.hh"

#include <iostream>



namespace tgm
{



namespace HipRoofAlgorithm
{
    auto Microtile::type() const noexcept -> MicrotileType
    {
        if (m_height != -1)
        {
            auto incl = inclinations();
            if (m_height == 0)
            {
                if		(incl.count() == 1)				//nghb is a non-perimeter edge
                {
                    return MicrotileType::Edge;
                }
                else if (incl.count() == 2)
                {
                    if		((incl & (Microtile::north | Microtile::south)).count() == 2)
                        return MicrotileType::Edge;
                    else if ((incl & (Microtile::east  | Microtile::west )).count() == 2)
                        return MicrotileType::Edge;
                    else
                        return MicrotileType::Vertex;
                }
                else /*nghb_inc.count() > 2*/			//nghb is a perimeter vertex
                {
                    return MicrotileType::Vertex;
                }
            }
            else
            {
                if		(incl.count() == 1)				//nghb is a side
                {
                    return MicrotileType::Side;
                }
                else if (incl.count() == 2)				//nghb is an edge
                {
                    return MicrotileType::Edge;
                }
                else /*nghb_inc.count() > 2*/			//nghb is a vertex
                {
                    return MicrotileType::Vertex;
                }
            }
        }
        else
        {
            return MicrotileType::None;
        }
    }
    
    auto operator<<(std::ostream & os, MicrotileInclination const& mi) -> std::ostream &
    {
        switch(mi)
        {		
            case MicrotileInclination::North:
                os << "North";
                break;
            case MicrotileInclination::East:
                os << "East";
                break;
            case MicrotileInclination::South:
                os << "South";
                break;
            case MicrotileInclination::West:
                os << "West";
                break;
            case MicrotileInclination::Flat:
                os << "Flat";
                break;
            default:
                throw std::runtime_error("Unexpected MicrotileInclination.");
                break;
        }

        return os;
    }

    auto operator<<(std::ostream & os, MicrotileType const& mt) -> std::ostream &
    {
        switch(mt)
        {
            case MicrotileType::None:
                os << "None";
                break;
            case MicrotileType::Side:
                os << "Side";
                break;
            case MicrotileType::Edge:
                os << "Edge";
                break;
            case MicrotileType::Vertex:
                os << "Vertex";
                break;
            default:
                throw std::runtime_error("Unexpected MicrotileType.");
                break;
        }

        return os;
    }
}



} //namespace tgm