#include "transport_catalogue.h"

using namespace std;

void TransportCatalogue::AddRoute(string& bus, vector<string>& stops, bool circle_key) {
	Bus add_bus;
	add_bus.name = move(bus);
	add_bus.circle_key = circle_key;
	for (auto& stop : stops) {
		for (auto& busstop : busstops_) {
			if (busstop.name == stop) {
				add_bus.bus_stops.push_back(&busstop);
				add_bus.unic_bus_stops_.insert(&busstop);
			}
		}
	}
	buses_.push_back(move(add_bus));
	for (auto& stop : stops) {
		for (auto& busstop : busstops_) {
			if (busstop.name == stop) {
				busstop.buses.insert(buses_.back().name);
			}
		}
	}
	busname_to_bus_[buses_.back().name] = &buses_.back();
}
void TransportCatalogue::AddBusStop(std::string& name , Coordinates& coord){
	busstops_.push_back({name,move(coord),{},{} });
	stopname_to_stop_[busstops_.back().name] = &busstops_.back();
}

const Bus* TransportCatalogue::FindRoute(const string& bus_f) const {
	if (busname_to_bus_.count(bus_f)) {
		return busname_to_bus_.at(bus_f);
	}
	return nullptr;
}
 const BusStop* TransportCatalogue::FindBusStop(const string& busstop_f) const{
	 if (stopname_to_stop_.count(busstop_f)) {
		 return stopname_to_stop_.at(busstop_f);
	 }
	 return nullptr;
}

RoutInfoS TransportCatalogue::RouteInfo(string& bus_f) const {
	RoutInfoS routinfo;
	const Bus* bus = FindRoute(bus_f);
	routinfo.rout_stops_count = bus->circle_key ? (bus->bus_stops.size() + 1) : ((bus->bus_stops.size() * 2) - 1);
	routinfo.unic_rout_stop_count = bus->unic_bus_stops_.size();
	double route_length = 0.0;
	double geographical_distance = 0.0;
	for (auto iter = bus->bus_stops.begin(); iter + 1 != bus->bus_stops.end();++iter) {
		if (bus->circle_key) {
			route_length += GetDistanceBetweenStops((*iter)->name, (*(iter + 1))->name);
			geographical_distance += ComputeDistance((*iter)->coordinates, (*(iter + 1))->coordinates);
		}else{
			route_length += GetDistanceBetweenStops((*iter)->name, (*(iter + 1))->name) + GetDistanceBetweenStops((*(iter + 1))->name, (*iter)->name);
			geographical_distance += ComputeDistance((*iter)->coordinates, (*(iter + 1))->coordinates) * 2;
		}
	}

	if (bus->circle_key) {
		route_length += GetDistanceBetweenStops((*(bus->bus_stops.end() - 1))->name, (*bus->bus_stops.begin())->name);
		geographical_distance += ComputeDistance((*(bus->bus_stops.end() - 1))->coordinates, (*bus->bus_stops.begin())->coordinates);
	}
	routinfo.route_length = route_length;
	routinfo.curvature = route_length / geographical_distance;
	return routinfo;
}

std::set<string_view> TransportCatalogue::BusStopInfo(std::string& busstop) const {
	return stopname_to_stop_.at(busstop)->buses;
}

void TransportCatalogue::SetDistanceBetweenStops(std::string from, std::string_view to, uint32_t dist) {
	for (auto& stop : busstops_) {
		if (from == stop.name) {
			stop.otherstop_dist[to] = dist;
			break;
		}
	}
}

double TransportCatalogue::GetDistanceBetweenStops(std::string from, std::string to) const {
	if (stopname_to_stop_.at(from)->otherstop_dist.count(to)) {
		return stopname_to_stop_.at(from)->otherstop_dist.at(to);
	}
	else if(stopname_to_stop_.at(to)->otherstop_dist.count(from)){
		return stopname_to_stop_.at(to)->otherstop_dist.at(from);
	}
	return ComputeDistance(stopname_to_stop_.at(from)->coordinates, stopname_to_stop_.at(to)->coordinates);
}