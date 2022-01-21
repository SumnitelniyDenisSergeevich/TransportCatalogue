#pragma once
#include "json.h"

#include <transport_catalogue.pb.h>
#include <optional>

void Serialize(const json::Dict& make_base_requests);
transport_catalogue_serialization::BaseRequests SerializeBaseRequests(const json::Array& make_base_requests);
transport_catalogue_serialization::RenderSettings SerializeRenderSettings(const json::Dict& make_base_requests);
transport_catalogue_serialization::RoutingSettings SerializeRoutingSettings(const json::Dict& make_base_requests);

void SerializeUnredlayerColor(const json::Dict& render_settings, transport_catalogue_serialization::RenderSettings& render_settings_ser);
void SerializeColorPalette(const json::Dict& render_settings, transport_catalogue_serialization::RenderSettings& render_settings_ser);
void SerializeStops(const json::Array& stop_requests, transport_catalogue_serialization::BaseRequests& base_requests_ser, const transport_catalogue_serialization::StopsNameMap& stops_names_map);
void SerializeRoutes(const json::Array& bus_requests, transport_catalogue_serialization::BaseRequests& base_requests_ser, const transport_catalogue_serialization::StopsNameMap& stops_names_map);
std::optional<transport_catalogue_serialization::TransportCatalogue> Deserialize(const json::Dict& process_requests);

