#include "slot_manager.hh"


#include <exception>
#include <sstream>


namespace tgm
{



SlotManager::SlotManager(size_type const capacity)
{
	if (capacity > max_capacity)
		throw std::runtime_error("Such a big capacity is not allowed.");
			
	m_capacity = capacity;


	try
	{
		m_ids.reserve(m_capacity);
		m_free.reserve(m_capacity); //TODO: PERFORMANCE: m_free is hardly as big as m_ids, so a smaller initial value can be assigned
	}
	catch (std::exception const& e)
	{
		std::ostringstream oss; oss << "No space in RAM for such a big slot count. " << e.what();
		throw std::runtime_error(oss.str());
	}
}


auto SlotManager::create_id() -> std::tuple<bool, CompleteId, SlotId>
{
	std::tuple<bool, CompleteId, SlotId> ret;
	auto & [recycling, new_id, slot_id] = ret;
	new_id = 0;

	if (m_free.empty())
	{
		if(m_ids.size() >= m_capacity)
			throw std::runtime_error("Overflow: All the slots are full.");

		recycling = false;

		++m_max_used;
		new_id = m_max_used - 1; //conversion from size_type to SlotId
		new_id |= UINT64_C(0x0000000080000000); //version is set to 1
				
		m_ids.push_back(new_id);


		slot_id = slot(new_id);
	}
	else
	{
		recycling = true;

		auto free_slot = m_free.back();
		m_free.pop_back();
				
		// nullify the leftmost bit (that indicate a free state)  ---  id version was already changed in DataArray::destroy
		m_ids[free_slot] &= UINT64_C(0x7FFFFFFFFFFFFFFF);


		new_id = m_ids[free_slot];	
		slot_id = free_slot;
	}

	++m_count;

	return ret;
}


auto SlotManager::validate_id(CompleteId const id) -> std::optional<SlotId>
{
	std::optional<SlotId> ret;

	auto slot_id = slot(id);
	auto current_id = m_ids[slot_id];

	if (current_id != id) // Compare free status, version and slot. To be precise one should also verify that !is_free(el.id), 
	{					  // but if the version match that would be a rare case.
		#if DYNAMIC_ASSERTS
			throw std::runtime_error("Trying to validate the id of a destroyed object.");
		#endif
	}
	else
	{
		ret.emplace(slot_id);
	}

	return ret;
}


auto SlotManager::destroy_id(CompleteId const id) -> std::optional<SlotId>
{
	std::optional<SlotId> ret;

	auto id_slot = slot(id);
	auto current_id = m_ids[id_slot];

	if (current_id != id) //compare free status, version and slot
	{
		#if DYNAMIC_ASSERTS
			throw std::runtime_error("Trying to destroy an already destroyed id (or, rarely, the provided id is completely invalid and wrong).");
		#endif
	}
	else
	{
		m_ids[id_slot] = setFree_and_increaseVersion(current_id);

		m_free.push_back(id_slot);

		--m_count;


		ret.emplace(id_slot);
	}

	return ret;
}


auto SlotManager::setFree_and_increaseVersion(CompleteId id) -> CompleteId
{
	CompleteId vn = version(id); //conversion from size_type to DataArrayId

	#if DYNAMIC_ASSERTS
		if (vn == UINT64_C(0xFFFFFFFF))
			throw std::runtime_error("The maximum version has been reached.");

		if (is_free(id))
			throw std::runtime_error("Unexpected state: the id was already set to free.");
	#endif
			
	++vn;

	return id = UINT64_C(0x8000000000000000) | (vn << 31) | (id & UINT64_C(0x000000007FFFFFFF));
}


auto operator<<(Logger & lgr, SlotManager const& sm) -> Logger &
{
	lgr << Logger::nltb << "SlotManager {"
		<< Logger::addt
		<< Logger::nltb << "max_used: " << sm.m_max_used
	    << Logger::nltb << "m_free.vertices_count(): " << sm.m_free.size()
	    << Logger::nltb << "m_capacity: " << sm.m_capacity;

	for (auto id : sm.m_ids)
	{
		lgr << Logger::nltb
			<< Logger::nltb 
			<< Logger::nltb				<< "free    : "	 <<(SlotManager::debug_isFree(id) ? "yes" : "no") << std::noboolalpha
			<< Logger::nltb				<< "version : "	 << SlotManager::debug_version(id)
			<< Logger::nltb				<< "slot    : "	 << SlotManager::debug_slot(id)
		    << Logger::nltb	<< std::hex	<< "id      : 0x"<< std::setw(16) << std::setfill('0') << id << std::dec;
	}

	lgr << Logger::remt
		<< Logger::nltb << "}";

	return lgr;
}



} // namespace tgm