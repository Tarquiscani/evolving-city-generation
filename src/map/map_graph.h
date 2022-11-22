#ifndef GM_MAP_GRAPH_H
#define GM_MAP_GRAPH_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <utility>

#include "std_extensions/hash_functions.hh"
#include "map_forward_decl.hh"


//TODO: 02: Completely wrong with 64bit BuildingId
//struct HashBuildingInfos
//{
//	auto operator()(std::pair<OldBuildingId, OldBuildingAreaId> const& p) const -> std::size_t
//	{
//		int a = p.first;
//		int b = p.second;
//
//		std::size_t seed = 0;
//
//		seed ^= std::hash<int>{}(a)  + 0x9e3779b9 + (seed << 6) + (seed >> 2);  //same as boost::hash_combine()
//		seed ^= std::hash<int>{}(b) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//
//		return seed;
//	}
//};

//struct NodeConnectionInfo
//{
//	std::unordered_set<std::pair<OldBuildingId, OldBuildingAreaId>, HashBuildingInfos> edges;
//	DoorId did = 0;
//
//	void add_edge_to(OldBuildingId bid, OldBuildingAreaId aid, DoorId did_)
//	{
//		auto[iter, already_existent] = edges.insert({ bid, aid });
//		if (already_existent)
//			throw std::runtime_error("The connection was already added.");
//		did = did_;
//	}
//};
//
//class MapGraph
//{
//	public:
//		void add_connection(OldBuildingId bid1, OldBuildingAreaId aid1, OldBuildingId bid2, OldBuildingAreaId aid2, DoorId did) 
//		{
//			{
//				auto[it, ex] = connections.insert({ { bid1, aid1 }, {} });
//				it->second.add_edge_to(bid2, aid2, did);
//			}
//			{
//				auto[it, ex] = connections.insert({ { bid2, aid2 }, {} });
//				it->second.add_edge_to(bid1, aid1, did);
//			}
//		}
//		void remove_connection(OldBuildingId bid1, OldBuildingAreaId aid1, OldBuildingId bid2, OldBuildingAreaId aid2)
//		{
//
//		}
//
//	private:
//
//		std::unordered_map<std::pair<OldBuildingId, OldBuildingAreaId>, 
//						   NodeConnectionInfo,
//						   HashBuildingInfos> connections;
//};


#endif //GM_MAP_GRAPH_H