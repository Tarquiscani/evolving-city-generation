#ifndef GM_VECTOR2_HH
#define GM_VECTOR2_HH


#include <cmath>
#include <iostream>


namespace tgm
{



template <typename T>
class Vector2
{
public:

    ////
    //  Creates a Vector2(0, 0).
    ////
	constexpr inline Vector2() :
		x(0), y(0) { }


	constexpr inline Vector2(T a_x, T a_y) :
		x(a_x), y(a_y) { }


	template <typename U>
	constexpr inline explicit Vector2(Vector2<U> const& vector) :
		x(static_cast<T>(vector.x)),
		y(static_cast<T>(vector.y))
	{ }


    T x;
    T y;
};


template <typename T>
inline auto operator-(Vector2<T> const& rhs) -> Vector2<T>
{
	return Vector2<T>(-rhs.x, -rhs.y);
}


template <typename T>
inline auto operator+=(Vector2<T> & lhs, Vector2<T> const& rhs) -> Vector2<T> &
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;

	return lhs;
}


template <typename T>
inline auto operator-=(Vector2<T> & lhs, Vector2<T> const& rhs) -> Vector2<T> &
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;

	return lhs;
}


template <typename T>
inline auto operator*=(Vector2<T> & lhs, T rhs) -> Vector2<T> &
{
	lhs.x *= rhs;
	lhs.y *= rhs;

	return lhs;
}


template <typename T>
inline auto operator/=(Vector2<T> & lhs, T rhs) -> Vector2<T> &
{
	lhs.x /= rhs;
	lhs.y /= rhs;

	return lhs;
}


template <typename T>
inline auto operator+(Vector2<T> const& lhs, Vector2<T> const& rhs) -> Vector2<T>
{
	return Vector2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}


template <typename T>
inline auto operator-(Vector2<T> const& lhs, Vector2<T> const& rhs) -> Vector2<T>
{
	return Vector2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}


template <typename T>
inline auto operator*(Vector2<T> const& lhs, T rhs) -> Vector2<T>
{
	return Vector2<T>(lhs.x * rhs, lhs.y * rhs);
}


template <typename T>
inline auto operator*(T lhs, Vector2<T> const& rhs) -> Vector2<T>
{
	return Vector2<T>(rhs.x * lhs, rhs.y * lhs);
}


template <typename T>
inline auto operator/(Vector2<T> const& lhs, T rhs) -> Vector2<T>
{
	return Vector2<T>(lhs.x / rhs, lhs.y / rhs);
}



template <typename T>
inline bool operator ==(Vector2<T> const& lhs, Vector2<T> const& rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}


template <typename T>
inline bool operator !=(Vector2<T> const& lhs, Vector2<T> const& rhs)
{
	return !(lhs == rhs);
}

// Define the most common types
using Vector2i = Vector2<int>;
using Vector2u = Vector2<unsigned int>;
using Vector2f = Vector2<float>;


namespace Versor2i
{
	inline Vector2i constexpr N { -1,  0 };
	inline Vector2i constexpr NE{ -1,  1 };
	inline Vector2i constexpr E {  0,  1 };
	inline Vector2i constexpr SE{  1,  1 };
	inline Vector2i constexpr S {  1,  0 };
	inline Vector2i constexpr SW{  1, -1 };
	inline Vector2i constexpr W {  0, -1 };
	inline Vector2i constexpr NW{ -1, -1 };
}



inline auto distance(Vector2f const lhs, Vector2f const rhs) -> float
{
	auto const diff = lhs - rhs;

    return ::sqrtf(diff.x * diff.x + diff.y * diff.y); //TODO: 11: It should be "std::sqtrf", but g++ doesn't allow it.
}



template<typename T>
auto operator<<(std::ostream & os, Vector2<T> const& v) -> std::ostream &
{
        auto const w = os.width();
        os.width(0);

        os << "(" << std::setw(w) << v.x << ", " << std::setw(w) << v.y << ")";

        return os;
}



} // namespace tgm


#endif // GM_VECTOR2_HH

