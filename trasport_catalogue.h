#pragma once

#include "geo.h"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <deque>

class HashInt {
public:
	size_t operator()(const int obj) const {
		return static_cast<size_t>(obj);
	}
};

struct Bus {
	std::string name;
	bool circle_key;
	std::vector<std::string> bus_stops;
};

struct BusStop {
	std::string name;
	Coordinates coordinates;
};

class TransportCatalogue {
public:
	void AddRoute(int, std::vector<std::string>&, bool);

	void AddBusStop(std::string bus_stop, Coordinates bus_coordinates);

	void FindRoute();

	void FindBusStop();

	void RouteInfo();
private:

	std::unordered_map<std::string, Coordinates, std::hash<std::string>> stop_to_coordinates_;    

	std::unordered_map<int, std::vector<std::string>, HashInt> bus_to_stops_;

	std::unordered_map<int, bool, HashInt> bus_to_cirkle_;
};