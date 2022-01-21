#include "serialization.h"

#include <fstream>


using namespace std::literals;

void Serialize(const json::Dict& make_base_requests) {
    transport_catalogue_serialization::TransportCatalogue transport_catalogue;
    auto base_requests = make_base_requests.at("base_requests"s).AsArray();
    *transport_catalogue.mutable_base_requests() = SerializeBaseRequests(base_requests);
    auto render_settings = make_base_requests.at("render_settings"s).AsMap();
    *transport_catalogue.mutable_render_settings() = SerializeRenderSettings(render_settings);
    auto routing_settings = make_base_requests.at("routing_settings"s).AsMap();
    *transport_catalogue.mutable_routing_settings() = SerializeRoutingSettings(routing_settings);

    std::ofstream out_file(make_base_requests.at("serialization_settings"s).AsMap().begin()->second.AsString(), std::ios::binary);
    transport_catalogue.SerializeToOstream(&out_file);
}

void SerializeUnredlayerColor(const json::Dict& render_settings, transport_catalogue_serialization::RenderSettings& render_settings_ser) {
    auto underlayer_color = render_settings.at("underlayer_color"s);
    if (underlayer_color.IsArray()) {
        json::Array color = underlayer_color.AsArray();
        if (color.size() == 3U) {
            transport_catalogue_serialization::Rgb rgb_color;
            rgb_color.set_red(color[0].AsInt());
            rgb_color.set_green(color[1].AsInt());
            rgb_color.set_blue(color[2].AsInt());
            *render_settings_ser.mutable_underlayer_color()->mutable_rgb_color() = rgb_color;
        }
        else {
            transport_catalogue_serialization::Rgba rgba_color;
            rgba_color.set_red(color[0].AsInt());
            rgba_color.set_green(color[1].AsInt());
            rgba_color.set_blue(color[2].AsInt());
            rgba_color.set_opacity(color[3].AsDouble());
            *render_settings_ser.mutable_underlayer_color()->mutable_rgba_color() = rgba_color;
        }
    }
    else {
        render_settings_ser.mutable_underlayer_color()->mutable_string_color()->set_color(underlayer_color.AsString());
    }
}

void SerializeColorPalette(const json::Dict& render_settings, transport_catalogue_serialization::RenderSettings& render_settings_ser) {
    auto color_palette = render_settings.at("color_palette"s).AsArray();
    for (auto color : color_palette) {
        if (color.IsArray()) {
            json::Array color_arr = color.AsArray();
            if (color_arr.size() == 3U) {
                transport_catalogue_serialization::Color color_ser;
                transport_catalogue_serialization::Rgb rgb_color;
                rgb_color.set_red(color_arr[0].AsInt());
                rgb_color.set_green(color_arr[1].AsInt());
                rgb_color.set_blue(color_arr[2].AsInt());
                *color_ser.mutable_rgb_color() = rgb_color;
                *render_settings_ser.add_palette() = color_ser;
            }
            else {
                transport_catalogue_serialization::Color color_ser;
                transport_catalogue_serialization::Rgba rgba_color;
                rgba_color.set_red(color_arr[0].AsInt());
                rgba_color.set_green(color_arr[1].AsInt());
                rgba_color.set_blue(color_arr[2].AsInt());
                rgba_color.set_opacity(color_arr[3].AsDouble());
                *color_ser.mutable_rgba_color() = rgba_color;
                *render_settings_ser.add_palette() = color_ser;
            }
        }
        else {
            transport_catalogue_serialization::Color color_ser;
            color_ser.mutable_string_color()->set_color(color.AsString());
            *render_settings_ser.add_palette() = color_ser;
        }
    }
}

