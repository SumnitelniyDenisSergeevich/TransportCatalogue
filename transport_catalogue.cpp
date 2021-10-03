#include "transport_catalogue.h"
#include <stdexcept>
using namespace std;

namespace Transport_Catalogue {
	void TransportCatalogue::AddRoute(string& bus, const vector<string>& stops, bool circle_key) {
		Bus add_bus;
		add_bus.name = move(bus);
		add_bus.circle_key = circle_key;
		for (auto& stop : stops) {
			for (auto& busstop : stops_) {
				if (busstop.name == stop) {
					add_bus.bus_stops.push_back(&busstop);
				}
			}
		}
		buses_.push_back(move(add_bus));
		for (auto& stop : stops) {
			for (auto& busstop : stops_) {
				if (busstop.name == stop) {
					busstop.buses.insert(buses_.back().name);
				}
			}
		}
		busname_to_bus_[buses_.back().name] = &buses_.back();
	}
	void TransportCatalogue::AddStop(const std::string& name, Coordinates& coord) {
		stops_.push_back({ name, move(coord), {}, {} });
		stopname_to_stop_[stops_.back().name] = &stops_.back();
	}

	const Bus* TransportCatalogue::FindRoute(const string& bus_f) const {
		if (busname_to_bus_.count(bus_f) != 0) {
			return busname_to_bus_.at(bus_f);
		}
		return nullptr;
	}
	const Stop* TransportCatalogue::FindStop(const string& busstop_f) const {
		if (stopname_to_stop_.count(busstop_f) != 0) {
			return stopname_to_stop_.at(busstop_f);
		}
		return nullptr;
	}

	RoutInfoS TransportCatalogue::RouteInfo(const string& bus_f) const {
		RoutInfoS routinfo;
		const Bus* bus = FindRoute(bus_f);
		if (!bus) {
			throw invalid_argument("bus is not found"s);
		}
		routinfo.rout_stops_count = bus->circle_key ? (bus->bus_stops.size() + 1) : ((bus->bus_stops.size() * 2) - 1);
		routinfo.unic_rout_stop_count = UnicStopsCount(bus_f);
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
		routinfo.route_length = route_length;
		routinfo.curvature = route_length / geographical_distance;
		return routinfo;
	}

	std::set<string_view> TransportCatalogue::StopInfo(const std::string& busstop) const {
		return stopname_to_stop_.at(busstop)->buses;
	}

	void TransportCatalogue::SetDistanceBetweenStops(const Stop* from, const Stop* to, const uint32_t dist) {
		if (from && to) {
			from_to_dist_[from][to] = dist;
		}
	}

	double TransportCatalogue::GetDistanceBetweenStops(const Stop* from, const Stop* to) const {
		if (from_to_dist_.at(from).count(to) != 0) {
			return from_to_dist_.at(from).at(to);
		}
		else if (from_to_dist_.at(to).count(from) != 0) {
			return from_to_dist_.at(to).at(from);
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