#ifndef GM_PARALLELEPIPED_HH
#define GM_PARALLELEPIPED_HH


#include <iostream>
#include <algorithm>

#include "system/vector3.hh"
#include "system/rect.hh"


namespace tgm
{



struct IntParallelepiped
{
    IntParallelepiped() noexcept = default;

    IntParallelepiped(int const a_x, int const a_y, int const a_z, int const a_length, int const a_width, int const a_height) noexcept :
        behind(a_x), left(a_y), down(a_z), length(a_length), width(a_width), height(a_height) { }

    bool is_null() const noexcept
    {
        return behind == 0	&& left == 0 && down == 0 
            && length == 0 && width == 0 && height == 0;
    }

    ////
    //	Lowermost X belonging to the parallelepiped.
    ////
    int front() const noexcept { return behind + length - 1; }

    ////
    //	Rightermost Y belonging to the parallelepiped.
    ////
    int right() const noexcept { return left + width - 1; }

    ////
    //	Uppermost Z belonging to the parallelepiped.
    ////
    int up() const noexcept { return down + height - 1; }
    
    ////
    //	First X not belonging to the parallelepiped.
    ////
    int front_end() const noexcept { return behind + length; }
    ////
    //	First Y not belonging to the parallelepiped.
    ////
    int right_end() const noexcept { return left + width; }
    ////
    //	First Z not belonging to the parallelepiped.
    ////
    int up_end() const noexcept { return down + height; }

    auto begin() const noexcept -> Vector3i { return { behind, left, down }; }
    auto center() const noexcept -> Vector3i { return { behind + length / 2, left + width / 2, down + height / 2 }; }

    auto get_base() const noexcept -> IntRect { return { behind, left, length, width }; }
    auto base_dims() const noexcept -> Vector2i { return { length, width }; }

    ////
    //	Note: It doesn't work with negative rectangles. //TODO: Maybe it works with negative parallelepiped. Test it.
    //  Note: It should work with null rectangles. No tile belongs to null rectangles.
    //	Warning: Untested.
    ////
    bool contains(Vector3i const pos) const noexcept
    {
        return pos.x >= behind && pos.x < front_end()
            && pos.y >= left   && pos.y < right_end()
            && pos.z >= down   && pos.z < up_end();
    }

    //TODO: This function cannot handle correctly intersection of negative-dimension parallelepipeds. Think about negative-dimension parallelepipeds. 1
    //		Are they useful? Should I lose time properly implementing a management of negative parallelpipeds?
    auto intersect(IntParallelepiped const& other) const -> IntParallelepiped
    {
        // Rectangles with negative dimensions are allowed, so we must handle them correctly
        IntParallelepiped intersection;


        // Compute the min and max of the first rectangle on both axes
        int min_x = std::min(behind, behind + length);
        int max_x = std::max(behind, behind + length);
        int min_y = std::min(left, left + width);
        int max_y = std::max(left, left + width);
        int min_z = std::min(down, down + height);
        int max_z = std::max(down, down + height);

        // Compute the min and max of the second rectangle on both axes
        int other_min_x = std::min(other.behind, other.behind + other.length);
        int other_max_x = std::max(other.behind, other.behind + other.length);
        int other_min_y = std::min(other.left, other.left + other.width);
        int other_max_y = std::max(other.left, other.left + other.width);
        int other_min_z = std::min(other.down, other.down + other.height);
        int other_max_z = std::max(other.down, other.down + other.height);

        // Compute the intersection boundaries
        int biggest_min_x = std::max(min_x, other_min_x);
        int biggest_min_y = std::max(min_y, other_min_y);
        int biggest_min_z = std::max(min_z, other_min_z);
        int smallest_max_x = std::min(max_x, other_max_x);
        int smallest_max_y = std::min(max_y, other_max_y);
        int smallest_max_z = std::min(max_z, other_max_z);

        // If the intersection is valid (positive non-zero area), then there is an intersection
        if ((biggest_min_x < smallest_max_x) && (biggest_min_y < smallest_max_y) && (biggest_min_z < smallest_max_z))
        {
            intersection = IntParallelepiped(biggest_min_x, biggest_min_y, biggest_min_z, smallest_max_x - biggest_min_x, smallest_max_y - biggest_min_y, smallest_max_z - biggest_min_z);
        }
        else
        {
            intersection = IntParallelepiped(0, 0, 0, 0, 0, 0);
        }

        return intersection;
    }
    

    //TODO: This function cannot handle correctly intersection of negative-dimension parallelepipeds. Think about negative-dimension parallelepipeds. 1
    //		Are they useful? Should I lose time properly implementing a management of negative parallelpipeds?
    void combine(IntParallelepiped const& rhs);
    
    
    int behind = 0;
    int left = 0;
    int down = 0;
    int length = 0;
    int width = 0;
    int height = 0;

    friend auto operator<<(std::ostream & os, IntParallelepiped const& p) -> std::ostream &;
};

bool operator==(IntParallelepiped const& lhs, IntParallelepiped const& rhs);
bool operator!=(IntParallelepiped const& lhs, IntParallelepiped const& rhs);


 
struct FloatParallelepiped
{
    FloatParallelepiped(float const a_x, float const a_y, float const a_z, float const a_length, float const a_width, float const a_height) noexcept :
        behind(a_x), left(a_y), down(a_z), length(a_length), width(a_width), height(a_height) { }

    explicit FloatParallelepiped(IntParallelepiped const& int_par):
        behind(static_cast<float>(int_par.behind)), left(static_cast<float>(int_par.left)), down(static_cast<float>(int_par.down)), 
        length(static_cast<float>(int_par.length)), width(static_cast<float>(int_par.width)), height(static_cast<float>(int_par.height)) { }



    ////
    //	Lowermost X of the parallelepiped.
    ////
    auto front() const noexcept -> float { return behind + length; }

    ////
    //	Rightermost Y of the parallelepiped.
    ////
    auto right() const noexcept -> float { return left + width; }

    ////
    //	Uppermost Z of the parallelepiped.
    ////
    auto up() const noexcept -> float { return down + height; }


    auto center() const noexcept -> Vector3f { return { behind + length / 2.f, left + width / 2.f, down + height / 2.f }; }

    auto base() const noexcept -> FloatRect { return { behind, left, length, width }; }


    float behind = 0.f;
    float left = 0.f;
    float down = 0.f;
    float length = 0.f;
    float width = 0.f;
    float height = 0.f;
    
    friend auto operator<<(std::ostream & os, const FloatParallelepiped& p) -> std::ostream &;
};



} //namespace tgm


#endif //GM_PARALLELEPIPED_HH