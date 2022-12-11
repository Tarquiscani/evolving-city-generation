#include "color.hh"


namespace tgm
{



bool operator==(Color const& left, Color const& right)
{
    return (left.r == right.r)
        && (left.g == right.g)
        && (left.b == right.b)
        && (left.a == right.a);
}


bool operator!=(Color const& left, Color const& right)
{
    return !(left == right);
}



////
//	This operator returns the component-wise sum of two colors. Components that exceed 255 are clamped to 255.
////
auto operator+(Color const& left, Color const& right) -> Color
{
    return Color(std::uint8_t(std::min(int(left.r) + right.r, 255)),
        std::uint8_t(std::min(int(left.g) + right.g, 255)),
        std::uint8_t(std::min(int(left.b) + right.b, 255)),
        std::uint8_t(std::min(int(left.a) + right.a, 255)));
}


////
//	This operator returns the component-wise subtraction of two colors. Components below 0 are clamped to 0.
////
auto operator-(Color const& left, Color const& right) -> Color
{
    return Color(std::uint8_t(std::max(int(left.r) - right.r, 0)),
        std::uint8_t(std::max(int(left.g) - right.g, 0)),
        std::uint8_t(std::max(int(left.b) - right.b, 0)),
        std::uint8_t(std::max(int(left.a) - right.a, 0)));
}


////
//	This operator returns the component-wise multiplication (also called "modulation") of two colors.
//	Components are then divided by 255 so that the result is still in the range [0, 255].
////
auto operator*(Color const& left, Color const& right) -> Color
{
    return Color(std::uint8_t(int(left.r) * right.r / 255),
        std::uint8_t(int(left.g) * right.g / 255),
        std::uint8_t(int(left.b) * right.b / 255),
        std::uint8_t(int(left.a) * right.a / 255));
}


////
//	This operator computes the component-wise sum of two colors, and assigns the result to the left operand.
//	Components that exceed 255 are clamped to 255.
////
auto operator+=(Color & left, Color const& right) -> Color &
{
    return left = left + right;
}


////
//	This operator computes the component-wise subtraction of two colors, and assigns the result to the left operand.
//	Components below 0 are clamped to 0.
////
auto operator-=(Color & left, Color const& right) -> Color &
{
    return left = left - right;
}


////
//	This operator returns the component-wise multiplication (also called "modulation") of two colors, and assigns the 
//	result to the left operand. Components are then divided by 255 so that the result is still in the range [0, 255].
////
auto operator*=(Color & left, Color const& right) -> Color &
{
    return left = left * right;
}



} //namespace tgm