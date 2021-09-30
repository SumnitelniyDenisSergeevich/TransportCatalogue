#pragma once
#include "geo.h"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <deque>
#include <set>


#include <iostream>
using namespace std;//delete

namespace Transport_Catalogue {
//namespace detail {
//struct StopFindResult {
//	std::string name;
//	Coordinates	coord;
//	std::unordered_map<std::string, uint32_t> otherstop_dist;
//};
//}// namespace detail
struct Stop {
	std::string name;
	Coordinates coordinates;
	std::set<std::string_view> buses;

	bool operator==(const Stop& other) const { // доработать
		return name == other.name;
	}
};
class StopHasher {							//доработать
public:
	size_t operator()(const Stop& stop) const{
		return static_cast<size_t>(stop.coordinates.lat);
	}
};
struct Bus {
	std::string name;
	bool circle_key;
	std::vector<const Stop*> bus_stops;
};
struct RoutInfoS {
	size_t rout_stops_count;
	size_t unic_rout_stop_count;
	double route_length;
	double curvature;
};
using DistFromTo = std::unordered_map<Stop, std::unordered_map<Stop, uint32_t, StopHasher>, StopHasher>;
class TransportCatalogue {
public:
	void AddRoute(const Bus& bus, std::vector<std::string> stops);
	void AddStop(const Stop& stop);

	const Bus* FindRoute(const std::string& bus_name) const;
	const Stop* FindStop(const std::string& stop_name) const;

	RoutInfoS RouteInfo(const std::string& bus_name) const;
	std::set<std::string_view> StopInfo(const std::string& stop_name) const;

	void SetDistanceBetweenStops(const Stop* from, const Stop* to, const uint32_t dist);
	double GetDistanceBetweenStops(const Stop* from, const Stop* to) const;

	size_t UnicStopsCount(const std::string& bus_name) const;

	void Print() {
		/*cout << endl << endl;
		for (auto [bus_name, bus] : busname_to_bus_) {
			cout << bus_name << " "s << bus->name << " "s << bus->circle_key << " "s;
			for (auto& temp : bus->bus_stops) {
				cout << temp->name << endl;
			}
			cout << endl << endl;
		}
		cout << "------------"s << endl;
		for (auto [stop_name, stop] : stopname_to_stop_) {
			
			cout << stop_name << stop->name << " "s << stop->coordinates.lat << " "s << stop->coordinates.lng << " "s;
			for (auto bus : stop->buses) {
				cout << bus << " "s;
			}
			cout << endl << endl;
		}*/
		for (auto [from, to_dist] : from_to_dist_) {
			cout << from.name << ": "s << endl;
			for (auto [to, dist] : to_dist) {
				cout << to.name << " " << dist << " , "s;
			}
			cout << endl<< "=================="s << endl;
		}

		system("pause");
	}
private:
	std::deque<Bus> buses_;
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
	std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
	DistFromTo from_to_dist_;
};
}//namespace Transport_Catalogue
