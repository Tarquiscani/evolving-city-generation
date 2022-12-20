#ifndef GM_BORDER_TYPE_HH
#define GM_BORDER_TYPE_HH


#include <iostream>


namespace tgm
{


    
enum class BorderBackgroundType
{
    None,

    // NoSides
    NoSides,

    // SideCutTopShort
    SideCutTopShortLeft,
    SideCutTopShortRight,
    SideCutTopShortFull,

    // SideCutTopLong
    SideCutTopLongLeft,
    SideCutTopLongRight,
    SideCutTopLongFull,

    // Sides
    SideFull,
    SideCutBottomRight,
    SideCutBottomLeft,
    SideCutBottomFull,
};

enum class BorderSectionType
{
    None,

    // NoSection
    NoSection,
    NoSectionShadowLeft,
    NoSectionShadowRight,
    NoSectionShadowFront,
    NoSectionShadowLeftShadowRight,
    NoSectionShadowLeftShadowFront,
    NoSectionShadowRightShadowFront,
    NoSectionShadowLeftShadowRightShadowFront,

    // SectionBehind
    SectionBehind,
    SectionBehindShadowFront,
    SectionBehindShadowBehind,
    SectionBehindShadowFrontShadowBehind,

    // SectionFront
    SectionFront,
    SectionFrontShadowLeft,
    SectionFrontShadowRight,
    SectionFrontShadowLeftShadowRight,

    // SectionBehindFront
    SectionBehindFront,
    SectionBehindFrontShadowBehind,

    // SectionBehindLeft
    SectionBehindLeft,
    SectionBehindLeftShadowFront,

    // SectionBehindRight
    SectionBehindRight,
    SectionBehindRightShadowFront,

    // SectionFrontLeft
    SectionFrontBehindLeft,
    SectionFrontLeft,

    // SectionFrontRight
    SectionFrontBehindRight,
    SectionFrontRight,

    // SectionRightLeft
    SectionBehindRightLeft,
    SectionBehindRightLeftShadowFront,
    SectionRightLeft,
    SectionRightLeftShadowFront,
    SectionRight,
    SectionRightShadowFront,
    SectionLeft,
    SectionLeftShadowFront,

    // SectionFrontRightLeft
    SectionFrontBehindRightLeft,
    SectionFrontRightLeft,
};

enum class BorderCornerShadowType
{
    None,

    // 1 shadow
    CornerLeft,
    CornerRight,
    ShortLeft,
    ShortRight,

    // 2 shadows
    CornerLeftCornerRight,
    CornerLeftShortLeft,
    CornerLeftShortRight,
    CornerRightShortLeft,
    CornerRightShortRight,
    ShortLeftShortRight,

    // 3 shadows
    CornerLeftCornerRightShortLeft,
    CornerLeftCornerRightShortRight,
    CornerLeftShortLeftShortRight,
    CornerRightShortLeftShortRight,

    // 4 shadows
    Full,
};

struct BorderType
{
    BorderBackgroundType background = BorderBackgroundType::None;
    BorderSectionType section = BorderSectionType::None;
    BorderCornerShadowType corner_shadow = BorderCornerShadowType::None;
};

auto operator<<(std::ostream & os, BorderBackgroundType const type) -> std::ostream &;
auto operator<<(std::ostream & os, BorderSectionType const type) -> std::ostream &;
auto operator<<(std::ostream & os, BorderCornerShadowType const type) -> std::ostream &;



} //namespace tgm


#endif //GM_BORDER_TYPE_HH