void SerializeStops(const json::Array& stop_requests, transport_catalogue_serialization::BaseRequests& base_requests_ser, const transport_catalogue_serialization::StopsNameMap& stops_names_map) {
    for (auto& stop_request : stop_requests) {
        transport_catalogue_serialization::Stop stop;
        stop.set_name_id(stops_names_map.stop_name_id().at(stop_request.AsMap().at("name"s).AsString()));
        stop.mutable_coordinates()->set_lat(stop_request.AsMap().at("latitude"s).AsDouble());
        stop.mutable_coordinates()->set_lng(stop_request.AsMap().at("longitude"s).AsDouble());
        for (auto& road_distance : stop_request.AsMap().at("road_distances"s).AsMap()) {
            transport_catalogue_serialization::RoadDistance* road_dist = stop.add_road_distance();
            road_dist->set_stop_id(stops_names_map.stop_name_id().at(road_distance.first));
            road_dist->set_distance(road_distance.second.AsInt());
        }
        *base_requests_ser.add_stops() = stop;
    }
}
void SerializeRoutes(const json::Array& bus_requests, transport_catalogue_serialization::BaseRequests& base_requests_ser, const transport_catalogue_serialization::StopsNameMap& stops_names_map) {
    for (auto bus_request : bus_requests) {
        transport_catalogue_serialization::Bus route;
        route.set_name(bus_request.AsMap().at("name"s).AsString());
        route.set_is_roundtrip(bus_request.AsMap().at("is_roundtrip"s).AsBool());
        json::Array stops = bus_request.AsMap().at("stops"s).AsArray();
        for (size_t i = 0; i + 1 < stops.size(); ++i  /* auto& stop : stops*/) {
            route.add_stops_id(stops_names_map.stop_name_id().at(stops[i].AsString()));
        }
        if (!route.is_roundtrip()) {
            route.add_stops_id(stops_names_map.stop_name_id().at(stops[stops.size() - 1].AsString()));
        }
        *base_requests_ser.add_routes() = route;
    }
}

transport_catalogue_serialization::BaseRequests SerializeBaseRequests(const json::Array& base_requests) {
    transport_catalogue_serialization::BaseRequests base_requests_ser;
    transport_catalogue_serialization::StopsNameMap stops_names_map;

    json::Array bus_requests;
    json::Array stop_requests;

    size_t i = 0;
    for (auto base_request : base_requests) {
        auto map_iter = base_request.AsMap().find("type");
        if (map_iter->second.AsString() == "Stop"s) {
            (*stops_names_map.mutable_stop_name_id())[base_request.AsMap().at("name"s).AsString()] = i;
            ++i;
            stop_requests.push_back(base_request);
        }
        if (map_iter->second.AsString() == "Bus"s) {
            bus_requests.push_back(base_request);
        }
    }
    *base_requests_ser.mutable_stops_name_map() = stops_names_map;
    SerializeStops(stop_requests, base_requests_ser, stops_names_map);
    SerializeRoutes(bus_requests, base_requests_ser, stops_names_map);
    return base_requests_ser;
}
transport_catalogue_serialization::RenderSettings SerializeRenderSettings(const json::Dict& render_settings) {
    transport_catalogue_serialization::RenderSettings render_settings_ser;

    render_settings_ser.set_bus_label_font_size(render_settings.at("bus_label_font_size"s).AsInt());
    render_settings_ser.add_bus_label_offset(render_settings.at("bus_label_offset"s).AsArray().at(0).AsDouble());
    render_settings_ser.add_bus_label_offset(render_settings.at("bus_label_offset"s).AsArray().at(1).AsDouble());
    render_settings_ser.set_height(render_settings.at("height"s).AsDouble());
    render_settings_ser.set_line_width(render_settings.at("line_width"s).AsDouble());
    render_settings_ser.set_padding(render_settings.at("padding"s).AsDouble());
    render_settings_ser.set_stop_label_font_size(render_settings.at("stop_label_font_size"s).AsInt());
    render_settings_ser.add_stop_label_offset(render_settings.at("stop_label_offset"s).AsArray().at(0).AsDouble());
    render_settings_ser.add_stop_label_offset(render_settings.at("stop_label_offset"s).AsArray().at(1).AsDouble());
    render_settings_ser.set_stop_radius(render_settings.at("stop_radius"s).AsDouble());
    render_settings_ser.set_underlayer_width(render_settings.at("underlayer_width"s).AsDouble());
    render_settings_ser.set_width(render_settings.at("width"s).AsDouble());
    SerializeColorPalette(render_settings, render_settings_ser);
    SerializeUnredlayerColor(render_settings, render_settings_ser);
    return render_settings_ser;
}
transport_catalogue_serialization::RoutingSettings SerializeRoutingSettings(const json::Dict& routing_settings) {
    transport_catalogue_serialization::RoutingSettings routing_settings_ser;
    routing_settings_ser.set_bus_velocity(routing_settings.at("bus_velocity"s).AsDouble());
    routing_settings_ser.set_bus_wait_time(routing_settings.at("bus_wait_time"s).AsInt());
    return routing_settings_ser;
}

std::optional<transport_catalogue_serialization::TransportCatalogue> Deserialize(const json::Dict& process_requests) {
    std::ifstream in_file(process_requests.at("serialization_settings"s).AsMap().begin()->second.AsString(), std::ios::binary);
    transport_catalogue_serialization::TransportCatalogue t_catalogue;
    if (!t_catalogue.ParseFromIstream(&in_file)) {
        return std::nullopt;
    }
    return { std::move(t_catalogue) };
}


