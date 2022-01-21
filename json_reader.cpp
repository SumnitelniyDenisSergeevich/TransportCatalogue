#include "json_reader.h" 
#include <sstream>
using namespace std;


JsonReader::JsonReader(istream& in) : doc_(json::Load(in)) {
}

void JsonReader::FillCatalogue(transport_catalogue::TransportCatalogue& tc, const transport_catalogue_serialization::BaseRequests& base_requests_ser) const {
    transport_catalogue::transport_catalogue_input::FillCatalog(tc, base_requests_ser);
}

void JsonReader::FillRenderSettings(renderer::MapRender& map, const transport_catalogue_serialization::RenderSettings& render_settings_ser) const {
	map.SetWidth(render_settings_ser.width());
	map.SetHeight(render_settings_ser.height());
	map.SetPadding(render_settings_ser.padding());
	map.SetLineWidth(render_settings_ser.line_width());
	map.SetStopRadius(render_settings_ser.stop_radius());
	map.SetBusLableFontSize(render_settings_ser.bus_label_font_size());
	map.SetBusLableOffset(render_settings_ser.bus_label_offset(0), render_settings_ser.bus_label_offset(1));
	map.SetStopLableFontSize(render_settings_ser.stop_label_font_size());
	map.SetStopLableOffset(render_settings_ser.stop_label_offset(0), render_settings_ser.stop_label_offset(1));
	map.SetUnderLayerWidth(render_settings_ser.underlayer_width());

	transport_catalogue_serialization::Color underlayer_color = render_settings_ser.underlayer_color();
	if (underlayer_color.has_rgb_color()) {
		map.SetUnderLayerColor(svg::Rgb{ static_cast<uint8_t>(underlayer_color.rgb_color().red()),
										 static_cast<uint8_t>(underlayer_color.rgb_color().green()),
										 static_cast<uint8_t>(underlayer_color.rgb_color().blue())
										});
	}
	else if (underlayer_color.has_rgba_color()) {
		map.SetUnderLayerColor(svg::Rgba{ static_cast<uint8_t>(underlayer_color.rgba_color().red()),
										 static_cast<uint8_t>(underlayer_color.rgba_color().green()),
										 static_cast<uint8_t>(underlayer_color.rgba_color().blue()),
										 underlayer_color.rgba_color().opacity()
			});
	}
	else {
		map.SetUnderLayerColor(underlayer_color.string_color().color());
	}
	vector<svg::Color> palette_colors;
	for (size_t i = 0; i < render_settings_ser.palette_size(); ++i) {
		transport_catalogue_serialization::Color palette_color = render_settings_ser.palette(i);
		if (palette_color.has_rgb_color()) {
			palette_colors.push_back(svg::Rgb{ static_cast<uint8_t>(palette_color.rgb_color().red()),
											 static_cast<uint8_t>(palette_color.rgb_color().green()),
											 static_cast<uint8_t>(palette_color.rgb_color().blue())
				});
		}
		else if (palette_color.has_rgba_color()) {
			palette_colors.push_back(svg::Rgba{ static_cast<uint8_t>(palette_color.rgba_color().red()),
											 static_cast<uint8_t>(palette_color.rgba_color().green()),
											 static_cast<uint8_t>(palette_color.rgba_color().blue()),
											 palette_color.rgba_color().opacity()
				});
		}
		else {
			palette_colors.push_back({ palette_color.string_color().color() });
		}

	}
	map.SetColorPalette(palette_colors);
}

void JsonReader::FillRouteSettings(TransportRouter& t_r, const transport_catalogue_serialization::RoutingSettings& routing_settings_ser)const {
	t_r.SetRouteSettings(routing_settings_ser.bus_velocity(), routing_settings_ser.bus_wait_time());
}

