#include "map_renderer.h"
using namespace std;

namespace renderer {
	void MapRender::SetWidth(const double width) noexcept {
		width_ = width;
	}
	void MapRender::SetHeight(const double height) noexcept {
		height_ = height;
	}
	void MapRender::SetPadding(const double padding) noexcept {
		padding_ = padding;
	}
	void MapRender::SetLineWidth(const double line_width) noexcept {
		line_width_ = line_width;
	}
	void MapRender::SetStopRadius(const double stop_radius) noexcept {
		stop_radius_ = stop_radius;
	}
	void MapRender::SetBusLableFontSize(const uint32_t bus_label_font_size) noexcept {
		bus_label_font_size_ = bus_label_font_size;
	}
	void MapRender::SetBusLableOffset(const double bus_label_offset_dx, const double bus_label_offset_dy) noexcept {
		bus_label_offset_[0] = bus_label_offset_dx;
		bus_label_offset_[1] = bus_label_offset_dy;
	}
	void MapRender::SetStopLableFontSize(const uint32_t stop_label_font_size) noexcept {
		stop_label_font_size_ = stop_label_font_size;
	}
	void MapRender::SetStopLableOffset(const double stop_label_offset_dx, const double stop_label_offset_dy) noexcept {
		stop_label_offset_[0] = stop_label_offset_dx;
		stop_label_offset_[1] = stop_label_offset_dy;
	}
	void MapRender::SetUnderLayerColor(const svg::Color underlayer_color) noexcept {
		underlayer_color_ = underlayer_color;
	}
	void MapRender::SetUnderLayerWidth(const double underlayer_width) noexcept {
		underlayer_width_ = underlayer_width;
	}
	void MapRender::SetColorPalette(const std::vector<svg::Color>& color_palette) noexcept {
		color_palette_ = color_palette;
	}

	svg::Document MapRender::RenderSVG(const transport_catalogue::TransportCatalogue& db_) const {

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
			double width_zoom_coef = (width_ - 2 * padding_) / (max_lon - min_lon);
			double height_zoom_coef = (height_ - 2 * padding_) / (max_lat - min_lat);
			zoom_coef = height_zoom_coef < width_zoom_coef ? height_zoom_coef : width_zoom_coef;
		}
		else if (max_lon == min_lon && max_lat != min_lat) {
			zoom_coef = (height_ - 2 * padding_) / (max_lat - min_lat);
		}
		else if (max_lon != min_lon && max_lat == min_lat) {
			zoom_coef = (width_ - 2 * padding_) / (max_lon - min_lon);
		}
		else {
			zoom_coef = 0.0;
		}
		//========================================================================рисуем линиии=======================================
		auto bus_name__to_bus = db_.GetBusNameToBus();

