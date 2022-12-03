#include "mobile.h"


namespace tgm 
{
	


void MobileBody::add_move_direction(Direction const drc)
{
	auto current_drc = DirectionUtil::direction_to_unit_vector(m_move_drc);
	auto input_drc = DirectionUtil::direction_to_unit_vector(drc);

	m_move_drc = DirectionUtil::unit_vector_to_direction(Vector3i{ input_drc.x != 0 ? input_drc.x : current_drc.x,
																			 input_drc.y != 0 ? input_drc.y : current_drc.y,
																			 input_drc.z != 0 ? input_drc.z : current_drc.z });
}



} // namespace tgm