void JsonReader::PrintRequestsAnswer(ostream& out) const {
	json::Array stat_requests = doc_.GetRoot().AsMap().at("stat_requests"s).AsArray();
	json::Array out_data;
	for (auto stat_request : stat_requests) {
		auto map_iter = stat_request.AsMap().find("type"s);
		if (map_iter->second.AsString() == "Stop"s) {
			out_data.push_back(GetStatStop(stat_request));
		}
		if (map_iter->second.AsString() == "Bus"s) {
			out_data.push_back(GetStatRoute(stat_request));
		}
		if (map_iter->second.AsString() == "Map"s) {
			json::Dict result;
			result["request_id"s] = json::Node{ stat_request.AsMap().find("id"s)->second.AsInt() };
			ostringstream out;
			rh_->RenderMap().Render(out);
			string rendered_map = out.str();
			result["map"s] = json::Node{ rendered_map };
			out_data.push_back(json::Node{ result });
		}
		if (map_iter->second.AsString() == "Route") {											// обработка route  new
			out_data.push_back(GetRoute(stat_request));
		}
	}
	json::Print(json::Document{ json::Node{out_data} }, out);
}

json::Node JsonReader::GetStatStop(const json::Node& stop_node) const {
	json::Dict result;
	json::Dict stop_stat = stop_node.AsMap();
	string stop_name = stop_stat.find("name")->second.AsString();

	result["request_id"s] = json::Node{ stop_stat.find("id"s)->second.AsInt() };
	auto buses = rh_->GetBusesByStop(stop_name);
	if(!buses.has_value()) {
		result["error_message"s] = json::Node{ "not found"s };
	}
	else {
		if (!buses->size()) {
			result["buses"s] = json::Node{ json::Array{} };
		}
		else {
			json::Array json_buses;
			for (auto& bus : buses.value()) {
				json_buses.push_back(json::Node{ static_cast<string>(bus) });
			}
			result["buses"s] = json::Node{ json_buses };
		}
	}
	return result;
}

json::Node JsonReader::GetStatRoute(const json::Node& route_node) const {
	json::Dict result;
	json::Dict route_stat = route_node.AsMap();
	string route_name = route_stat.find("name"s)->second.AsString();
	result["request_id"s] = json::Node{ route_stat.find("id"s)->second.AsInt() };
	auto rout_info = rh_->GetBusStat(route_name);
	if (!rout_info.has_value()) {
		result["error_message"s] = json::Node{ "not found"s };
	}
	else {
		result["curvature"s] = json::Node{ rout_info->curvature };
		result["route_length"s] = json::Node{ rout_info->route_length };
		result["stop_count"s] = json::Node{ static_cast<int>(rout_info->rout_stops_count) };
		result["unique_stop_count"s] = json::Node{ static_cast<int>(rout_info->unic_rout_stop_count) };
	}
	return result;
}

json::Node JsonReader::GetRoute(const json::Node& route_node) const {			//new
	json::Dict result;
	json::Dict route_stat = route_node.AsMap();
	string route_from = route_stat.find("from"s)->second.AsString();
	string route_to = route_stat.find("to"s)->second.AsString();
	result["request_id"s] = json::Node{ route_stat.find("id"s)->second.AsInt() };
	std::optional<graph::Router<double>::RouteInfo> route = rh_->BuildRoute(t_router_->GetVertexId(route_from),
		t_router_->GetVertexId(route_to));
	if (route.has_value()) {
		json::Array wastes_time;
		for (auto edge : route->edges) {
			json::Dict waste_time_stop;
			json::Dict waste_time_bus;
			auto edge_r = t_router_->GetEdge(edge);
			waste_time_stop["stop_name"s] = json::Node{ static_cast<std::string>(t_router_->GetVertexStop(edge_r.from)) };
			waste_time_stop["type"s] = json::Node{ "Wait"s };
			waste_time_stop["time"s] = json::Node{ static_cast<int>(t_router_->GetBusWaitTime()) };
			wastes_time.push_back(waste_time_stop);

			waste_time_bus["bus"s] = json::Node{ edge_r.route_name};
			waste_time_bus["span_count"s] = json::Node{ (int)edge_r.span_count };
			waste_time_bus["time"s] = json::Node{ edge_r.weight - t_router_->GetBusWaitTime() };
			waste_time_bus["type"s] = json::Node{ "Bus"s };
			wastes_time.push_back(waste_time_bus);
		}
		result["items"s] = wastes_time;
		result["total_time"s] = route->weight ;
	}
	else {
		result["error_message"s] = json::Node{ "not found"s };
	}
	return json::Node{ result };
}

