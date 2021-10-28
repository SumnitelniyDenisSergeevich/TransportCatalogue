#include "request_handler.h"
#include <sstream>
#include <iostream>
#include <fstream>// delete

using namespace std;

using namespace ::renderer;



RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue& db, MapRender& renderer, const json::Dict requests) : db_(db), renderer_(renderer), requests_(requests) {

}
void RequestHandler::FillRenderSettings() {
	json::Dict render_settings = requests_.at("render_settings").AsMap();
	renderer_.SetWidth(render_settings.at("width"s).AsDouble());
	renderer_.SetHeight(render_settings.at("height"s).AsDouble());
	renderer_.SetPadding(render_settings.at("padding"s).AsDouble());
	renderer_.SetLineWidth(render_settings.at("line_width"s).AsDouble());
	renderer_.SetStopRadius(render_settings.at("stop_radius"s).AsDouble());
	renderer_.SetBusLableFontSize(render_settings.at("bus_label_font_size"s).AsInt());
	json::Array dx_dy_bus_offset = render_settings.at("bus_label_offset"s).AsArray();
	renderer_.SetBusLableOffset(dx_dy_bus_offset[0].AsDouble(), dx_dy_bus_offset[1].AsDouble());
	renderer_.SetStopLableFontSize(render_settings.at("stop_label_font_size"s).AsInt());
	json::Array dx_dy_stop_offset = render_settings.at("stop_label_offset"s).AsArray();
	renderer_.SetStopLableOffset(dx_dy_stop_offset[0].AsDouble(), dx_dy_stop_offset[1].AsDouble());
	auto underlayer_color = render_settings.at("underlayer_color"s);
	if (underlayer_color.IsArray()) {
			json::Array color = underlayer_color.AsArray();
			if (color.size() == 3) {
				renderer_.SetUnderLayerColor(svg::Rgb{static_cast<uint8_t>(color[0].AsInt()), static_cast<uint8_t>(color[1].AsInt()), static_cast<uint8_t>(color[2].AsInt())});
			}
			else {
				renderer_.SetUnderLayerColor(svg::Rgba{ static_cast<uint8_t>(color[0].AsInt()), static_cast<uint8_t>(color[1].AsInt()), static_cast<uint8_t>(color[2].AsInt()), color[3].AsDouble() });
			}
		}
	else {
		renderer_.SetUnderLayerColor(underlayer_color.AsString());
	}
	renderer_.SetUnderLayerWidth(render_settings.at("underlayer_width"s).AsDouble());
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
	renderer_.SetColorPalette(color_palette_);
}

void RequestHandler::ProcessRequests(std::ostream& out) {
	json::Array stat_requests = requests_.at("stat_requests").AsArray();
	json::Array out_data;
	for (auto stat_request : stat_requests) {
		auto map_iter = stat_request.AsMap().find("type"s);
		if (map_iter->second.AsString() == "Stop"s) {
			out_data.push_back(StatStop(stat_request));
		}
		if (map_iter->second.AsString() == "Bus"s) {
			out_data.push_back(StatRoute(stat_request));
		}
		if (map_iter->second.AsString() == "Map"s) {
			json::Dict result;
			result["request_id"s] = json::Node{ stat_request.AsMap().find("id"s)->second.AsInt() };
			std::ostringstream out;
			RenderMap(db_).Render(out);
			string rendered_map = out.str();
			result["map"s] = json::Node{ rendered_map };
			out_data.push_back(json::Node{ result });
		}
	}
	json::Print(json::Document{ json::Node{out_data} }, out);
}

json::Node RequestHandler::StatStop(json::Node& stop_node) {
	json::Dict result;
	json::Dict stop_stat = stop_node.AsMap();
	std::string stop_name = stop_stat.find("name")->second.AsString();

	result["request_id"s] = json::Node{ stop_stat.find("id"s)->second.AsInt() };

	if (!db_.FindStop(stop_name)) {
		result["error_message"s] = json::Node{ "not found"s };
	}
	else {
		auto busstops = db_.StopInfo(db_.FindStop(stop_name));
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
json::Node RequestHandler::StatRoute(json::Node& route_node) {
	json::Dict result;
	json::Dict route_stat = route_node.AsMap();
	std::string route_name = route_stat.find("name"s)->second.AsString();
	result["request_id"s] = json::Node{ route_stat.find("id"s)->second.AsInt() };
	if (!db_.FindRoute(route_name)) {
		result["error_message"s] = json::Node{ "not found"s };
	}
	else {
		RoutInfoS rout_info = db_.RouteInfo(route_name);
		result["curvature"s] = json::Node{ rout_info.curvature };
		result["route_length"s] = json::Node{ rout_info.route_length };
		result["stop_count"s] = json::Node{ static_cast<int>(rout_info.rout_stops_count) };
		result["unique_stop_count"s] = json::Node{ static_cast<int>(rout_info.unic_rout_stop_count) };
	}
	return json::Node{ result };
}

svg::Document RequestHandler::RenderMap(const transport_catalogue::TransportCatalogue& db) const {
	return renderer_.RenderSVG(db);
}