#include "area_template.hh"


#include "debug/logger/debug_printers.hh"


namespace tgm
{
	

	
auto operator<<(std::ostream & os, AreaType const at) -> std::ostream &
{
	switch (at)
	{
		case tgm::AreaType::none:
			os << "none";
			break;
		case tgm::AreaType::field:
			os << "field";
			break;
		case tgm::AreaType::cowshed:
			os << "cowshed";
			break;
		case tgm::AreaType::large_cowshed:
			os << "large_cowshed";
			break;
		case tgm::AreaType::xxl_cowshed:
			os << "xxl_cowshed";
			break;
		case tgm::AreaType::super_field:
			os << "super_field";
			break;
		case tgm::AreaType::farmyard:
			os << "farmyard";
			break;
		case tgm::AreaType::test:
			os << "test";
			break;
		default:
			break;
	}

	return os;
}



} //namespace tgm