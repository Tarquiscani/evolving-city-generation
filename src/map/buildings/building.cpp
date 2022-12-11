#include "building.hh"
#include <algorithm>
#include "std_extensions/hash_functions.hh"
#include <unordered_set>

#include "utilities.hh"


namespace tgm
{


        
auto Building::compute_volume() const -> IntParallelepiped
{
    #if DYNAMIC_ASSERTS
        if (is_empty()) { throw std::runtime_error("Cannot compute the volume of an empty Building."); }
    #endif

    auto vol = IntParallelepiped{};

    for (auto const& [aid, a] : m_areas)
    {
        vol.combine(a.volume());
    }

    return vol;
}
        
auto Building::compute_surface() const -> int
{
    auto sfc = 0;

    for (auto const& [aid, a] : m_areas)
    {
        auto const vol = a.volume();
        sfc += vol.length * vol.width;
    }

    return sfc;
}

auto Building::create_area(AreaType const type, Vector3i const pos, Vector2i const dims, 
                           std::unordered_map<AreaType, AreaExpansionTemplate> const& bld_expTemplate) -> DataArray<BuildingArea>::DataArrayEl const&
{
    #if DYNAMIC_ASSERTS
        if (does_area_overlap({pos.x, pos.y, pos.z, dims.x, dims.y, 1})) { throw std::runtime_error("Areas cannot overlap"); }
        if (bld_expTemplate.find(type) == bld_expTemplate.cend()) { throw std::runtime_error("The expansion template doesn't contain the specified AreaType."); }
    #endif

    auto const& el = m_areas.create(type, pos, dims);

    #if BUILDEXP_DEBUGLOG
        BElog << Logger::nltb << "Added to Building: " << el.value;
    #endif

    return el;
}


void Building::add_externalDoor(Vector3i const pos)
{
    #if DYNAMIC_ASSERTS
        if (std::find(m_external_doors.cbegin(), m_external_doors.cend(), pos) != m_external_doors.cend())
        {
            throw std::runtime_error("Already added external door.");
        }
    #endif

    m_external_doors.push_back(pos);
}

void Building::remove_externalDoor(Vector3i const pos)
{
    auto it = std::find(m_external_doors.cbegin(), m_external_doors.cend(), pos);

    #if DYNAMIC_ASSERTS
        if (it == m_external_doors.cend()) { throw std::runtime_error("Unexistent external door."); }
    #endif

    m_external_doors.erase(it);
}

void Building::add_blindDoor(Vector3i const pos)
{
    #if DYNAMIC_ASSERTS
        if (std::find(m_blind_doors.cbegin(), m_blind_doors.cend(), pos) != m_blind_doors.cend())
        {
            throw std::runtime_error("Already added blind door.");
        }
    #endif

    m_blind_doors.push_back(pos);
}

void Building::remove_blindDoor(Vector3i const pos)
{
    auto it = std::find(m_blind_doors.cbegin(), m_blind_doors.cend(), pos);

    #if DYNAMIC_ASSERTS
        if (it == m_blind_doors.cend()) { throw std::runtime_error("Trying to remove an unexistent blind door."); }
    #endif

    m_blind_doors.erase(it);
}

bool Building::does_area_overlap(IntParallelepiped const vol) const
{
    // Neither the borders nor the inner area should overlap the inner area of other BuildingAreas.
    for (auto const& [aid, a] : m_areas)
    {
        if (!vol.intersect(a.inner_volume()).is_null()) { return true; }
    }

    return false;
}

void Building::remove_area(BuildingAreaId const aid)
{
    if(!m_areas.destroy(aid)) {	throw std::runtime_error("Tried to remove an already destroyed area from the building."); }
}

auto Building::select_candidateAreas(std::unordered_map<AreaType, AreaExpansionTemplate> const& bld_exptempl, std::mt19937 & gen) -> std::vector<AreaType>
{
    std::vector<AreaType> candidate_areas;

    for (auto const& [type, aet] : bld_exptempl)
    {
        if (are_areaPrerequisites_satisfied(aet))
        {
            candidate_areas.push_back(type);
        }
    }

    shuffle_candidateAreas(gen, candidate_areas);

    return candidate_areas;
}

//TODO: PERFORMANCE: Find a better algorithm for this function.
bool Building::are_areaPrerequisites_satisfied(AreaExpansionTemplate const& aet)
{
    for (auto const req : aet.m_required_areas)
    {
        //if the prerequisite area can't be found between building areas
        if (std::find_if( m_areas.cbegin(), m_areas.cend(), [req](auto const& el) { return req == el.value.type(); }) ==  m_areas.cend() )
        {
            return false;
        }
    }

    return true;
}

//TODO: Make this function sort the vector giving more priority to candidate areas of higher cost
void Building::shuffle_candidateAreas(std::mt19937 & gen, std::vector<AreaType> & careas)
{
    std::shuffle(careas.begin(), careas.end(), gen);
}

auto Building::find_replaceableAreas(BuildingId const bid, AreaType const atype, 
                                     std::unordered_map<AreaType, AreaExpansionTemplate> const& bld_expTemplate) const -> std::vector<BuildingAreaCompleteId>
{
    auto replaceable_areas = std::vector<BuildingAreaCompleteId>{};

    auto const& a_exptempl = bld_expTemplate.at(atype);

    for (auto const& [aid, area] : m_areas)
    {
        // If the current area belongs to the areas that the new area could replace
        if (std::find(a_exptempl.m_replaceable_areas.cbegin(), a_exptempl.m_replaceable_areas.cend(), area.type()) != a_exptempl.m_replaceable_areas.cend())
        {
            replaceable_areas.push_back({ bid, aid });
        }
    }

    return replaceable_areas;
}

auto operator<<(std::ofstream & ofs, Building const& b) -> std::ofstream &
{
    //TODO: 12: Finire i file stream di Building
    ofs << b.m_expansionTemplate_id << ' ' << b.m_cid << ' ' << b.m_cbid << '\n';
//	ofs << b.m_areas << '\n';//TODO: 12: Implementa il file stream operator per BuildingArea
    ofs << b.m_external_doors << '\n';
    ofs << b.m_blind_doors;

    return ofs;
}

auto operator>>(std::ifstream & ifs, Building & b) -> std::ifstream &
{
    ifs >> b.m_expansionTemplate_id >> b.m_cid >> b.m_cbid;
//	ifs >> b.m_areas; //TODO: 12: Implementa il file stream operator per BuildingArea
    ifs >> b.m_external_doors;
    ifs >> b.m_blind_doors;

    return ifs;
}

auto operator<<(Logger & lgr, Building const& b) -> Logger &
{
    lgr << "Building{"
        << Logger::addt
        << Logger::nltb << "Areas: " << b.m_areas
        << Logger::remt
        << Logger::nltb << "}";

    return lgr;
}



} //namespace tgm