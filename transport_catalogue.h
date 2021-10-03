#pragma once
#include "geo.h"
#include "json.h"

#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <set>

#include <iostream> // delete
#include <cassert>//delete
using namespace std; // delete

namespace transport_catalogue {
	struct Stop {
		std::string name;
		geo::Coordinates coordinates;

		bool operator==(const Stop& other) const {
			return (this == &other);
		}
	};
	class StopHasher {
	public:
		size_t operator()(const Stop* stop) const {
			return static_cast<size_t>(stop->coordinates.lat * 17 + stop->coordinates.lng * 17 * 17);
		}
	};
	struct Bus {
		std::string name;
		bool circle_key;
		std::vector<const Stop*> bus_stops;
	};
	struct RoutInfoS {
		size_t rout_stops_count;
		size_t unic_rout_stop_count;
		double route_length;
		double curvature;
	};
	using DistFromTo = std::unordered_map<const Stop*, std::unordered_map<const Stop*, uint32_t, StopHasher>, StopHasher>;
	class TransportCatalogue {
	public:
		void AddRoute(Bus& bus, const json::Array stops);
		void AddStop(const Stop& stop);

		const Bus* FindRoute(const std::string& bus_name) const;
		const Stop* FindStop(const std::string& stop_name) const;

		RoutInfoS RouteInfo(const std::string& bus_name) const;
		std::set<std::string_view> StopInfo(const Stop* stop) const;

		void SetDistanceBetweenStops(const Stop* from, const Stop* to, const uint32_t dist);
		double GetDistanceBetweenStops(const Stop* from, const Stop* to) const;

		size_t UnicStopsCount(const std::string& bus_name) const;
	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
		std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
		std::unordered_map<const Stop*, std::set<std::string_view>> stop__by_buses_; 
		DistFromTo from_to_dist_;
	};
}//namespace transport_catalogue
