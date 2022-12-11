#include "border_type.hh"


namespace tgm
{



auto operator<<(std::ostream & os, BorderType type) -> std::ostream &
{
    os << "GENERIC OVERLOAD - ";

    switch (type)
    {
        case BorderType::none:
            os << "none";
            break;
        case BorderType::solo:
            os << "solo";
            break;
        case BorderType::N:
            os << "N";
            break;
        case BorderType::E:
            os << "E";
            break;
        case BorderType::S:
            os << "S";
            break;
        case BorderType::W:
            os << "W";
            break;
        case BorderType::NS:
            os << "NS";
            break;
        case BorderType::EW:
            os << "EW";
            break;
        case BorderType::ES:
            os << "ES";
            break;
        case BorderType::SW:
            os << "SW";
            break;
        case BorderType::NW:
            os << "NW";
            break;
        case BorderType::NE:
            os << "NE";
            break;
        case BorderType::NEW:
            os << "NEW";
            break;
        case BorderType::NES:
            os << "NES";
            break;
        case BorderType::NSW:
            os << "NSW";
            break;
        case BorderType::ESW:
            os << "ESW";
            break;
        case BorderType::NESW:
            os << "NESW";
            break;
        default:
            throw std::runtime_error("Unhandled BorderType.");
            break;
    }

    return os;
}



} //namespace tgm