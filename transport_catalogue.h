#pragma once
#include "geo.h"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <deque>

struct BusStop {
	std::string name;
	Coordinates coordinates;
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
};

class TransportCatalogue {
public:
	void AddRoute(std::string&, std::vector<std::string>&, bool);
	void AddBusStop(std::string& bus_stop, Coordinates& bus_coordinates);

	const Bus* FindRoute(std::string& bus_f) const;
	const BusStop* FindBusStop(std::string& busstop) const;

	RoutInfoS RouteInfo(std::string& bus_f) const;
private:
	std::deque<Bus> buses_;
	std::deque<BusStop> busstops_;
	std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
	std::unordered_map<std::string_view, const BusStop*> stopname_to_stop_;
};