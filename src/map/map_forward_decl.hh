#ifndef GM_MAP_FORWARD_DECL_HH
#define GM_MAP_FORWARD_DECL_HH


#include <cstdint>
#include <memory>
#include <vector>


namespace tgm
{



using DataArrayId = uint64_t;


////
// A CityId uniquely identify a city in the map. 0 is reserved for empty values. 
////
using CityId = DataArrayId;

////
// A CityBlockId uniquely identify a CityBlock within a city. 0 is reserved for empty values. 
////
using CityBlockId = DataArrayId;

////
// A BuildingId uniquely identify a building in the map. 0 is reserved for empty values. 
////
using BuildingId = DataArrayId;

////
//	Uniquely identify a BuildingArea inside a building. 0 is reserved for empty values.
////
using BuildingAreaId = DataArrayId;

struct BuildingAreaCompleteId
{
    BuildingId bid = 0;
    BuildingAreaId aid = 0;
};
inline auto operator==(BuildingAreaCompleteId const lhs, BuildingAreaCompleteId const rhs) { return lhs.bid == rhs.bid && lhs.aid == rhs.aid; }

////
//	Uniquely identify a roof in the map. 0 is reserved for empty values.
////
using RoofId = DataArrayId;

////
// Uniquely identifies a Door in the map. 0 is reserved for empty values. 
////
using DoorId = DataArrayId;



////
// Uniquely identifies a Sprite in dynamic_vertices container. 
////
using SpriteId = uint64_t;



} //namespace tgm


#endif //GM_MAP_FORWARD_DECL_HH