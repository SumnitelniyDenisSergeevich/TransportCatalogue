#include "json_reader.h"
#include <sstream>
using namespace std;


JsonReader::JsonReader(istream& in, RequestHandler& rh) : doc_(json::Load(in)), rh_(rh) {
}

void JsonReader::FillCatalogue(transport_catalogue::TransportCatalogue& tc) const {
    auto requests = doc_.GetRoot().AsMap().at("base_requests"s);
    transport_catalogue::transport_catalogue_input::FillCatalog(requests, tc);
}

void JsonReader::FillRenderSettings(renderer::MapRender& map) const {
	json::Dict render_settings = doc_.GetRoot().AsMap().at("render_settings"s).AsMap();
	map.SetWidth(render_settings.at("width"s).AsDouble());
	map.SetHeight(render_settings.at("height"s).AsDouble());
	map.SetPadding(render_settings.at("padding"s).AsDouble());
	map.SetLineWidth(render_settings.at("line_width"s).AsDouble());
	map.SetStopRadius(render_settings.at("stop_radius"s).AsDouble());
	map.SetBusLableFontSize(render_settings.at("bus_label_font_size"s).AsInt());
	json::Array dx_dy_bus_offset = render_settings.at("bus_label_offset"s).AsArray();
	map.SetBusLableOffset(dx_dy_bus_offset[0].AsDouble(), dx_dy_bus_offset[1].AsDouble());
	map.SetStopLableFontSize(render_settings.at("stop_label_font_size"s).AsInt());
	json::Array dx_dy_stop_offset = render_settings.at("stop_label_offset"s).AsArray();
	map.SetStopLableOffset(dx_dy_stop_offset[0].AsDouble(), dx_dy_stop_offset[1].AsDouble());
	auto underlayer_color = render_settings.at("underlayer_color"s);
	if (underlayer_color.IsArray()) {
		json::Array color = underlayer_color.AsArray();
		if (color.size() == 3) {
			map.SetUnderLayerColor(svg::Rgb{ static_cast<uint8_t>(color[0].AsInt()), static_cast<uint8_t>(color[1].AsInt()), static_cast<uint8_t>(color[2].AsInt()) });
		}
		else {
			map.SetUnderLayerColor(svg::Rgba{ static_cast<uint8_t>(color[0].AsInt()), static_cast<uint8_t>(color[1].AsInt()), static_cast<uint8_t>(color[2].AsInt()), color[3].AsDouble() });
		}
	}
	else {
		map.SetUnderLayerColor(underlayer_color.AsString());
	}
	map.SetUnderLayerWidth(render_settings.at("underlayer_width"s).AsDouble());
	auto color_palette = render_settings.at("color_palette"s).AsArray();
	vector<svg::Color> color_palette_;
	for (auto color_node : color_palette) {
		svg::Color color;
		if (color_node.IsArray()) {
			json::Array color_arr = color_node.AsArray();
			if (color_arr.size() == 3) {
				color = svg::Rgb{ static_cast<uint8_t>(color_arr[0].AsInt()), static_cast<uint8_t>(color_arr[1].AsInt()), static_cast<uint8_t>(color_arr[2].AsInt()) };
			}
			else {
				color = svg::Rgba{ static_cast<uint8_t>(color_arr[0].AsInt()), static_cast<uint8_t>(color_arr[1].AsInt()), static_cast<uint8_t>(color_arr[2].AsInt()) , color_arr[3].AsDouble() };
			}
		}
		else {
			color = color_node.AsString();
		}
		color_palette_.push_back(color);
	}
	map.SetColorPalette(color_palette_);
}

void JsonReader::FillRouteSettings(TransportRouter& t_r)const {
	json::Dict render_settings = doc_.GetRoot().AsMap().at("routing_settings"s).AsMap();
	t_r.SetRouteSettings(render_settings.at("bus_velocity"s).AsDouble(), render_settings.at("bus_wait_time"s).AsInt());
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
			rh_.RenderMap().Render(out);
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

	auto buses = rh_.GetBusesByStop(stop_name);

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
	return json::Node{ result };
}

json::Node JsonReader::GetStatRoute(const json::Node& route_node) const {
	json::Dict result;
	json::Dict route_stat = route_node.AsMap();
	string route_name = route_stat.find("name"s)->second.AsString();
	result["request_id"s] = json::Node{ route_stat.find("id"s)->second.AsInt() };
	auto rout_info = rh_.GetBusStat(route_name);
	if (!rout_info.has_value()) {
		result["error_message"s] = json::Node{ "not found"s };
	}
	else {
		result["curvature"s] = json::Node{ rout_info->curvature };
		result["route_length"s] = json::Node{ rout_info->route_length };
		result["stop_count"s] = json::Node{ static_cast<int>(rout_info->rout_stops_count) };
		result["unique_stop_count"s] = json::Node{ static_cast<int>(rout_info->unic_rout_stop_count) };
	}
	return json::Node{ result };
}

json::Node JsonReader::GetRoute(const json::Node& route_node) const {			//new
	json::Dict result;
	json::Dict route_stat = route_node.AsMap();
	string route_from = route_stat.find("from"s)->second.AsString();
	string route_to = route_stat.find("to"s)->second.AsString();
	result["request_id"s] = json::Node{ route_stat.find("id"s)->second.AsInt() };
	std::optional<graph::Router<double>::RouteInfo> route = transport_router_->BuildRoute(transport_router_->GetVertexId(route_from),
																							transport_router_->GetVertexId(route_to));
	if (route.has_value()) {
		json::Array wastes_time;
		for (auto edge : route->edges) {
			json::Dict waste_time_stop;
			json::Dict waste_time_bus;
			auto edge_r = transport_router_->GetEdge(edge);
			waste_time_stop["stop_name"s] = json::Node{ static_cast<std::string>(transport_router_->GetVertexStop(edge_r.from)) };
			waste_time_stop["type"s] = json::Node{ "Wait"s };
			waste_time_stop["time"s] = json::Node{ static_cast<int>(transport_router_->GetBusWaitTime()) };
			wastes_time.push_back(waste_time_stop);

			waste_time_bus["bus"s] = json::Node{ edge_r.route_name};
			waste_time_bus["span_count"s] = json::Node{ (int)edge_r.span_count };
			waste_time_bus["time"s] = json::Node{ edge_r.weight - transport_router_->GetBusWaitTime() };
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


const json::Dict& JsonReader::GetRequests() const {
	return doc_.GetRoot().AsMap();
}

void JsonReader::SetTransportRouter(const TransportRouter& transport_router) {
	transport_router_ = &transport_router;
}

namespace transport_catalogue {
    namespace transport_catalogue_input {
        void FillCatalog(const Node& base_requests, TransportCatalogue& tc) {
            Array bus_requests;
            unordered_map<string, Dict> from_to_dist;
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
                    tc.SetDistanceBetweenStops(tc.FindStop(from), tc.FindStop(to), abs(dist.AsInt()));
                }
            }
        }
        namespace detail {
            pair<Bus, vector<string>> FillRoute(Node& node_bus, TransportCatalogue& tc) {
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