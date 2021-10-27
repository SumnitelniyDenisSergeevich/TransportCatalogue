#include "map_render.h"
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
}// renderer