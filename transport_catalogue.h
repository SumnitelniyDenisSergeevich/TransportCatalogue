#pragma once
#include "geo.h"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <deque>
#include <set>

#include <map>

namespace Transport_Catalogue {
	namespace detail {
		struct StopFindResult {
			std::string name;
			Coordinates	coord;
			std::unordered_map<std::string, uint32_t> otherstop_dist;
		};
	}// namespace detail
	struct Stop {
		std::string name;
		Coordinates coordinates;
		std::set<std::string_view> buses;
		std::unordered_map<std::string_view, uint32_t> otherstop_dist;
		bool operator==(const Stop& other) const { // доработать
			return ((name == other.name) && (coordinates.lat == other.coordinates.lat) && (coordinates.lng == other.coordinates.lng));
		}
	};
	class StopHasher {							//доработать
	public:
		size_t operator()(const Stop* stop) const {
			return static_cast<size_t>(stop->coordinates.lat);
		}
	};
	struct Bus {
		std::string name;
		bool circle_key;
		std::vector<Stop*> bus_stops;
	};
	struct RoutInfoS {
		size_t rout_stops_count;
		size_t unic_rout_stop_count;
		double route_length;
		double curvature;
	};
	using DistFromTo = std::map<const Stop*, std::map<const Stop*, uint32_t>>;//map to execute hasher like a problem
	class TransportCatalogue {
	public:
		void AddRoute(std::string& bus_name, const std::vector<std::string>& stops, bool circle_key);
		void AddStop(const std::string& stop_name, Coordinates& stop_coord);

		const Bus* FindRoute(const std::string& bus_name) const;
		const Stop* FindStop(const std::string& stop_name) const;

		RoutInfoS RouteInfo(const std::string& bus_name) const;
		std::set<std::string_view> StopInfo(const std::string& stop_name) const;

		void SetDistanceBetweenStops(const Stop* from, const Stop* to, const uint32_t dist);
		double GetDistanceBetweenStops(const Stop* from, const Stop* to) const;

		size_t UnicStopsCount(const std::string& bus_name) const;
	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
		std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
		DistFromTo from_to_dist_;
	};
}//namespace Transport_Catalogue