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
	busname_to_bus_[buses_.back().name] = &buses_.back();
}
void TransportCatalogue::AddBusStop(string& bus_stop, Coordinates& bus_coordinates){
	busstops_.push_back({ move(bus_stop),move(bus_coordinates) });
	stopname_to_stop_[busstops_.back().name] = &busstops_.back();
}

const Bus* TransportCatalogue::FindRoute(string& bus_f) const {
	if (busname_to_bus_.count(bus_f)) {
		return busname_to_bus_.at(bus_f);
	}
	return nullptr;
}
 const BusStop* TransportCatalogue::FindBusStop(string& busstop_f) const{
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

	for (auto iter = bus->bus_stops.begin(); iter + 1 != bus->bus_stops.end();++iter) {
		if (bus->circle_key) {
			route_length += ComputeDistance((*iter)->coordinates, (*(iter + 1))->coordinates);
		}
		else {
			route_length += ComputeDistance((*iter)->coordinates, (*(iter + 1))->coordinates) * 2;
		}
	}
	if (bus->circle_key) {
		route_length += ComputeDistance((*bus->bus_stops.begin())->coordinates, (*(bus->bus_stops.end()-1))->coordinates);
	}
	routinfo.route_length = route_length;
	return routinfo;
}