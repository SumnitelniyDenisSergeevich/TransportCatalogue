#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"

class RequestHandler {

public:
    RequestHandler(const transport_catalogue::TransportCatalogue& db, renderer::MapRender& renderer);
    
    std::optional<RouteInfo> GetBusStat(const std::string& bus_name) const;

    std::optional < std::set<std::string_view>> GetBusesByStop(const std::string& stop_name) const;

    svg::Document RenderMap() const;
private:
    const transport_catalogue::TransportCatalogue& db_;
    renderer::MapRender& renderer_;
};