void JsonReader::SetTransportRouter(const TransportRouter& t_router) {
	t_router_ = &t_router;
}

void JsonReader::SetRequestHandler(RequestHandler& rh) {
	rh_ = &rh;
}

const json::Dict& JsonReader::GetRequests() const {
	return doc_.GetRoot().AsMap();
}

namespace transport_catalogue {
    namespace transport_catalogue_input {
        void FillCatalog(TransportCatalogue& tc, const transport_catalogue_serialization::BaseRequests& base_requests_ser) {
			unordered_map<string, unordered_map<string, int>> from_to_dist;
			transport_catalogue_serialization::StopsNameMap stops_name_map = base_requests_ser.stops_name_map();
			std::map<std::string, uint32_t> stops_name_id_std(stops_name_map.stop_name_id().begin(), stops_name_map.stop_name_id().end());
			std::map<uint32_t, std::string> id_stops_name;

			for (auto& [stop_name, id] : stops_name_id_std) {
				id_stops_name[id] = stop_name;
			}
			for (size_t i = 0; i < base_requests_ser.stops_size(); ++i) {
				transport_catalogue_serialization::Stop stop = base_requests_ser.stops(i);
				tc.AddStop(detail::FillStop(stop, id_stops_name, from_to_dist));
			}
			for (size_t i = 0; i < base_requests_ser.routes_size(); ++i) {
				transport_catalogue_serialization::Bus route = base_requests_ser.routes(i);
				auto [bus, stops] = detail::FillRoute(route, id_stops_name, tc);
				tc.AddRoute(bus, stops);
			};
            for (auto& [from, to_dist] : from_to_dist) {
                for (auto& [to, dist] : to_dist) {
                    tc.SetDistanceBetweenStops(tc.FindStop(from), tc.FindStop(to), abs(dist));
                }
            }
        }
        namespace detail {
            pair<Bus, vector<string>> FillRoute(const transport_catalogue_serialization::Bus& bus,
												const std::map<uint32_t, std::string>& id_stops_name_std,
												TransportCatalogue& tc) {
                Bus result;
                result.name = bus.name();
                result.circle_key = bus.is_roundtrip();
				vector<string> stops;
				for (size_t i = 0; i < bus.stops_id_size(); ++i) {
					stops.push_back(id_stops_name_std.at(bus.stops_id(i)));
				}
                for (auto& stop_str : stops) {
                    if (const Stop* stop = tc.FindStop(stop_str); stop) {
                        result.bus_stops.push_back(stop);
                    }
                }
                return { result, stops };
            }

            Stop FillStop(const transport_catalogue_serialization::Stop& stop,
						  const std::map<uint32_t, std::string>& id_stops_name_std,
						  unordered_map<string, unordered_map<string, int>>& from_to_dist) {
                Stop result;
				result.name = id_stops_name_std.at(stop.name_id());
				result.coordinates.lat = stop.coordinates().lat();
				result.coordinates.lng = stop.coordinates().lng();
				for (size_t i = 0; i < stop.road_distance_size(); ++i) {
					from_to_dist[result.name][id_stops_name_std.at(stop.road_distance(i).stop_id())] = stop.road_distance(i).distance();
				};
                return result;
            }

        }// namespace detail
    }// namespace transport_catalogue_input


}// namespace transport_catalogue