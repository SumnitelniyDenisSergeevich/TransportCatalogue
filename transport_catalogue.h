#pragma once
#include "domain.h"

#include <unordered_map>
#include <deque>
#include <set>
#include <map>
#include <string>
#include <vector>


namespace transport_catalogue {
	class StopHasher {
	public:
		size_t operator()(const Stop* stop) const {
			return static_cast<size_t>(stop->coordinates.lat * 17 + stop->coordinates.lng * 17 * 17);
		}
	};
	using DistFromTo = std::unordered_map<const Stop*, std::unordered_map<const Stop*, uint32_t, StopHasher>, StopHasher>;
	class TransportCatalogue {
	public:
		void AddRoute(Bus& bus, const std::vector<std::string> stops);
		void AddStop(const Stop& stop);

		const Bus* FindRoute(const std::string& bus_name) const;
		const Stop* FindStop(const std::string& stop_name) const;

		void SetDistanceBetweenStops(const Stop* from, const Stop* to, const uint32_t dist);

		RouteInfo GetRouteInfo(const std::string& bus_name) const;
		std::set<std::string_view> GetStopInfo(const Stop* stop) const;
		double GetDistanceBetweenStops(const Stop* from, const Stop* to) const;
		const std::map<std::string_view, const Stop*>& GetStopNameToStop() const;
		const std::map<std::string_view, const Bus*>& GetBusNameToBus() const;
		size_t GetStopsCount() const;
		const std::deque<Bus>& GetBuses() const;
		const std::deque<Stop>& GetStops() const;
		std::pair<double,size_t> GetDistAndSpanCountBetweenStopsInRoute(const Bus& route, const size_t from, const size_t to, bool last_ring = false) const;

		size_t UnicStopsCount(const std::string& bus_name) const;
	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;
		std::map<std::string_view, const Bus*> busname_to_bus_;
		std::map<std::string_view, const Stop*> stopname_to_stop_;
		std::unordered_map<const Stop*, std::set<std::string_view>> stop_by_buses_; 
		DistFromTo from_to_dist_;
	};
}//namespace transport_catalogue
