#include "serialization.h"

#include <fstream>

using namespace std::literals;

void Serialize(const std::string output_file, const transport_catalogue::TransportCatalogue& catalogue, const renderer::MapRender map, const TransportRouter& router) {
    transport_catalogue_serialization::TransportCatalogue transport_catalogue = SerializeTransportCatalogue(catalogue, map, router);
    WriteDataToFile(output_file, transport_catalogue);
}

void WriteDataToFile(const std::string output_file, transport_catalogue_serialization::TransportCatalogue transport_catalogue) {
    std::ofstream out_stream(output_file, std::ios::binary);
    transport_catalogue.SerializeToOstream(&out_stream);
}

transport_catalogue_serialization::TransportCatalogue SerializeTransportCatalogue(const transport_catalogue::TransportCatalogue& catalogue,
    const renderer::MapRender map,
    const TransportRouter& router) {

    transport_catalogue_serialization::TransportCatalogue transport_catalogue;
    *transport_catalogue.mutable_base_transport_catalogue() = SerializeBaseTransportCatalogue(catalogue);
    *transport_catalogue.mutable_render_settings() = SerializeRenderSettings(map);
    *transport_catalogue.mutable_routing_settings() = SerializeRoutingSettings(router);
    return transport_catalogue;
}

transport_catalogue_serialization::BaseTransportCatalogue SerializeBaseTransportCatalogue(const transport_catalogue::TransportCatalogue& catalogue) {
    transport_catalogue_serialization::BaseTransportCatalogue base_transport_catalogue;
    transport_catalogue_serialization::StopNameToId stop_name_to_id;

    size_t i = 0;
    for (const auto stop : catalogue.GetStops()) {
        (*stop_name_to_id.mutable_stop_name_to_id())[stop.name] = i;
        ++i;
    }
    *base_transport_catalogue.mutable_stop_name_to_id() = stop_name_to_id;

    for (const auto stop : catalogue.GetStops()) {
        *base_transport_catalogue.add_stops() = SerializeStop(stop, stop_name_to_id);
    }
    for (const auto bus : catalogue.GetBuses()) {
        *base_transport_catalogue.add_routes() = SerializeRoute(bus, stop_name_to_id);
    }
    *base_transport_catalogue.mutable_distance_between_stops() = SerializeDistanceBetweenStops(catalogue.GetDistancesBetweenStops(), stop_name_to_id);
    return base_transport_catalogue;
}

transport_catalogue_serialization::RenderSettings SerializeRenderSettings(const renderer::MapRender map) {
    transport_catalogue_serialization::RenderSettings render_settings_ser;
    render_settings_ser.set_bus_label_font_size(map.GetBusLableFontSize());
    std::pair<double, double> bus_label_offset = map.GetBusLableOffset();
    render_settings_ser.add_bus_label_offset(bus_label_offset.first);
    render_settings_ser.add_bus_label_offset(bus_label_offset.second);
    render_settings_ser.set_height(map.GetHeight());
    render_settings_ser.set_line_width(map.GetLineWidth());
    render_settings_ser.set_padding(map.GetPadding());
    render_settings_ser.set_stop_label_font_size(map.GetStopLableFontSize());
    std::pair<double, double> stop_label_offset = map.GetStopLableOffset();
    render_settings_ser.add_stop_label_offset(stop_label_offset.first);
    render_settings_ser.add_stop_label_offset(stop_label_offset.second);
    render_settings_ser.set_stop_radius(map.GetStopRadius());
    render_settings_ser.set_underlayer_width(map.GetUnderLayerWidth());
    render_settings_ser.set_width(map.GetWidth());
    SerializeSetUnredlayerColor(map.GetUnderLayerColor(), render_settings_ser);
    SerializeSetColorPalette(map.GetColorPalette(), render_settings_ser);
    return render_settings_ser;
}

transport_catalogue_serialization::RoutingSettings SerializeRoutingSettings(const TransportRouter& router) {
    transport_catalogue_serialization::RoutingSettings routing_settings_ser;
    routing_settings_ser.set_bus_velocity(router.GetBusVelocity());
    routing_settings_ser.set_bus_wait_time(router.GetBusWaitTime());
    return routing_settings_ser;
}

