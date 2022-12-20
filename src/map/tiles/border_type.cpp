#include "border_type.hh"


#include "debug/asserts.hh"


namespace tgm
{



auto operator<<(std::ostream & os, BorderBackgroundType const type) -> std::ostream &
{
    switch (type)
    {
        case BorderBackgroundType::None:
            os << "None";
            break;
        case BorderBackgroundType::SideCutTopShortLeft:
            os << "SideCutTopShortLeft";
            break;
        case BorderBackgroundType::SideCutTopShortRight:
            os << "SideCutTopShortRight";
            break;
        case BorderBackgroundType::SideCutTopShortFull:
            os << "SideCutTopShortFull";
            break;
        case BorderBackgroundType::SideCutTopLongLeft:
            os << "SideCutTopLongLeft";
            break;
        case BorderBackgroundType::SideCutTopLongRight:
            os << "SideCutTopLongRight";
            break;
        case BorderBackgroundType::SideCutTopLongFull:
            os << "SideCutTopLongFull";
            break;
        case BorderBackgroundType::SideFull:
            os << "SideFull";
            break;
        case BorderBackgroundType::SideCutBottomRight:
            os << "SideCutBottomRight";
            break;
        case BorderBackgroundType::SideCutBottomLeft:
            os << "SideCutBottomLeft";
            break;
        case BorderBackgroundType::SideCutBottomFull:
            os << "SideCutBottomFull";
            break;
        default:
            os << "Unexpected type";
            check_no_entry();
            break;
    }

    return os;
}

auto operator<<(std::ostream & os, BorderSectionType const type) -> std::ostream &
{
    switch (type)
    {
        case BorderSectionType::None:
            os << "None";
            break;
        case BorderSectionType::NoSection:
            os << "NoSection";
            break;
        case BorderSectionType::NoSectionShadowLeft:
            os << "NoSectionShadowLeft";
            break;
        case BorderSectionType::NoSectionShadowRight:
            os << "NoSectionShadowRight";
            break;
        case BorderSectionType::NoSectionShadowFront:
            os << "NoSectionShadowFront";
            break;
        case BorderSectionType::NoSectionShadowLeftShadowRight:
            os << "NoSectionShadowLeftShadowRight";
            break;
        case BorderSectionType::NoSectionShadowLeftShadowFront:
            os << "NoSectionShadowLeftShadowFront";
            break;
        case BorderSectionType::NoSectionShadowRightShadowFront:
            os << "NoSectionShadowRightShadowFront";
            break;
        case BorderSectionType::NoSectionShadowLeftShadowRightShadowFront:
            os << "NoSectionShadowLeftShadowRightShadowFront";
            break;
        case BorderSectionType::SectionBehind:
            os << "SectionBehind";
            break;
        case BorderSectionType::SectionBehindShadowFront:
            os << "SectionBehindShadowFront";
            break;
        case BorderSectionType::SectionBehindShadowBehind:
            os << "SectionBehindShadowBehind";
            break;
        case BorderSectionType::SectionBehindShadowFrontShadowBehind:
            os << "SectionBehindShadowFrontShadowBehind";
            break;
        case BorderSectionType::SectionFront:
            os << "SectionFront";
            break;
        case BorderSectionType::SectionFrontShadowLeft:
            os << "SectionFrontShadowLeft";
            break;
        case BorderSectionType::SectionFrontShadowRight:
            os << "SectionFrontShadowRight";
            break;
        case BorderSectionType::SectionFrontShadowLeftShadowRight:
            os << "SectionFrontShadowLeftShadowRight";
            break;
        case BorderSectionType::SectionBehindFront:
            os << "SectionBehindFront";
            break;
        case BorderSectionType::SectionBehindFrontShadowBehind:
            os << "SectionBehindFrontShadowBehind";
            break;
        case BorderSectionType::SectionBehindLeft:
            os << "SectionBehindLeft";
            break;
        case BorderSectionType::SectionBehindLeftShadowFront:
            os << "SectionBehindLeftShadowFront";
            break;
        case BorderSectionType::SectionBehindRight:
            os << "SectionBehindRight";
            break;
        case BorderSectionType::SectionBehindRightShadowFront:
            os << "SectionBehindRightShadowFront";
            break;
        case BorderSectionType::SectionFrontBehindLeft:
            os << "SectionFrontBehindLeft";
            break;
        case BorderSectionType::SectionFrontLeft:
            os << "SectionFrontLeft";
            break;
        case BorderSectionType::SectionFrontBehindRight:
            os << "SectionFrontBehindRight";
            break;
        case BorderSectionType::SectionFrontRight:
            os << "SectionFrontRight";
            break;
        case BorderSectionType::SectionBehindRightLeft:
            os << "SectionBehindRightLeft";
            break;
        case BorderSectionType::SectionBehindRightLeftShadowFront:
            os << "SectionBehindRightLeftShadowFront";
            break;
        case BorderSectionType::SectionRightLeft:
            os << "SectionRightLeft";
            break;
        case BorderSectionType::SectionRightLeftShadowFront:
            os << "SectionRightLeftShadowFront";
            break;
        case BorderSectionType::SectionRight:
            os << "SectionRight";
            break;
        case BorderSectionType::SectionRightShadowFront:
            os << "SectionRightShadowFront";
            break;
        case BorderSectionType::SectionLeft:
            os << "SectionLeft";
            break;
        case BorderSectionType::SectionLeftShadowFront:
            os << "SectionLeftShadowFront";
            break;
        case BorderSectionType::SectionFrontBehindRightLeft:
            os << "SectionFrontBehindRightLeft";
            break;
        case BorderSectionType::SectionFrontRightLeft:
            os << "SectionFrontRightLeft";
            break;
        default:
            os << "Unexpected type";
            check_no_entry();
            break;
    }

    return os;
}

auto operator<<(std::ostream & os, BorderCornerShadowType const type) -> std::ostream &
{
    switch (type)
    {
        case BorderCornerShadowType::None:
            os << "None";
            break;
        case BorderCornerShadowType::CornerLeft:
            os << "CornerLeft";
            break;
        case BorderCornerShadowType::CornerRight:
            os << "CornerRight";
            break;
        case BorderCornerShadowType::ShortLeft:
            os << "ShortLeft";
            break;
        case BorderCornerShadowType::ShortRight:
            os << "ShortRight";
            break;
        case BorderCornerShadowType::CornerLeftCornerRight:
            os << "CornerLeftCornerRight";
            break;
        case BorderCornerShadowType::CornerLeftShortLeft:
            os << "CornerLeftShortLeft";
            break;
        case BorderCornerShadowType::CornerLeftShortRight:
            os << "CornerLeftShortRight";
            break;
        case BorderCornerShadowType::CornerRightShortLeft:
            os << "CornerRightShortLeft";
            break;
        case BorderCornerShadowType::CornerRightShortRight:
            os << "CornerRightShortRight";
            break;
        case BorderCornerShadowType::ShortLeftShortRight:
            os << "ShortLeftShortRight";
            break;
        case BorderCornerShadowType::CornerLeftCornerRightShortLeft:
            os << "CornerLeftCornerRightShortLeft";
            break;
        case BorderCornerShadowType::CornerLeftCornerRightShortRight:
            os << "CornerLeftCornerRightShortRight";
            break;
        case BorderCornerShadowType::CornerLeftShortLeftShortRight:
            os << "CornerLeftShortLeftShortRight";
            break;
        case BorderCornerShadowType::CornerRightShortLeftShortRight:
            os << "CornerRightShortLeftShortRight";
            break;
        case BorderCornerShadowType::Full:
            os << "Full";
            break;
        default:
            os << "Unexpected type";
            check_no_entry();
            break;
    }

    return os;
}



} //namespace tgm