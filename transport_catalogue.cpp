#include "transport_catalogue.h"

using namespace std;

namespace transport_catalogue {
void TransportCatalogue::AddRoute(const Bus& bus, const std::vector<std::string> stops){

	buses_.push_back(move(bus));
	for (const auto& stop : stops) {
		for (const auto& busstop : stops_) {
			if (busstop.name == stop) {
				stop__by_buses_[&busstop].insert(buses_.back().name);
			}
		}
	}
	busname_to_bus_[buses_.back().name] = &buses_.back();
}
void TransportCatalogue::AddStop(const Stop& stop) {
	stops_.push_back(stop);
	stopname_to_stop_[stops_.back().name] = &stops_.back();
}

const Bus* TransportCatalogue::FindRoute(const string& bus_name) const {
	if (busname_to_bus_.count(bus_name) != 0) {
		return busname_to_bus_.at(bus_name);
	}
	return nullptr;
}
const Stop* TransportCatalogue::FindStop(const string& stop_name) const {
	if (stopname_to_stop_.count(stop_name) != 0) {
		return stopname_to_stop_.at(stop_name);
	}
	return nullptr;
}

RoutInfoS TransportCatalogue::RouteInfo(const string& bus_name) const {
	RoutInfoS rout_info;
	const Bus* bus = FindRoute(bus_name);
	if (!bus) {
		throw invalid_argument("bus is not found"s);
	}
	rout_info.rout_stops_count = bus->circle_key ? (bus->bus_stops.size() + 1) : ((bus->bus_stops.size() * 2) - 1);
	rout_info.unic_rout_stop_count = UnicStopsCount(bus_name);
	double route_length = 0.0;
	double geographical_distance = 0.0;
	for (auto iter = bus->bus_stops.begin(); iter + 1 != bus->bus_stops.end(); ++iter) {
		if (bus->circle_key) {
			route_length += GetDistanceBetweenStops((*iter), (*(iter + 1)));
			geographical_distance += ComputeDistance((*iter)->coordinates, (*(iter + 1))->coordinates);
		}
		else {
			route_length += GetDistanceBetweenStops((*iter), (*(iter + 1))) + GetDistanceBetweenStops((*(iter + 1)), (*iter));
			geographical_distance += ComputeDistance((*iter)->coordinates, (*(iter + 1))->coordinates) * 2;
		}
	}
	if (bus->circle_key) {
		route_length += GetDistanceBetweenStops((*(bus->bus_stops.end() - 1)), (*bus->bus_stops.begin()));
		geographical_distance += ComputeDistance((*(bus->bus_stops.end() - 1))->coordinates, (*bus->bus_stops.begin())->coordinates);
	}
	rout_info.route_length = route_length;
	rout_info.curvature = route_length / geographical_distance;
	return rout_info;
}

std::set<string_view> TransportCatalogue::StopInfo(const Stop* stop) const {
	if (stop__by_buses_.count(stop) != 0) {
		return stop__by_buses_.at(stop);
	}
	return {};
}

void TransportCatalogue::SetDistanceBetweenStops(const Stop* from, const Stop* to, const uint32_t dist) {
	from_to_dist_[*from][*to] = dist;
}

double TransportCatalogue::GetDistanceBetweenStops(const Stop* from, const Stop* to) const {
	if (from_to_dist_.at(*from).count(*to) != 0) {
		return from_to_dist_.at(*from).at(*to);
	}
	else if (from_to_dist_.at(*to).count(*from) != 0) {
		return from_to_dist_.at(*to).at(*from);
	}
	return ComputeDistance(from->coordinates, to->coordinates);
}

size_t TransportCatalogue::UnicStopsCount(const string& bus_name) const {
	std::unordered_set<string_view> unic_bus_stops;
	for (const auto& bus_stop : busname_to_bus_.at(bus_name)->bus_stops) {
		unic_bus_stops.insert(bus_stop->name);
	}
	return unic_bus_stops.size();
}
}