void SerializeSetUnredlayerColor(const svg::Color color, transport_catalogue_serialization::RenderSettings& render_settings_ser) {
    if (std::holds_alternative<svg::Rgb>(color)) {
        transport_catalogue_serialization::Rgb rgb_color;
        svg::Rgb rgb = std::get<svg::Rgb>(color);
        rgb_color.set_red(rgb.red);
        rgb_color.set_green(rgb.green);
        rgb_color.set_blue(rgb.blue);
        *render_settings_ser.mutable_underlayer_color()->mutable_rgb_color() = rgb_color;
    }else if(std::holds_alternative<svg::Rgba>(color)){
        transport_catalogue_serialization::Rgba rgba_color;
        svg::Rgba rgba = std::get<svg::Rgba>(color);
        rgba_color.set_red(rgba.red);
        rgba_color.set_green(rgba.green);
        rgba_color.set_blue(rgba.blue);
        rgba_color.set_opacity(rgba.opacity);
        *render_settings_ser.mutable_underlayer_color()->mutable_rgba_color() = rgba_color;
    }else {
        std::string string_color = std::get<std::string>(color);
        render_settings_ser.mutable_underlayer_color()->mutable_string_color()->set_color(string_color);
    }
}

void SerializeSetColorPalette(const std::vector<svg::Color>& colors, transport_catalogue_serialization::RenderSettings& render_settings_ser) {
    for (auto color : colors) {
        if (std::holds_alternative<svg::Rgb>(color)) {
            svg::Rgb rgb = std::get<svg::Rgb>(color);
            transport_catalogue_serialization::Color color_ser;
            transport_catalogue_serialization::Rgb rgb_color;
            rgb_color.set_red(rgb.red);
            rgb_color.set_green(rgb.green);
            rgb_color.set_blue(rgb.blue);
            *color_ser.mutable_rgb_color() = rgb_color;
            *render_settings_ser.add_palette() = color_ser;
        }
        else if (std::holds_alternative<svg::Rgba>(color)) {
            svg::Rgba rgba = std::get<svg::Rgba>(color);
            transport_catalogue_serialization::Color color_ser;
            transport_catalogue_serialization::Rgba rgba_color;
            rgba_color.set_red(rgba.red);
            rgba_color.set_green(rgba.green);
            rgba_color.set_blue(rgba.blue);
            rgba_color.set_opacity(rgba.opacity);
            *color_ser.mutable_rgba_color() = rgba_color;
            *render_settings_ser.add_palette() = color_ser;
        }
        else {
            transport_catalogue_serialization::Color color_ser;
            std::string string_color = std::get<std::string>(color);
            color_ser.mutable_string_color()->set_color(string_color);
            *render_settings_ser.add_palette() = color_ser;
        }
    }
}

transport_catalogue_serialization::Stop SerializeStop(const Stop& stop, const transport_catalogue_serialization::StopNameToId& stop_name_to_id) {
    transport_catalogue_serialization::Stop ser_stop;
    ser_stop.set_name_id(stop_name_to_id.stop_name_to_id().at(stop.name));
    ser_stop.mutable_coordinates()->set_lat(stop.coordinates.lat);
    ser_stop.mutable_coordinates()->set_lng(stop.coordinates.lng);
    return ser_stop;
}

transport_catalogue_serialization::Bus SerializeRoute(const Bus& stop, const transport_catalogue_serialization::StopNameToId& stop_name_to_id) {
    transport_catalogue_serialization::Bus route;

    route.set_name(stop.name);
    route.set_is_roundtrip(stop.circle_key);

    for (const auto stop : stop.bus_stops) {
        route.add_stops_id(stop_name_to_id.stop_name_to_id().at(stop->name));
    }
    return route;
}

