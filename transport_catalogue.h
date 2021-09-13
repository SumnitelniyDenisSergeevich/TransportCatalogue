#pragma once
#include "geo.h"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <deque>
#include <set>

struct StopFindResult {
	std::string name;
	Coordinates	coord;
	std::unordered_map<std::string, uint32_t> otherstop_dist;
};

struct BusStop {
	std::string name;
	Coordinates coordinates;
	std::set<std::string_view> buses;
	std::unordered_map<std::string_view, uint32_t> otherstop_dist;
};
struct Bus {
	std::string name;
	bool circle_key;
	std::vector<BusStop*> bus_stops;
	std::unordered_set<BusStop*> unic_bus_stops_;
};
struct RoutInfoS {
	size_t rout_stops_count;
	size_t unic_rout_stop_count;
	double route_length;
	double curvature;	
};

class TransportCatalogue {
public:
	void AddRoute(std::string&, std::vector<std::string>&, bool);
	void AddBusStop(std::string&, Coordinates&);

	const Bus* FindRoute(const std::string& bus_f) const;
	const BusStop* FindBusStop(const std::string& busstop) const;

	RoutInfoS RouteInfo(std::string& bus_f) const;
	std::set<std::string_view> BusStopInfo(std::string& busstop) const;

	void SetDistanceBetweenStops(std::string, std::string_view, uint32_t);

	double GetDistanceBetweenStops(std::string, std::string) const;
private:
	std::deque<Bus> buses_;
	std::deque<BusStop> busstops_;
	std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
	std::unordered_map<std::string_view, const BusStop*> stopname_to_stop_;
};