		size_t i = 0;
		for (auto& [bus_name, bus] : bus_name__to_bus) {
			if (bus->bus_stops.size() != 0) {
				svg::Polyline polyline;
				vector<pair<double, double>> points;
				polyline.SetStrokeColor(color_palette_.at(i)).SetFillColor("none"s).SetStrokeWidth(line_width_)
					.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
				for (auto& stop : bus->bus_stops) {
					double x = (stop->coordinates.lng - min_lon) * zoom_coef + padding_;
					double y = (max_lat - stop->coordinates.lat) * zoom_coef + padding_;
					polyline.AddPoint({ x,y });
					if (!bus->circle_key) {
						points.push_back({ x,y });
					}
				}
				if (bus->circle_key) {
					auto& stop = *(bus->bus_stops.begin());
					double x = (stop->coordinates.lng - min_lon) * zoom_coef + padding_;
					double y = (max_lat - stop->coordinates.lat) * zoom_coef + padding_;
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
				if (i == color_palette_.size()) {
					i = 0;
				}
			}
		}


		//========================================================================рисуем текст=======================================
		i = 0;
		for (auto& [bus_name, bus] : bus_name__to_bus) {
			if (bus->bus_stops.size() != 0) {
				if (bus->circle_key) {
					svg::Text text;
					svg::Text podlojka;
					auto& stop = *(bus->bus_stops.begin());
					double x = (stop->coordinates.lng - min_lon) * zoom_coef + padding_;
					double y = (max_lat - stop->coordinates.lat) * zoom_coef + padding_;

					text.SetPosition({ x,y }).SetData(bus->name).SetFillColor(color_palette_.at(i));
					podlojka.SetPosition({ x,y }).SetData(bus->name);
					DrawText(text, podlojka);

					result.Add(podlojka);
					result.Add(text);
				}
				else {
					auto& stop_begin = *(bus->bus_stops.begin());
					auto& stop_end = *(bus->bus_stops.end() - 1);
					if (stop_begin == stop_end) {
						svg::Text text;
						svg::Text podlojka;
						auto& stop = stop_begin;
						double x = (stop->coordinates.lng - min_lon) * zoom_coef + padding_;
						double y = (max_lat - stop->coordinates.lat) * zoom_coef + padding_;
						text.SetPosition({ x,y }).SetData(bus->name).SetFillColor(color_palette_.at(i));
						podlojka.SetPosition({ x,y }).SetData(bus->name);
						DrawText(text, podlojka);

						result.Add(podlojka);
						result.Add(text);
					}
					else {
						svg::Text text;
						svg::Text podlojka;
						double x = (stop_begin->coordinates.lng - min_lon) * zoom_coef + padding_;
						double y = (max_lat - stop_begin->coordinates.lat) * zoom_coef + padding_;
						text.SetPosition({ x,y }).SetData(bus->name).SetFillColor(color_palette_.at(i));
						podlojka.SetPosition({ x,y }).SetData(bus->name);
						DrawText(text, podlojka);

						result.Add(podlojka);
						result.Add(text);

						x = (stop_end->coordinates.lng - min_lon) * zoom_coef + padding_;
						y = (max_lat - stop_end->coordinates.lat) * zoom_coef + padding_;
						text.SetPosition({ x,y });
						podlojka.SetPosition({ x,y });

						result.Add(podlojka);
						result.Add(text);
					}
				}

				i++;
				if (i == color_palette_.size()) {
					i = 0;
				}
			}
		}
		//========================================================================рисуем символы остановок=======================================
		for (auto& [stop_name, stop] : stop_name__to_stop) {
			for (auto& [bus_name, bus] : bus_name__to_bus) {
				if (bus->bus_stops.size() != 0) {
					if (db_.StopInfo(stop).count(bus_name)) {

						svg::Circle stop_simbol;
						double x = (stop->coordinates.lng - min_lon) * zoom_coef + padding_;
						double y = (max_lat - stop->coordinates.lat) * zoom_coef + padding_;
						stop_simbol.SetCenter({ x,y }).SetRadius(stop_radius_).SetFillColor("white"s);
						result.Add(stop_simbol);
						break;
					}
				}
			}
		}

		//========================================================================рисуем названия остановок=======================================
		for (auto& [stop_name, stop] : stop_name__to_stop) {
			for (auto& [bus_name, bus] : bus_name__to_bus) {
				if (bus->bus_stops.size() != 0) {
					if (db_.StopInfo(stop).count(bus_name)) {

						svg::Text text_stop_name;
						svg::Text podlojka;
						double x = (stop->coordinates.lng - min_lon) * zoom_coef + padding_;
						double y = (max_lat - stop->coordinates.lat) * zoom_coef + padding_;
						text_stop_name.SetPosition({ x,y }).SetOffset({ stop_label_offset_[0],stop_label_offset_[1] }).SetFontSize(stop_label_font_size_);
						text_stop_name.SetFontFamily("Verdana"s).SetData(stop->name).SetFillColor("black"s);
						podlojka.SetPosition({ x,y }).SetOffset({ stop_label_offset_[0],stop_label_offset_[1] }).SetFontSize(stop_label_font_size_);
						podlojka.SetFontFamily("Verdana"s).SetData(stop->name).SetFillColor(underlayer_color_).SetStrokeWidth(underlayer_width_);
						podlojka.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetStrokeColor(underlayer_color_);
						result.Add(podlojka);
						result.Add(text_stop_name);
						break;
					}
				}
			}
		}

		return result;
	}
	void MapRender::DrawText(svg::Text& text, svg::Text& podlojka) const {
		text.SetOffset({ bus_label_offset_[0],bus_label_offset_[1] }).SetFontSize(bus_label_font_size_);
		text.SetFontFamily("Verdana"s).SetFontWeight("bold"s);
		podlojka.SetOffset({ bus_label_offset_[0],bus_label_offset_[1] }).SetFontSize(bus_label_font_size_);
		podlojka.SetFontFamily("Verdana"s).SetFontWeight("bold"s).SetFillColor(underlayer_color_).SetStrokeWidth(underlayer_width_);
		podlojka.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetStrokeColor(underlayer_color_);
	}

}// renderer