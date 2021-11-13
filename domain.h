#pragma once
#include "geo.h"

#include <string>
#include <vector>

struct Stop {
	std::string name;
	geo::Coordinates coordinates;

	bool operator==(const Stop& other) const {
		return (this == &other);
	}
};

struct RouteInfo {
	size_t rout_stops_count;
	size_t unic_rout_stop_count;
	double route_length;
	double curvature;
};

struct RouteSettings {
	inline static size_t bus_wait_time;
	inline static double bus_velocity;
};

struct Bus {
	std::string name;
	bool circle_key;
	std::vector<const Stop*> bus_stops;
};
