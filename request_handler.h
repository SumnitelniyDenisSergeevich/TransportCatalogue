#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

class RequestHandler {

public:
    RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRender& renderer);
    
    std::optional<RouteInfo> GetBusStat(const std::string& bus_name) const;

    std::optional < std::set<std::string_view>> GetBusesByStop(const std::string& stop_name) const;

    svg::Document RenderMap() const;

    void SetTransportRouter(const TransportRouter& t_router);

    std::optional<Router<double>::RouteInfo> BuildRoute(VertexId from, VertexId to) const;
private:
    const transport_catalogue::TransportCatalogue& db_;
    const renderer::MapRender& renderer_;
    const TransportRouter* t_router_ = nullptr;
};

