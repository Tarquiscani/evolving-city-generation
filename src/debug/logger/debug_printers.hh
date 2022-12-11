#ifndef GM_DEBUG_PRINTERS_HH
#define GM_DEBUG_PRINTERS_HH


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
auto operator<<(Logger & lgr, std::pair<T, U> const& p) -> Logger &
{
	lgr << "pair{ " << p.first << ", " << p.second << " }";

	return lgr;
}

template<typename T>
auto operator<<(Logger & lgr, std::vector<T> const& v) -> Logger&
{
	lgr << "vector: ["
		<< Logger::addt;

	for (T const& el : v)
	{
		lgr << Logger::nltb << el << ',';
	}

	lgr << Logger::remt
		<< Logger::nltb << "]";

	return lgr;
}


template<typename K, typename T>
auto operator<<(Logger & lgr, std::map<K, T> const& m) -> Logger &
{
	lgr << "map: ["
		<< Logger::addt;

	for (auto const& [key, val] : m)
	{
		lgr << Logger::nltb << "{" << key << ", " << val << "}";
	}

	lgr << Logger::remt
		<< Logger::nltb << "]";

	return lgr;
}

template<typename T>
auto operator<<(Logger & lgr, std::unordered_set<T> const& us) -> Logger &
{
	lgr << "unordered_set: ["
		<< Logger::addt;

	for (auto const& el : us)
	{
		lgr << Logger::nltb << el;
	}

	lgr << Logger::remt
		<< Logger::nltb << "]";

	return lgr;
}

template<typename K, typename T>
auto operator<<(Logger & lgr, std::unordered_map<K, T> const& um) -> Logger &
{
	lgr << "unordered_map: ["
		<< Logger::addt;

	for (auto const& [key, val] : um)
	{
		lgr << Logger::nltb << "{" << key << "," << val << "}";
	}

	lgr << Logger::remt
		<< Logger::nltb << "]";

	return lgr;
}

template<typename T, std::size_t N>
auto operator<<(Logger & lgr, std::array<T, N> const& a) -> Logger&
{
	lgr << "array: ["
		<< Logger::addt;

	for (auto it = a.cbegin(); it != a.cend(); ++it)
	{
		lgr << Logger::nltb << *it << ',';
	}

	lgr << Logger::remt
		<< Logger::nltb << "]";

	return lgr;
}



} // namespace tgm


#endif //GM_DEBUG_PRINTERS_HH