#ifndef GM_BUILDING_HH
#define GM_BUILDING_HH


#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <tuple>
#include <unordered_map>

#include "data_strctures/data_array.hh"
#include "map/buildings/area_expansion_template.hh"
#include "map/buildings/area_template.hh"
#include "map/buildings/building_area.hh"
#include "map/map_forward_decl.hh"
#include "system/parallelepiped.hh"

#include "debug/logger/log_streams.hh"


namespace tgm
{



//class ExpansionProposal
//{
//    public:
//        ExpansionProposal(BuildingArea new_area_) : new_area(new_area_) { }
//        ExpansionProposal(BuildingArea new_area_, std::unique_ptr<std::vector<BuildingArea>>&& old_areas_) :
//            replacement(true), new_area(new_area_), old_areas(old_areas_)) { }
//
//    private:
//        bool replacement = false;
//        BuildingArea new_area;
//        std::unique_ptr<std::vector<BuildingArea>> old_areas;
//};


class Building
{
    public:		
        ////
        //	@exptemp_id: Must be the id of an existent BuildingExpansionTemplate
        ////
        Building(BuildingExpansionTemplateId const exptemp_id, CityId const cid, CityBlockId const cbid):
            m_expansionTemplate_id{ exptemp_id }, m_cid{ cid }, m_cbid{ cbid } { }


        auto expTempl_id() const { return m_expansionTemplate_id; }
        
        auto cid() const -> CityId;
        auto cbid() const -> CityBlockId;

        auto areas_by_ref() const -> DataArray<BuildingArea, true> const& { return m_areas; }
        auto getOrThrow_area(BuildingAreaId const aid) const -> BuildingArea const& { return m_areas.get_or_throw(aid); };
        bool is_empty() const noexcept { return m_areas.count() == 0; } 

        auto compute_volume() const -> IntParallelepiped;
        auto compute_surface() const -> int;

        auto const& external_doors() const { return m_external_doors; }

        ////
        //	Create a new area.
        //	@return: The index of the area in the "areas" vector and the area itself.
        ////
        auto create_area(AreaType const type, Vector3i const pos, Vector2i const dims, 
                         std::unordered_map<AreaType, AreaExpansionTemplate> const& bld_expTemplate) -> DataArray<BuildingArea>::DataArrayEl const&;
        void remove_area(BuildingAreaId const aid);

        void add_externalDoor(Vector3i const pos);
        void remove_externalDoor(Vector3i const pos);
        
        void add_blindDoor(Vector3i const pos);
        void remove_blindDoor(Vector3i const pos);


        ////
        //  @return: Boolean indicating if the candidate areas could reuse an abandoned area. Integer indicating the power. 
        //           Pointer to a vector of candidate areas.
        ////
        auto propose_expansion(std::unordered_map<AreaType, AreaExpansionTemplate> const& bld_exptempl, std::mt19937 & gen) -> std::tuple<bool, int, std::vector<AreaType>>
        {
            std::tuple<bool, int, std::vector<AreaType>> ret; //NRVO
            auto & [reuse, power, candidate_areas] = ret;
            reuse = false;
            power = 1;

            candidate_areas = select_candidateAreas(bld_exptempl, gen);

            #if BUILDEXP_DEBUGLOG
                BElog << Logger::nltb << "Potential Areas: " << bld_exptempl;
                BElog << Logger::nltb << "Proposed candidate areas: " << candidate_areas;
            #endif

            return ret;
        }

        auto find_replaceableAreas(BuildingId const bid, AreaType const atype, 
                                   std::unordered_map<AreaType, AreaExpansionTemplate> const& bld_expTemplate) const -> std::vector<BuildingAreaCompleteId>;


    private:
        BuildingExpansionTemplateId m_expansionTemplate_id = 0;

        CityId m_cid = 0;
        CityBlockId m_cbid = 0;
        DataArray<BuildingArea, true> m_areas{ 10u }; 

        std::vector<Vector3i> m_external_doors;
        std::vector<Vector3i> m_blind_doors;

        bool does_area_overlap(IntParallelepiped const vol) const;
        
        auto select_candidateAreas(std::unordered_map<AreaType, AreaExpansionTemplate> const& bld_exptempl, std::mt19937 & gen) -> std::vector<AreaType>;
        bool are_areaPrerequisites_satisfied(AreaExpansionTemplate const& pa);
        void shuffle_candidateAreas(std::mt19937 & gen, std::vector<AreaType> & careas);

        

    friend auto operator<<(std::ofstream & ofs, Building const& b) -> std::ofstream &;
    friend auto operator>>(std::ifstream & ifs, Building & b) -> std::ifstream &;

    friend auto operator<<(Logger & lgr, Building const& b) -> Logger &;
};



inline auto Building::cid() const -> CityId 
{ 
    #if DYNAMIC_ASSERTS
        if(m_cid == 0) { throw std::runtime_error("This building doesn't belong to a block."); }
    #endif

    return m_cid;
}
inline auto Building::cbid() const -> CityBlockId 
{
    #if DYNAMIC_ASSERTS
        if(m_cbid == 0) { throw std::runtime_error("This building doesn't belong to a block."); }
    #endif

    return m_cbid; 
}



} //namespace tgm


#endif // GM_BUILDING_HH