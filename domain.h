#pragma once

#include "geo.h"

#include <string>
#include <vector>

struct RouteInfo {
	size_t rout_stops_count;
	size_t unic_rout_stop_count;
	double route_length;
	double curvature;
};

struct Stop {
	std::string name;
	geo::Coordinates coordinates;

	bool operator==(const Stop& other) const {
		return (this == &other);
	}
};


struct Bus {
	std::string name;
	bool circle_key = false;
	std::vector<const Stop*> bus_stops;
};
