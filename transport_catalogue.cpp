#include "transport_catalogue.h"

#include <stdexcept>
#include <unordered_set>

using namespace std;

namespace transport_catalogue {
	void TransportCatalogue::AddRoute( Bus& bus, const vector<string> stops) {
		buses_.push_back(move(bus));
		for (const auto& stop : stops) {
			for (const auto& busstop : stops_) {
				if (busstop.name == stop) {
					stop_by_buses_[&busstop].insert(buses_.back().name);
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

	RouteInfo TransportCatalogue::GetRouteInfo(const string& bus_name) const {
		::RouteInfo rout_info;
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
				geographical_distance += geo::ComputeDistance((*iter)->coordinates, (*(iter + 1))->coordinates);
			}
			else {
				route_length += GetDistanceBetweenStops((*iter), (*(iter + 1))) + GetDistanceBetweenStops((*(iter + 1)), (*iter));
				geographical_distance += geo::ComputeDistance((*iter)->coordinates, (*(iter + 1))->coordinates) * 2;
			}
		}
		if (bus->circle_key) {
			route_length += GetDistanceBetweenStops((*(bus->bus_stops.end() - 1)), (*bus->bus_stops.begin()));
			geographical_distance += geo::ComputeDistance((*(bus->bus_stops.end() - 1))->coordinates, (*bus->bus_stops.begin())->coordinates);
		}
		rout_info.route_length = route_length;
		rout_info.curvature = route_length / geographical_distance;
		return rout_info;
	}

	set<string_view> TransportCatalogue::GetStopInfo(const Stop* stop) const {
		if (stop_by_buses_.count(stop) != 0) {
			return stop_by_buses_.at(stop);
		}
		return {};
	}

	void TransportCatalogue::SetDistanceBetweenStops(const Stop* from, const Stop* to, const uint32_t dist) {
		from_to_dist_[from][to] = dist;
	}

	double TransportCatalogue::GetDistanceBetweenStops(const Stop* from, const Stop* to) const {
		if (from_to_dist_.count(from) != 0) {
			if (from_to_dist_.at(from).count(to) != 0) {
				return from_to_dist_.at(from).at(to);
			}
		}
		if (from_to_dist_.count(to) != 0) {
			if (from_to_dist_.at(to).count(from) != 0) {
				return from_to_dist_.at(to).at(from);
			}
		}
		return geo::ComputeDistance(from->coordinates, to->coordinates);
	}

	pair<double, size_t> TransportCatalogue::GetDistAndSpanCountBetweenStopsInRoute(const std::vector<const Stop*>& route_stops,
																	const size_t from, const size_t to, bool last_ring) const {
		auto station_from = route_stops.begin() + from;
		auto station_to = route_stops.begin() + to;
		double dist = 0.0;
		size_t span_count = 0;

		if (last_ring) {
			for (; station_from + 1 < route_stops.end(); ++station_from) {
				dist += GetDistanceBetweenStops(*station_from, *next(station_from, 1));
				++span_count;
			}
			dist += GetDistanceBetweenStops(*station_from, *route_stops.begin());
			++span_count;
			return { dist / 1000.0, span_count };
		}

		if (station_from < station_to) {
			for (; station_from < station_to; ++station_from) {
				dist += GetDistanceBetweenStops(*station_from, *next(station_from, 1));
				++span_count;
			}
		}
		else if (station_from > station_to) {
			for (; station_from > station_to; --station_from) {
				dist += GetDistanceBetweenStops(*station_from, *next(station_from, -1));
				++span_count;
			}
		}
		return { dist / 1000.0, span_count };
	}

	size_t TransportCatalogue::UnicStopsCount(const string& bus_name) const {
		unordered_set<string_view> unic_bus_stops;
		for (const auto& bus_stop : busname_to_bus_.at(bus_name)->bus_stops) {
			unic_bus_stops.insert(bus_stop->name);
		}
		return unic_bus_stops.size();
	}

	const std::map<std::string_view, const Stop*>& TransportCatalogue::GetStopNameToStop() const {
		return stopname_to_stop_;
	}
	const std::map<std::string_view, const Bus*>& TransportCatalogue::GetBusNameToBus() const {
		return busname_to_bus_;
	}
	size_t TransportCatalogue::GetStopsCount() const {
		return stops_.size();
	}
	const std::deque<Bus>& TransportCatalogue::GetBuses() const {
		return buses_;
	}
	const std::deque<Stop>& TransportCatalogue::GetStops() const {
		return stops_;
	}
}