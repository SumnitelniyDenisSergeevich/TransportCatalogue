#include "request_handler.h"

using namespace std;
namespace transport_catalogue {
	namespace transport_catalogue_output {
		void ProcessingRequests(json::Node& stat_requests, std::ostream& out, const TransportCatalogue& tc) {
			json::Array out_data;
			for (auto stat_request : stat_requests.AsArray()) {
				auto map_iter = stat_request.AsMap().find("type"s);
				if (map_iter->second.AsString() == "Stop"s) {
					out_data.push_back( detail::StatStop(stat_request, tc) );
				}
				if (map_iter->second.AsString() == "Bus"s) {
					out_data.push_back( detail::StatRoute(stat_request, tc) );
				}
			}
			json::Print(json::Document{ json::Node{out_data} }, out);
		}
		namespace detail {
			json::Node StatStop(json::Node& stop_node, const TransportCatalogue& tc) {
				json::Dict result;
				json::Dict stop_stat = stop_node.AsMap();
				std::string stop_name = stop_stat.find("name")->second.AsString();

				result["request_id"s] = json::Node{ stop_stat.find("id"s)->second.AsInt() };

				if (!tc.FindStop(stop_name)) {
					result["error_message"s] = json::Node{ "not found"s };
				}
				else {
					auto busstops = tc.StopInfo(tc.FindStop(stop_name));
					if (!busstops.size()) {
						result["buses"s] = json::Node{ json::Array{} };
					}
					else {
						json::Array buses;
						for (auto& stop : busstops) {
							buses.push_back(json::Node{ static_cast<string>(stop) });
						}
						
						result["buses"s] = json::Node{ buses };
					}
				}
				return json::Node{ result };
			}
			json::Node StatRoute(json::Node& route_node, const TransportCatalogue& tc) {
				json::Dict result;
				json::Dict route_stat = route_node.AsMap();
				std::string route_name = route_stat.find("name"s)->second.AsString();
				result["request_id"s] = json::Node{ route_stat.find("id"s)->second.AsInt() };
				if (!tc.FindRoute(route_name)) {
					result["error_message"s] = json::Node{ "not found"s };
				}
				else {
					RoutInfoS rout_info = tc.RouteInfo(route_name);
					result["curvature"s] = json::Node{ rout_info.curvature };
					result["route_length"s] = json::Node{ rout_info.route_length };
					result["stop_count"s] = json::Node{ static_cast<int>(rout_info.rout_stops_count) };
					result["unique_stop_count"s] = json::Node{ static_cast<int>(rout_info.unic_rout_stop_count) };
				}
				return json::Node{ result };
			}
		}//namespace detail

	}//namespace transport_catalogue_output
}//namespace transport_catalogue