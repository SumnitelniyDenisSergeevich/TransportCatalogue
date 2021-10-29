#pragma once
#include "domain.h"
#include "svg.h"
#include "transport_catalogue.h"

#include <array>

namespace renderer {
	class MapRender {
	public:
		void SetWidth(const double width) noexcept;
		void SetHeight(const double height) noexcept;
		void SetPadding(const double padding) noexcept;
		void SetLineWidth(const double line_width) noexcept;
		void SetStopRadius(const double stop_radius) noexcept;
		void SetBusLableFontSize(const uint32_t bus_label_font_size) noexcept;
		void SetBusLableOffset(const double bus_label_offset_dx, const double bus_label_offset_dy) noexcept;
		void SetStopLableFontSize(const uint32_t stop_label_font_size) noexcept;
		void SetStopLableOffset(const double stop_label_offset_dx, const double stop_label_offset_dy) noexcept;
		void SetUnderLayerColor(const svg::Color underlayer_color) noexcept;
		void SetUnderLayerWidth(const double underlayer_width) noexcept;
		void SetColorPalette(const std::vector<svg::Color>& color_palette) noexcept;

		svg::Document RenderSVG(const transport_catalogue::TransportCatalogue& db_) const;
	private:
		void DrawText(svg::Text& text, svg::Text& podlojka) const;

		double width_ = 0.0;
		double height_ = 0.0;
		double padding_ = 0.0;
		double line_width_ = 0.0;
		double stop_radius_ = 0.0;
		uint32_t bus_label_font_size_ = 0;
		std::array<double, 2> bus_label_offset_ = { 0.0, 0.0 };
		uint32_t stop_label_font_size_ = 0;
		std::array<double, 2> stop_label_offset_ = { 0.0, 0.0 };
		svg::Color underlayer_color_ = "none";
		double underlayer_width_ = 0.0;
		std::vector<svg::Color> color_palette_;
	};
}//renderer