transport_catalogue_serialization::DistanceBetweenStops SerializeDistanceBetweenStops(const transport_catalogue::DistFromTo from_to_dist, const transport_catalogue_serialization::StopNameToId& stop_name_to_id) {
    transport_catalogue_serialization::DistanceBetweenStops distance_between_stops;
    for (const auto& [from, to_dist] : from_to_dist) {
        transport_catalogue_serialization::ToDistance to_distance;
        for (const auto& [to, dist] : to_dist) {
            (*to_distance.mutable_to_distance())[stop_name_to_id.stop_name_to_id().at(to->name)] = dist;
        }
        (*distance_between_stops.mutable_from_to_distance())[stop_name_to_id.stop_name_to_id().at(from->name)] = to_distance;
    }
    return distance_between_stops;
}

std::optional<transport_catalogue_serialization::TransportCatalogue> Deserialize(const std::string input_file) {
    std::ifstream in_file(input_file, std::ios::binary);
    transport_catalogue_serialization::TransportCatalogue t_catalogue;
    if (!t_catalogue.ParseFromIstream(&in_file)) {
        return std::nullopt;
    }
    return { std::move(t_catalogue) };
}

transport_catalogue::TransportCatalogue DeserializeTransportCatalogue(const transport_catalogue_serialization::BaseTransportCatalogue& base_transport_catalogue) {
    transport_catalogue::TransportCatalogue catalogue;
    transport_catalogue_serialization::StopNameToId stop_name_to_id_ser = base_transport_catalogue.stop_name_to_id();
    std::map<std::string, uint32_t> stops_name_to_id(stop_name_to_id_ser.stop_name_to_id().begin(), stop_name_to_id_ser.stop_name_to_id().end());
    std::map<uint32_t, std::string> id_to_stop_name;

    for (auto& [stop_name, id] : stops_name_to_id) {
        id_to_stop_name[id] = stop_name;
    }
    for (size_t i = 0; i < base_transport_catalogue.stops_size(); ++i) {
        transport_catalogue_serialization::Stop stop = base_transport_catalogue.stops(i);
        catalogue.AddStop(DeserializeStop(stop, id_to_stop_name));
    }
    for (size_t i = 0; i < base_transport_catalogue.routes_size(); ++i) {
        transport_catalogue_serialization::Bus route = base_transport_catalogue.routes(i);
        Bus bus = DeserializeRoute(route, id_to_stop_name, catalogue);
        catalogue.AddRoute(bus);
    }
    transport_catalogue_serialization::DistanceBetweenStops distance_betweenStops = base_transport_catalogue.distance_between_stops();
    for (auto& [from, to_distance] : distance_betweenStops.from_to_distance()) {
        for (auto& [to, distance] : to_distance.to_distance()) {
            catalogue.SetDistanceBetweenStops(catalogue.FindStop(id_to_stop_name.at(from)), catalogue.FindStop(id_to_stop_name.at(to)), distance);
        }
    }
    return catalogue;
}

renderer::MapRender DeserializeMapRender(const transport_catalogue_serialization::RenderSettings& render_settings_ser) {
    renderer::MapRender map;
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
    std::vector<svg::Color> palette_colors;
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
    return map;
}

TransportRouter DeserializeTransportRouter(const transport_catalogue_serialization::RoutingSettings& routing_settings_ser, const transport_catalogue::TransportCatalogue catalogue) {
    TransportRouter router(catalogue);
    router.SetRouteSettings(routing_settings_ser.bus_velocity(), routing_settings_ser.bus_wait_time());
    return router;
}

Bus DeserializeRoute(const transport_catalogue_serialization::Bus& bus,
                                                          const std::map<uint32_t, std::string>& id_to_stop_name,
                                                          transport_catalogue::TransportCatalogue& catalogue) {
    Bus result;
    result.name = bus.name();
    result.circle_key = bus.is_roundtrip();
    for (size_t i = 0; i < bus.stops_id_size(); ++i) {
        if (const Stop* stop = catalogue.FindStop(id_to_stop_name.at(bus.stops_id(i))); stop) {
            result.bus_stops.push_back(stop);
        }
    }
    return result;
}

Stop DeserializeStop(const transport_catalogue_serialization::Stop& stop, const std::map<uint32_t, std::string>& id_to_stop_name) {
    Stop result;
    result.name = id_to_stop_name.at(stop.name_id());
    result.coordinates.lat = stop.coordinates().lat();
    result.coordinates.lng = stop.coordinates().lng();
    return result;
}