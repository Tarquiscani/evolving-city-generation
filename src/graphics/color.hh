#ifndef GM_COLOR_HH
#define GM_COLOR_HH


#include <cstdint>
#include <algorithm>


namespace tgm
{



class Color
{
	public:
		////
		//	Constructs an opaque black color. It is equivalent to Color(0, 0, 0, 255).
		////
		constexpr Color() noexcept :
			r(0), g(0),	b(0), a(255) { }


		////
		//  Construct the color from its 4 RGBA components.
		//	@red, @green, @blue, @alpha: Components (in the range [0, 255]).
		////
		constexpr Color(std::uint8_t const red, std::uint8_t const green, std::uint8_t const blue, std::uint8_t const alpha = 255) noexcept :
			r(red),	g(green), b(blue), a(alpha) { }

		////
		//	Construct the color from 32-bit unsigned integer.
		////
		constexpr explicit Color(std::uint32_t const color) noexcept :
			r(static_cast<uint8_t>((color & 0xff000000) >> 24)),
			g(static_cast<uint8_t>((color & 0x00ff0000) >> 16)),
			b(static_cast<uint8_t>((color & 0x0000ff00) >> 8 )),
			a(static_cast<uint8_t>((color & 0x000000ff) >> 0 ))
		{ }

		////
		//  @return: Color represented as a 32-bit unsigned integer
		////
		std::uint32_t toInteger() const noexcept
		{
			return (r << 24) | (g << 16) | (b << 8) | a;
		}



		static Color const Black;
		static Color const White;
		static Color const Red;
		static Color const Green;
		static Color const Blue;
		static Color const Yellow;
		static Color const Magenta;
		static Color const Cyan;
		static Color const Mint;
		static Color const Transparent;

		static Color const Grey;

		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
		std::uint8_t a;
};


inline Color constexpr Color::Black(0, 0, 0);
inline Color constexpr Color::White(255, 255, 255);
inline Color constexpr Color::Red(255, 0, 0);
inline Color constexpr Color::Green(0, 255, 0);
inline Color constexpr Color::Blue(0, 0, 255);
inline Color constexpr Color::Yellow(255, 255, 0);
inline Color constexpr Color::Magenta(255, 0, 255);
inline Color constexpr Color::Cyan(0, 255, 255);
inline Color constexpr Color::Mint(0xAAFFC3FF);
inline Color constexpr Color::Transparent(0, 0, 0, 0);

inline Color constexpr Color::Grey(75, 75, 75);



bool operator==(Color const& left, Color const& right);


bool operator!=(Color const& left, Color const& right);



////
//	This operator returns the component-wise sum of two colors. Components that exceed 255 are clamped to 255.
////
auto operator+(Color const& left, Color const& right) -> Color;

////
//	This operator returns the component-wise subtraction of two colors. Components below 0 are clamped to 0.
////
auto operator-(Color const& left, Color const& right) -> Color;


////
//	This operator returns the component-wise multiplication (also called "modulation") of two colors.
//	Components are then divided by 255 so that the result is still in the range [0, 255].
////
auto operator*(Color const& left, Color const& right) -> Color;


////
//	This operator computes the component-wise sum of two colors, and assigns the result to the left operand.
//	Components that exceed 255 are clamped to 255.
////
auto operator+=(Color & left, Color const& right) -> Color &;


////
//	This operator computes the component-wise subtraction of two colors, and assigns the result to the left operand.
//	Components below 0 are clamped to 0.
////
auto operator-=(Color & left, Color const& right) -> Color &;


////
//	This operator returns the component-wise multiplication (also called "modulation") of two colors, and assigns the 
//	result to the left operand. Components are then divided by 255 so that the result is still in the range [0, 255].
////
auto operator*=(Color & left, Color const& right) -> Color &;



} // namespace tgm


#endif // GM_COLOR_HH