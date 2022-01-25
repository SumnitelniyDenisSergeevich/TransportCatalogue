#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include <transport_catalogue.pb.h>
#include <optional>

void WriteDataToFile(const std::string output_file, transport_catalogue_serialization::TransportCatalogue transport_catalogue);
transport_catalogue_serialization::TransportCatalogue SerializeTransportCatalogue(const transport_catalogue::TransportCatalogue& catalogue,
                                                                                  const renderer::MapRender map, 
                                                                                  const TransportRouter& router);
void Serialize(const std::string output_file, const transport_catalogue::TransportCatalogue& catalogue, const renderer::MapRender map, const TransportRouter& router);
transport_catalogue_serialization::BaseTransportCatalogue SerializeBaseTransportCatalogue(const transport_catalogue::TransportCatalogue& catalogue);
transport_catalogue_serialization::RenderSettings SerializeRenderSettings(const renderer::MapRender map);
transport_catalogue_serialization::RoutingSettings SerializeRoutingSettings(const TransportRouter& router);

void SerializeSetUnredlayerColor(const svg::Color color, transport_catalogue_serialization::RenderSettings& render_settings_ser);
void SerializeSetColorPalette(const std::vector<svg::Color>& colors, transport_catalogue_serialization::RenderSettings& render_settings_ser);
transport_catalogue_serialization::Stop SerializeStop(const Stop& stop, const transport_catalogue_serialization::StopNameToId& stop_name_to_id);
transport_catalogue_serialization::Bus SerializeRoute(const Bus& stop, const transport_catalogue_serialization::StopNameToId& stop_name_to_id);
transport_catalogue_serialization::DistanceBetweenStops SerializeDistanceBetweenStops(const transport_catalogue::DistFromTo from_to_dist, const transport_catalogue_serialization::StopNameToId& stop_name_to_id);

std::optional<transport_catalogue_serialization::TransportCatalogue> Deserialize(const std::string input_file);

transport_catalogue::TransportCatalogue DeserializeTransportCatalogue(const transport_catalogue_serialization::BaseTransportCatalogue& base_transport_catalogue);
renderer::MapRender DeserializeMapRender(const transport_catalogue_serialization::RenderSettings& render_settings_ser);
TransportRouter DeserializeTransportRouter(const transport_catalogue_serialization::RoutingSettings& routing_settings_ser, const transport_catalogue::TransportCatalogue catalogue);

Stop DeserializeStop(const transport_catalogue_serialization::Stop& stop, const std::map<uint32_t, std::string>& id_to_stop_name);
Bus DeserializeRoute(const transport_catalogue_serialization::Bus& bus,
    const std::map<uint32_t, std::string>& id_stops_name_std,
    transport_catalogue::TransportCatalogue& catalogue);


