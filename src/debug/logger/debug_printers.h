#ifndef GM_DEBUG_PRINTERS_H
#define GM_DEBUG_PRINTERS_H


#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "data_strctures/data_array.hh"
#include "graphics/color.hh"
#include "map/map_forward_decl.hh"
#include "system/vector2.hh"
#include "system/vector3.hh"

#include "debug/logger/logger.h"


namespace tgm
{



template<typename T>
auto operator<<(std::ostream & os, std::unique_ptr<T> const& p) -> std::ostream &
{
        os << p.get();

        return os;
}

template<typename T, typename U>
auto operator<<(Logger& lgr, const std::pair<T, U> &p) -> Logger&
{
	lgr << "pair{ " << p.first << ", " << p.second << " }";

	return lgr;
}

template<typename T>
auto operator<<(Logger& lgr, std::vector<T> const& v) -> Logger&
{
	lgr << "vector: ["
		<< Logger::addt;

	for (const T& el : v)
		lgr << Logger::nltb << el << ',';

	lgr << Logger::remt
		<< Logger::nltb << "]";

	return lgr;
}


template<typename K, typename T>
auto operator<<(Logger& lgr, const std::map<K, T> &m) -> Logger&
{
	lgr << "map: ["
		<< Logger::addt;

	for (const auto&[key, val] : m)
		lgr << Logger::nltb << "{" << key << ", " << val << "}";

	lgr << Logger::remt
		<< Logger::nltb << "]";

	return lgr;
}

template<typename T>
auto operator<<(Logger& lgr, const std::unordered_set<T> &us) -> Logger&
{
	lgr << "unordered_set: ["
		<< Logger::addt;

	for (const auto& el : us)
		lgr << Logger::nltb << el;

	lgr << Logger::remt
		<< Logger::nltb << "]";

	return lgr;
}

template<typename K, typename T>
auto operator<<(Logger& lgr, const std::unordered_map<K, T> &um) -> Logger&
{
	lgr << "unordered_map: ["
		<< Logger::addt;

	for (const auto& [key, val] : um)
		lgr << Logger::nltb << "{" << key << "," << val << "}";

	lgr << Logger::remt
		<< Logger::nltb << "]";

	return lgr;
}

template<typename T, std::size_t N>
auto operator<<(Logger& lgr, std::array<T, N> const& a) -> Logger&
{
	lgr << "array: ["
		<< Logger::addt;

	for (auto it = a.cbegin(); it != a.cend(); ++it)
		lgr << Logger::nltb << *it << ',';

	lgr << Logger::remt
		<< Logger::nltb << "]";

	return lgr;
}

//TODO: Make Logger capable of handling std::ostringstream, then change the following operator<< from accepting std::osteam to Logger.
//namespace tgm
//{
template<typename T>
auto operator<<(std::ostream & os, Vector3<T> const& v) -> std::ostream &
{
    auto const w = os.width();
    os.width(0);

    os << "( " << std::setw(w) << v.x << " , " << std::setw(w) << v.y << " , " << std::setw(w) << v.z << " )";

    return os;
}

template<typename T>
auto operator>>(std::istream & is, Vector3<T> & v) -> std::istream &
{
	//TODO: PERFORMANCE: IO: Maybe it's better to make a custom parser, avoiding creating unused temporary variables
	auto unused = ' ';

	is >> unused >> v.x >> unused >> v.y >> unused >> v.z >> unused;

    return is;
}


template<typename T>
auto operator<<(std::ostream & os, Vector2<T> const& v) -> std::ostream &
{
        auto const w = os.width();
        os.width(0);

        os << "(" << std::setw(w) << v.x << ", " << std::setw(w) << v.y << ")";

        return os;
}


inline auto operator<<(std::ostream & os, Color const& c) -> std::ostream &
{
        os << "color(" << static_cast<unsigned>(c.r) << ", " << static_cast<unsigned>(c.g) << ", " << static_cast<unsigned>(c.b) << ")";

        return os;
}
		
////
//	Convert a DataArrayId in a human readable string.
////
inline auto human_did(DataArrayId const did) -> std::string
{
	std::ostringstream oss;
	oss << DataArray<int>::debug_slot(did) << " (v" << DataArray<int>::debug_version(did) << ')'; // The type "int" is used just to access the static function debug_slot that is independent from the tempate parameter

	return oss.str();
}

inline auto operator<<(std::ostream & os, BuildingAreaCompleteId const& acid) -> std::ostream &
{
        os << "{ " << human_did(acid.bid) << ", " << human_did(acid.aid) << " }";

        return os;
}



} // namespace tgm


#endif //GM_DEBUG_PRINTERS_H