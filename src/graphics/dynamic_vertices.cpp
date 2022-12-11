#include "dynamic_vertices.hh"


#include <sstream>

#include "debug/logger/debug_printers.hh"


namespace tgm
{



DynamicVertices::DynamicVertices(size_type const max_size) : m_slot_mgr(max_size)
{
	auto const max_attributes = static_cast<decltype(m_attributes)::size_type>(max_size) * aps;

	if (max_attributes > m_attributes.max_size() )
		throw std::runtime_error("Such a big vertices_count is not allowed.");

	try	
	{
		m_attributes.reserve(max_attributes);
	}
	catch (std::exception const& e)
	{
		std::ostringstream oss; oss << "No space in RAM for such a big vertices_count. " << e.what();
		throw std::runtime_error(oss.str());
	}
}


auto DynamicVertices::create_sprite(WorldParallelepiped const volume, TextureSubimage const& subimage) -> SpriteId
{
	auto [recycling, new_id, slot] = m_slot_mgr.create_id();
	
	auto const attribute0_id = static_cast<decltype(m_attributes)::size_type>(slot) * aps;  // id of the first attribute of the slot

	if (recycling)
	{
		internal_setSprite(&m_attributes[attribute0_id], volume, subimage);	
	}
	else
	{
		m_attributes.insert(m_attributes.end(), aps, 0.0f);
				
		internal_setSprite(&m_attributes[attribute0_id], volume, subimage);
	}

	return new_id;
}


void DynamicVertices::set_sprite(SpriteId const id, WorldParallelepiped const volume, TextureSubimage const& subimage)
{
	auto slot = m_slot_mgr.validate_id(id);

	if (slot)
	{
		auto const attribute0_id = static_cast<decltype(m_attributes)::size_type>(*slot) * aps;  // id of the first attribute of the slot

		internal_setSprite(&m_attributes[attribute0_id], volume, subimage);
	}
}


void DynamicVertices::destroy(SpriteId const id)
{
	auto slot = m_slot_mgr.destroy_id(id);

	if (slot)
	{
		auto const attribute0_id = static_cast<decltype(m_attributes)::size_type>(*slot) * aps;  // id of the first attribute of the slot

		internal_setSprite(&m_attributes[attribute0_id], { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f, 0.f, default_texture_dynamics });
	}
}


//TODO: Update this old system to the new system where each dynamic sprite consists of two sprites (one oblique and one parallel to the floor), that way the competition
//		among sprites would become perfect.
void DynamicVertices::internal_setSprite(float *const a, WorldParallelepiped const volume, TextureSubimage const& subimage)
{
	// Top-left triangle																	text_coord_x						tex_coord y
	//top-left vertex										
	*a		= volume.left;		*(a+1) = volume.behind();		*(a+2)  = volume.up();		*(a+3)  = subimage.left;		*(a+4)  = subimage.top;
	//top-right vertex
	*(a+5)  = volume.right();	*(a+6)  = volume.behind();		*(a+7)  = volume.up();		*(a+8)  = subimage.right;		*(a+9)  = subimage.top;
	//bottom-left vertex
	*(a+10) = volume.left;		*(a+11) = volume.front;			*(a+12) = volume.down;		*(a+13) = subimage.left;		*(a+14) = subimage.bottom;


	// Bottom-right triangle																text_coord_x						tex_coord y
	//top-right vertex
	*(a+15) = volume.right();	*(a+16) = volume.behind();		*(a+17) = volume.up();		*(a+18) = subimage.right;		*(a+19) = subimage.top;
	//bottom-right vertex
	*(a+20) = volume.right();	*(a+21) = volume.front;			*(a+22) = volume.down;		*(a+23) = subimage.right;		*(a+24) = subimage.bottom;
	//bottom-left vertex
	*(a+25) = volume.left;	    *(a+26) = volume.front;			*(a+27) = volume.down;		*(a+28) = subimage.left;		*(a+29) = subimage.bottom;
}



auto operator<<(Logger & lgr, DynamicVertices const& dv) -> Logger &
{
	lgr << Logger::nltb << "DynamicVertices {"
		<< Logger::addt
		<< Logger::nltb << "m_slot_mgr: " << dv.m_slot_mgr
		<< Logger::nltb << "m_attributes.vertices_count(): " << dv.m_attributes.size();

	for (auto id : dv.m_slot_mgr.debug_idsVecRef())
	{
		if(!SlotManager::debug_isFree(id))
		{
			auto const slot_id = SlotManager::debug_slot(id);
			auto const attribute0_id = static_cast<decltype(dv.m_attributes)::size_type>(slot_id) * dv.aps;  // id of the first attribute of the slot

			lgr << "\n\n\n" << Logger::nltb << "Slot: " << slot_id;
			lgr << Logger::addt;

			float const* a = &dv.m_attributes[attribute0_id];

		
			lgr << Logger::nltb << "Top-left triangle"
				<< Logger::addt;

			lgr	<< Logger::nltb << "top-left vertex\t\t";
			for (int i = 0; i < 5; ++i)
				lgr << *(a + i) << '\t';

			lgr << Logger::nltb << "top-right vertex\t";
			for (int i = 5; i < 10; ++i)
				lgr << *(a + i) << '\t';

			lgr << Logger::nltb << "bottom-left vertex\t";
			for (int i = 10; i < 15; ++i)
				lgr << *(a + i) << '\t';
		
			lgr << Logger::remt;



			lgr << Logger::nltb << "Bottom-right triangle"
				<< Logger::addt;

			lgr << Logger::nltb << "top-right vertex\t";
			for (int i = 15; i < 20; ++i)
				lgr << *(a + i) << '\t';

			lgr << Logger::nltb << "bottom-right vertex\t";
			for (int i = 20; i < 25; ++i)
				lgr << *(a + i) << '\t';

			lgr << Logger::nltb << "bottom-left vertex\t";
			for (int i = 25; i < 30; ++i)
				lgr << *(a + i) << '\t';
		
			lgr << Logger::remt;


			lgr << Logger::remt;
		}
	}

	lgr << Logger::remt
		<< Logger::nltb << "}";

	return lgr;
}



} //namespace tgm