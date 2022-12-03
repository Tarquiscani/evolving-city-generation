#ifndef GM_VECTOR3_HH
#define GM_VECTOR3_HH


namespace tgm
{



template <typename T>
class Vector3
{
	public:
		////
		//  Creates a Vector3(0, 0, 0).
		////
		constexpr inline Vector3() :
			x(0), y(0),	z(0) { }

		constexpr inline Vector3(T a_x, T a_y, T a_z): 
			x(a_x), y(a_y), z(a_z) { }

		////
		//  This constructor doesn't replace the copy constructor, it's called only when U != T.
		//  A call to this constructor will fail to compile if U is not convertible to T.
		////
		template <typename U>
		constexpr inline explicit Vector3(Vector3<U> const& vector) :
			x(static_cast<T>(vector.x)),
			y(static_cast<T>(vector.y)),
			z(static_cast<T>(vector.z))	{}


		T x;
		T y;
		T z;
};


template <typename T>
inline auto operator-(Vector3<T> const& lhs) -> Vector3<T>
{
	return Vector3<T>(-lhs.x, -lhs.y, -lhs.z);
}

template <typename T>
inline auto operator +=(Vector3<T>& lhs, const Vector3<T>& rhs) -> Vector3<T> &
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;

	return lhs;
}

template <typename T>
inline auto operator -=(Vector3<T> & lhs, Vector3<T> const& rhs) -> Vector3<T> &
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;

	return lhs;
}

template <typename T>
inline auto operator *=(Vector3<T> & lhs, T rhs) -> Vector3<T> &
{
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;

	return lhs;
}

template <typename T>
inline auto operator /=(Vector3<T> & lhs, T rhs) -> Vector3<T> &
{
	lhs.x /= rhs;
	lhs.y /= rhs;
	lhs.z /= rhs;

	return lhs;
}

template <typename T>
inline auto operator+(const Vector3<T>& lhs, const Vector3<T>& rhs) -> Vector3<T>
{
	return Vector3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

template <typename T>
inline auto operator-(const Vector3<T>& lhs, const Vector3<T>& rhs) -> Vector3<T>
{
	return Vector3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

template <typename T>
inline auto operator*(const Vector3<T>& lhs, T rhs) -> Vector3<T>
{
	return Vector3<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

template <typename T>
inline auto operator*(T lhs, const Vector3<T>& rhs) -> Vector3<T>
{
	return Vector3<T>(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs);
}

template <typename T>
inline auto operator/(const Vector3<T>& lhs, T rhs) -> Vector3<T>
{
	return Vector3<T>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}



////
//	Strict equality between two vectors.
////
template <typename T>
inline bool operator ==(Vector3<T> const& lhs,  Vector3<T> const& rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

////
//	Strict difference between two vectors.
////
template <typename T>
inline bool operator !=(Vector3<T> const& lhs, Vector3<T> const& rhs)
{
	return !(lhs == rhs);
}


// Define the most common types
using Vector3i = Vector3<int>;
using Vector3f = Vector3<float>;



namespace Versor3i
{
	inline Vector3i constexpr N { -1,  0,  0 };
	inline Vector3i constexpr NE{ -1,  1,  0 };
	inline Vector3i constexpr E {  0,  1,  0 };
	inline Vector3i constexpr SE{  1,  1,  0 };
	inline Vector3i constexpr S {  1,  0,  0 };
	inline Vector3i constexpr SW{  1, -1,  0 };
	inline Vector3i constexpr W {  0, -1,  0 };
	inline Vector3i constexpr NW{ -1, -1,  0 };
	inline Vector3i constexpr U {  0,  0,  1 };
	inline Vector3i constexpr D {  0,  0, -1 };
}



} // namespace tgm


#endif // GM_VECTOR3_HH