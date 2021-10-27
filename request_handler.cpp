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
			//ofstream out1("render.svg"s); // в файл	
			std::ostringstream out;
			//RenderMap().Render(out1);//в файл
			RenderMap().Render(out);
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
		transport_catalogue::RoutInfoS rout_info = db_.RouteInfo(route_name);
		result["curvature"s] = json::Node{ rout_info.curvature };
		result["route_length"s] = json::Node{ rout_info.route_length };
		result["stop_count"s] = json::Node{ static_cast<int>(rout_info.rout_stops_count) };
		result["unique_stop_count"s] = json::Node{ static_cast<int>(rout_info.unic_rout_stop_count) };
	}
	return json::Node{ result };
}

svg::Document RequestHandler::RenderMap() const {
	auto stop_name__to_stop = db_.GetStopNameToStop();

	svg::Document result;

	double min_lat = 0.0, max_lat = min_lat;
	double min_lon = 0.0, max_lon = min_lon;
	bool b = true;

	for (auto& [stop_name, stop] : stop_name__to_stop) {
		if (db_.StopInfo(stop).size() != 0) {
			if (b) {
				min_lat = stop->coordinates.lat;
				max_lat = min_lat;
				min_lon = stop->coordinates.lng;
				max_lon = min_lon;
			}
			min_lat = stop->coordinates.lat < min_lat ? stop->coordinates.lat : min_lat;
			max_lat = stop->coordinates.lat > max_lat ? stop->coordinates.lat : max_lat;
			min_lon = stop->coordinates.lng < min_lon ? stop->coordinates.lng : min_lon;
			max_lon = stop->coordinates.lng > max_lon ? stop->coordinates.lng : max_lon;
			b = false;
		}
	}
	double zoom_coef = 0.0;
	if (max_lon != min_lon && max_lat != min_lat) {
		double width_zoom_coef = (renderer_.width_ - 2 * renderer_.padding_) / (max_lon - min_lon);// максимально допустимый коэффициент масштабирования долгот в иксы
		double height_zoom_coef = (renderer_.height_ - 2 * renderer_.padding_) / (max_lat - min_lat);// Максимально допустимый коэффициент масштабирования широт в игреки
		zoom_coef = height_zoom_coef < width_zoom_coef ? height_zoom_coef : width_zoom_coef;
	}
	else if (max_lon == min_lon && max_lat != min_lat) {
		zoom_coef= (renderer_.height_ - 2 * renderer_.padding_) / (max_lat - min_lat);// Максимально допустимый коэффициент масштабирования широт в игреки
	}
	else if (max_lon != min_lon && max_lat == min_lat) {
		zoom_coef = (renderer_.width_ - 2 * renderer_.padding_) / (max_lon - min_lon);// максимально допустимый коэффициент масштабирования долгот в иксы 
	}
	else {
		zoom_coef = 0.0;
	}

	/*Формулы вычисления x и y по долготе lon и широте lat :
	x = (lon - min_lon) * zoom_coef + padding;
	y = (max_lat - lat) * zoom_coef + padding;*/
	//========================================================================рисуем линиии=======================================
	auto bus_name__to_bus = db_.GetBusNameToBus();

	int i = 0;
	for (auto& [bus_name, bus] : bus_name__to_bus) {
		if (bus->bus_stops.size() !=0) { //возможна ошибка, возможно >1
			svg::Polyline polyline;
			vector<pair<double,double>> points;
			polyline.SetStrokeColor(renderer_.color_palette_.at(i)).SetFillColor("none"s).SetStrokeWidth(renderer_.line_width_).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
			for (auto& stop : bus->bus_stops) {
				double x = (stop->coordinates.lng - min_lon) * zoom_coef + renderer_.padding_;
				double y = (max_lat - stop->coordinates.lat) * zoom_coef + renderer_.padding_;
				polyline.AddPoint({ x,y });
				if (!bus->circle_key) {
					points.push_back({ x,y });
				}
			}
			if (bus->circle_key) {
				auto& stop = *(bus->bus_stops.begin());
				double x = (stop->coordinates.lng - min_lon) * zoom_coef + renderer_.padding_;
				double y = (max_lat - stop->coordinates.lat) * zoom_coef + renderer_.padding_;
				polyline.AddPoint({ x,y });
			}
			else {
				points.pop_back();
				for (auto iter = points.rbegin(); iter != points.rend(); ++iter) {
					polyline.AddPoint({ iter->first,iter->second });
				}
			}
			result.Add(polyline);
			i++;
			if (i == renderer_.color_palette_.size()) {
				i = 0;
			}
		}
	}


	//========================================================================рисуем текст=======================================
	i = 0;
	for (auto& [bus_name, bus] : bus_name__to_bus) {
		if (bus->bus_stops.size() != 0) { //возможна ошибка, возможно >1
			if (bus->circle_key) {
				svg::Text text;
				svg::Text podlojka;
				auto& stop = *(bus->bus_stops.begin());
				double x = (stop->coordinates.lng - min_lon) * zoom_coef + renderer_.padding_;
				double y = (max_lat - stop->coordinates.lat) * zoom_coef + renderer_.padding_;
				text.SetPosition({ x,y }).SetOffset({ renderer_.bus_label_offset_[0],renderer_.bus_label_offset_[1] }).SetFontSize(renderer_.bus_label_font_size_);
				text.SetFontFamily("Verdana"s).SetFontWeight("bold"s).SetData(bus->name).SetFillColor(renderer_.color_palette_.at(i));
				podlojka.SetPosition({ x,y }).SetOffset({ renderer_.bus_label_offset_[0],renderer_.bus_label_offset_[1] }).SetFontSize(renderer_.bus_label_font_size_);
				podlojka.SetFontFamily("Verdana"s).SetFontWeight("bold"s).SetData(bus->name).SetFillColor(renderer_.underlayer_color_).SetStrokeWidth(renderer_.underlayer_width_);
				podlojka.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetStrokeColor(renderer_.underlayer_color_);

				result.Add(podlojka);
				result.Add(text);
			}
			else {
				auto& stop_begin = *(bus->bus_stops.begin());
				auto& stop_end = *(bus->bus_stops.end()-1);
				if (stop_begin == stop_end) {
					svg::Text text;
					svg::Text podlojka;
					auto& stop = stop_begin;
					double x = (stop->coordinates.lng - min_lon) * zoom_coef + renderer_.padding_;
					double y = (max_lat - stop->coordinates.lat) * zoom_coef + renderer_.padding_;
					text.SetPosition({ x,y }).SetOffset({ renderer_.bus_label_offset_[0],renderer_.bus_label_offset_[1] }).SetFontSize(renderer_.bus_label_font_size_);
					text.SetFontFamily("Verdana"s).SetFontWeight("bold"s).SetData(bus->name).SetFillColor(renderer_.color_palette_.at(i));
					podlojka.SetPosition({ x,y }).SetOffset({ renderer_.bus_label_offset_[0],renderer_.bus_label_offset_[1] }).SetFontSize(renderer_.bus_label_font_size_);
					podlojka.SetFontFamily("Verdana"s).SetFontWeight("bold"s).SetData(bus->name).SetFillColor(renderer_.underlayer_color_).SetStrokeWidth(renderer_.underlayer_width_);
					podlojka.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetStrokeColor(renderer_.underlayer_color_);

					result.Add(podlojka);
					result.Add(text);
				}
				else {
					svg::Text text;
					svg::Text podlojka;
					double x = (stop_begin->coordinates.lng - min_lon) * zoom_coef + renderer_.padding_;
					double y = (max_lat - stop_begin->coordinates.lat) * zoom_coef + renderer_.padding_;
					text.SetPosition({ x,y }).SetOffset({ renderer_.bus_label_offset_[0],renderer_.bus_label_offset_[1] }).SetFontSize(renderer_.bus_label_font_size_);
					text.SetFontFamily("Verdana"s).SetFontWeight("bold"s).SetData(bus->name).SetFillColor(renderer_.color_palette_.at(i));
					podlojka.SetPosition({ x,y }).SetOffset({ renderer_.bus_label_offset_[0],renderer_.bus_label_offset_[1] }).SetFontSize(renderer_.bus_label_font_size_);
					podlojka.SetFontFamily("Verdana"s).SetFontWeight("bold"s).SetData(bus->name).SetFillColor(renderer_.underlayer_color_).SetStrokeWidth(renderer_.underlayer_width_);
					podlojka.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetStrokeColor(renderer_.underlayer_color_);

					result.Add(podlojka);
					result.Add(text);

					x = (stop_end->coordinates.lng - min_lon) * zoom_coef + renderer_.padding_;
					 y = (max_lat - stop_end->coordinates.lat) * zoom_coef + renderer_.padding_;
					text.SetPosition({ x,y });
					podlojka.SetPosition({ x,y });

					result.Add(podlojka);
					result.Add(text);
				}
			}

			i++;
			if (i == renderer_.color_palette_.size()) {
				i = 0;
			}
		}
	}
	//========================================================================рисуем символы остановок=======================================
	for (auto& [stop_name, stop] : stop_name__to_stop) {
		for (auto& [bus_name, bus] : bus_name__to_bus) {
			if (bus->bus_stops.size() != 0) { //возможна ошибка, возможно >1
				if (db_.StopInfo(stop).count(bus_name)) {

					svg::Circle stop_simbol;
					double x = (stop->coordinates.lng - min_lon) * zoom_coef + renderer_.padding_;
					double y = (max_lat - stop->coordinates.lat) * zoom_coef + renderer_.padding_;
					stop_simbol.SetCenter({ x,y }).SetRadius(renderer_.stop_radius_).SetFillColor("white"s);
					result.Add(stop_simbol);
					break;
				}
			}
		}
	}

	//========================================================================рисуем названия остановок=======================================
	for (auto& [stop_name, stop] : stop_name__to_stop) {
		for (auto& [bus_name, bus] : bus_name__to_bus) {
			if (bus->bus_stops.size() != 0) { //возможна ошибка, возможно >1
				if (db_.StopInfo(stop).count(bus_name)) {

					svg::Text text_stop_name;
					svg::Text podlojka;
					double x = (stop->coordinates.lng - min_lon) * zoom_coef + renderer_.padding_;
					double y = (max_lat - stop->coordinates.lat) * zoom_coef + renderer_.padding_;
					text_stop_name.SetPosition({ x,y }).SetOffset({ renderer_.stop_label_offset_[0],renderer_.stop_label_offset_[1] }).SetFontSize(renderer_.stop_label_font_size_);
					text_stop_name.SetFontFamily("Verdana"s).SetData(stop->name).SetFillColor("black"s);
					podlojka.SetPosition({ x,y }).SetOffset({ renderer_.stop_label_offset_[0],renderer_.stop_label_offset_[1] }).SetFontSize(renderer_.stop_label_font_size_);
					podlojka.SetFontFamily("Verdana"s).SetData(stop->name).SetFillColor(renderer_.underlayer_color_).SetStrokeWidth(renderer_.underlayer_width_);
					podlojka.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetStrokeColor(renderer_.underlayer_color_);
					result.Add(podlojka);
					result.Add(text_stop_name);
					break;
				}
			}
		}
	}

	return result;
}