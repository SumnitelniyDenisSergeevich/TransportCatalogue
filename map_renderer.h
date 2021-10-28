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
		double width_;
		double height_;
		double padding_;
		double line_width_;
		double stop_radius_;
		uint32_t bus_label_font_size_;
		std::array<double, 2> bus_label_offset_;
		uint32_t stop_label_font_size_;
		std::array<double, 2> stop_label_offset_;
		svg::Color underlayer_color_;
		double underlayer_width_;  
		std::vector<svg::Color> color_palette_;

		void DrawText(svg::Text& text, svg::Text& podlojka) const;
	};
}//renderer