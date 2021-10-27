#include "json_reader.h"
#include <iostream> // delete
#include <map>
using namespace std;


JsonReader::JsonReader(std::istream& in) : doc_(json::Load(in)) {
}

void JsonReader::FillCatalogue(transport_catalogue::TransportCatalogue& tc) {
    auto iter = doc_.GetRoot().AsMap().at("base_requests"s);
    transport_catalogue::transport_catalogue_input::FillCatalog(iter, tc);
}

json::Dict JsonReader::GetRequests() {
	json::Dict requests = doc_.GetRoot().AsMap();
	return requests;
}

namespace transport_catalogue {
    namespace transport_catalogue_input {
        void FillCatalog(Node& base_requests, TransportCatalogue& tc) {
            Array bus_requests;
            unordered_map<string,Dict> from_to_dist;
            for (auto base_request : base_requests.AsArray()) {// вектор запросов, который содержит map
                auto map_iter = base_request.AsMap().find("type");
                if (map_iter->second.AsString() == "Stop"s) {
                    tc.AddStop(detail::FillStop(base_request, from_to_dist));
                }
                if (map_iter->second.AsString() == "Bus"s) {
                    bus_requests.push_back(base_request);
                }
            }

            for (auto bus_request : bus_requests) {
                auto [bus, stops] = detail::FillRoute(bus_request, tc);
                tc.AddRoute(bus, stops);
            }
            for (auto& [from, to_dist] : from_to_dist) {
                for (auto& [to, dist] : to_dist) {
                    tc.SetDistanceBetweenStops(tc.FindStop(from), tc.FindStop(to), std::abs(dist.AsInt()));
                }
            }
        }
        namespace detail {
            std::pair<Bus, vector<string>> FillRoute(Node& node_bus, TransportCatalogue& tc) {
                Bus result;
                result.name = node_bus.AsMap().at("name"s).AsString();
                result.circle_key = node_bus.AsMap().at("is_roundtrip"s).AsBool();
                Array array = node_bus.AsMap().at("stops"s).AsArray();
				vector<string> array_str;
				for (auto& stop_str : array) {
					array_str.push_back(stop_str.AsString());
				}
                if (result.circle_key) {
					array_str.pop_back();
                }
                for (auto& stop_str : array_str) {
                    if (const Stop* stop = tc.FindStop(stop_str); stop) {
                        result.bus_stops.push_back(stop);
                    }
                }
                return { result, array_str };
            }
            Stop FillStop(const Node& node_stop, unordered_map<string, Dict>& from_to_dist) {
                Stop result;
                result.name = node_stop.AsMap().at("name"s).AsString();
                result.coordinates.lat = node_stop.AsMap().at("latitude"s).AsDouble();
                result.coordinates.lng = node_stop.AsMap().at("longitude"s).AsDouble();
                from_to_dist[result.name] = node_stop.AsMap().at("road_distances"s).AsMap();
                return result;
            }

        }// namespace detail
    }// namespace transport_catalogue_input


}// namespace transport_catalogue