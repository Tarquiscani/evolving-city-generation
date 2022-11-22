#ifndef GM_VECTOR_IOF_HH
#define GM_VECTOR_IOF_HH


#include <vector>
#include <fstream>


namespace tgm
{



template<typename T>
auto operator<<(std::ofstream & ofs, std::vector<T> const& v) -> std::ofstream &
{
	ofs << v.size();

	for (T const& el : v)
	{
		ofs << '\n'; //it returns the base class, so I can't append another operator<< after it
		ofs << el;
	}

	return ofs;
}


template<typename T>
auto operator>>(std::ifstream & ifs, std::vector<T> & v) -> std::ifstream &
{
	auto size = typename std::vector<T>::size_type{ 0 };
	ifs >> size;
	
	try
	{
		v.resize(size);
	}
	catch (std::exception const& e)
	{
		std::ostringstream oss;	oss << "Not enough space in RAM for this vector (~" << (size * sizeof(T) / (1024*1024)) << " MB required). " << e.what();
		throw std::runtime_error(oss.str());
	}
	
	for (auto i = decltype(size){ 0 }; i < size; ++i)
	{
		ifs >> v[i];
	}

	return ifs;
}



} //namespace tgm


#endif //GM_VECTOR_IOF_HH