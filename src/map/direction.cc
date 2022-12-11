#include "direction.h"


namespace tgm
{



std::ostream& operator<<(std::ostream & os, Direction d) noexcept
{
    switch (d)
    {
        case Direction::U:
            os << "Up";
            break;
        case Direction::UN:
            os << "Up North";
            break;
        case Direction::UNE:
            os << "Up North-east";
            break;
        case Direction::UE:
            os << "Up East";
            break;
        case Direction::USE:
            os << "Up South-east";
            break;
        case Direction::US:
            os << "Up South";
            break;
        case Direction::USW:
            os << "Up South-west";
            break;
        case Direction::UW:
            os << "Up West";
            break;
        case Direction::UNW:
            os << "Up North-west";
            break;

        case Direction::N:
            os << "North";
            break;
        case Direction::NE:
            os << "North-east";
            break;
        case Direction::E:
            os << "East";
            break;
        case Direction::SE:
            os << "South-east";
            break;
        case Direction::S:
            os << "South";
            break;
        case Direction::SW:
            os << "South-west";
            break;
        case Direction::W:
            os << "West";
            break;
        case Direction::NW:
            os << "North-west";
            break;

        case Direction::L:
            os << "Down";
            break;
        case Direction::LN:
            os << "Down North";
            break;
        case Direction::LNE:
            os << "Down North-east";
            break;
        case Direction::LE:
            os << "Down East";
            break;
        case Direction::LSE:
            os << "Down South-east";
            break;
        case Direction::LS:
            os << "Down South";
            break;
        case Direction::LSW:
            os << "Down South-west";
            break;
        case Direction::LW:
            os << "Down West";
            break;
        case Direction::LNW:
            os << "Down North-west";
            break;

        case Direction::none:
            os << "None";
            break;
    }

    return os;
}



namespace DirectionUtil
{
    //TODO: 14: Elimina questa funzione ed elimina l'enum Direction del tutto.
    auto planeUnitVector(Direction const drc) -> Vector2i
    {
        Vector2i uv;

        switch (drc)
        {
        case Direction::N:
            uv = { -1, 0 };
            break;

        case Direction::E:
            uv = { 0, 1 };
            break;

        case Direction::S:
            uv = { 1, 0 };
            break;

        case Direction::W:
            uv = { 0, -1 };
            break;

        case Direction::NE:
            uv = { -1, 1 };
            break;

        case Direction::SE:
            uv = { 1, 1 };
            break;

        case Direction::SW:
            uv = { 1, -1 };
            break;

        case Direction::NW:
            uv = { -1, -1 };
            break;

        case Direction::none:
            uv = { 0, 0 };
            break;
        }

        return uv;
    }


    auto planeUnitVector_to_clockWiseId(Vector2i const uvec) -> int
    {
        if		(uvec == Versor2i::N)	{ return 0; }
        else if (uvec == Versor2i::NE)	{ return 1; }
        else if (uvec == Versor2i::E)	{ return 2; }
        else if (uvec == Versor2i::SE)	{ return 3; }
        else if (uvec == Versor2i::S)	{ return 4; }
        else if (uvec == Versor2i::SW)	{ return 5; }
        else if (uvec == Versor2i::W)	{ return 6; }
        else if (uvec == Versor2i::NW)	{ return 7; }
        else							{ throw std::runtime_error("Not a unit vector"); }
    }

    auto planeUnitVector_to_counterclockwiseId(Vector2i const uvec) -> int
    {
        if		(uvec == Versor2i::N)	{ return 0; }
        else if (uvec == Versor2i::NW)	{ return 1; }
        else if (uvec == Versor2i::W)	{ return 2; }
        else if (uvec == Versor2i::SW)	{ return 3; }
        else if (uvec == Versor2i::S)	{ return 4; }
        else if (uvec == Versor2i::SE)	{ return 5; }
        else if (uvec == Versor2i::E)	{ return 6; }
        else if (uvec == Versor2i::NE)	{ return 7; }
        else							{ throw std::runtime_error("Not a unit vector"); }
    }

    ////
    //	Useful function to iterate counterclockwise through plane directions.
    //	@nghb_id: A number ranging from '0' to '7'. '0' identifies the northern neighbor, '7' identifies the north-eastern neighbor.
    ////
    auto counterclockwiseId_to_planeUnitVector(int const ccw_id) -> Vector2i
    {
        switch (ccw_id % 8)
        {
            case 0:
                return Versor2i::N;
                break;
            case 1:
                return Versor2i::NW;
                break;
            case 2:
                return Versor2i::W;
                break;
            case 3:
                return Versor2i::SW;
                break;
            case 4:
                return Versor2i::S;
                break; 
            case 5:
                return Versor2i::SE;
                break;
            case 6:
                return Versor2i::E;
                break;
            case 7:
                return Versor2i::NE;
                break;
            default:
                throw std::runtime_error("Unexpected case. It's impossible to reach this statement.");
                break;
        }
    }

    //TODO: Test this function
    auto planeUnitVector_distance(Vector2i const lhs, Vector2i const rhs, bool const counterclockwise) -> int
    {
        if (counterclockwise)
        {
            auto lhs_id = planeUnitVector_to_counterclockwiseId(lhs);
            auto rhs_id = planeUnitVector_to_counterclockwiseId(rhs);

            if (lhs_id > rhs_id)
                rhs_id += 8;

            return rhs_id - lhs_id;
        }
        else
        {
            auto lhs_id = planeUnitVector_to_clockWiseId(lhs);
            auto rhs_id = planeUnitVector_to_clockWiseId(rhs);

            if (lhs_id > rhs_id)
                rhs_id += 8;

            return rhs_id - lhs_id;
        }
    }
}



} // namespace tgm