#ifndef GM_RECT_HH
#define GM_RECT_HH

#include <iostream>
#include <algorithm>

#include "system/vector2.hh"


namespace tgm
{



struct IntRect
{
    IntRect() = default;

    IntRect(int x_, int y_, int length_, int width_) noexcept:
        top(x_), left(y_), length(length_), width(width_) { }

    IntRect(Vector2i pos, int length_, int width_) noexcept:
        top(pos.x), left(pos.y), length(length_), width(width_) { }

    auto get_NWvertex() const noexcept -> Vector2i { return Vector2i(top, left); }
    void set_NWvertex(Vector2i p) noexcept { top = p.x; left = p.y; }

    ////
    //	Lowermost X belonging to the rectangle.
    ////
    int bottom() const noexcept { return top + length - 1; }

    ////
    //	Rightermost Y belonging to the rectangle.
    ////
    int right() const noexcept { return left + width - 1; }
 
    int centralX() const noexcept { return top + (length / 2); }
    int centralY() const noexcept { return left + (width / 2); }

    ////
    //	Note: It doesn't work with negative rectangles.
    //	Warning: Untested.
    ////
    bool contains(const IntRect other) const noexcept
    {
        return top <= other.top 
            && left <= other.left
            && bottom() >= other.bottom() 
            && right() >= other.right();
    }

    ////
    //	Warning: Untested.
    ////
    //TODO: Test this function (if it'll really be used)
    bool intersects(const IntRect other) const
    {
        IntRect intersection;
        return intersects(other, intersection);
    }

    ////
    //	Warning: Untested.
    ////
    //TODO: Test this function (if it'll really be used)
    bool intersects(const IntRect other, IntRect& intersection) const
    {
        // Rectangles with negative dimensions are allowed, so we must handle them correctly

        // Compute the min and max of the first rectangle on both axes
        int r1MinX = std::min(top, top + length);
        int r1MaxX = std::max(top, top + length);
        int r1MinY = std::min(left, left + width);
        int r1MaxY = std::max(left, left + width);

        // Compute the min and max of the second rectangle on both axes
        int r2MinX = std::min(other.top, other.top + other.length);
        int r2MaxX = std::max(other.top, other.top + other.length);
        int r2MinY = std::min(other.left, other.left + other.width);
        int r2MaxY = std::max(other.left, other.left + other.width);

        // Compute the intersection boundaries
        int biggest_minX = std::max(r1MinX, r2MinX);
        int biggest_minY = std::max(r1MinY, r2MinY);
        int smallest_maxX = std::min(r1MaxX, r2MaxX);
        int smallest_minY = std::min(r1MaxY, r2MaxY);

        // If the intersection is valid (positive non zero area), then there is an intersection
        if ((biggest_minX < smallest_maxX) && (biggest_minY < smallest_minY))
        {
            intersection = IntRect(biggest_minX, biggest_minY, smallest_maxX - biggest_minX, smallest_minY - biggest_minY);
            return true;
        }
        else
        {
            intersection = IntRect(0, 0, 0, 0);
            return false;
        }
    }

    int top = 0;
    int left = 0;
    int length = 0;
    int width = 0;

    friend std::ostream& operator<<(std::ostream& os, IntRect r);
};

bool operator==(IntRect const& lhs, IntRect const& rhs);
bool operator!=(IntRect const& lhs, IntRect const& rhs);



struct FloatRect
{
    FloatRect() = default;

    FloatRect(float const a_x, float const a_y, float const a_length, float const a_width) noexcept :
        top(a_x), left(a_y), length(a_length), width(a_width) { }

    explicit FloatRect(IntRect const& int_par) :
        top(static_cast<float>(int_par.top)),
        left(static_cast<float>(int_par.left)), 
        length(static_cast<float>(int_par.length)), 
        width(static_cast<float>(int_par.width)) { }



    ////
    //	Lowermost X of the parallelepiped.
    ////
    auto bottom() const noexcept -> float { return top + length; }

    ////
    //	Rightermost Y of the parallelepiped.
    ////
    auto right() const noexcept -> float { return left + width; }



    float top = 0.f;
    float left = 0.f;
    float length = 0.f;
    float width = 0.f;

    friend auto operator<<(std::ostream & os, FloatRect const& fr) -> std::ostream &;
};

bool are_equal(FloatRect const& lhs, FloatRect const& rhs, float const abs_tolerance, float const rel_tolerance);



} //namespace tgm


#endif //GM_RECT_HH