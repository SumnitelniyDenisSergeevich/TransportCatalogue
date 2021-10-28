#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json.h"

#include <iostream>

class RequestHandler {

public:
    RequestHandler(const transport_catalogue::TransportCatalogue& db, renderer::MapRender& renderer, const json::Dict requests);

	void FillRenderSettings();
    
    void ProcessRequests(std::ostream& out);

    json::Node StatStop(json::Node& stop_node);

    json::Node StatRoute(json::Node& route_node);

    svg::Document RenderMap(const transport_catalogue::TransportCatalogue& db) const;
private:
    const transport_catalogue::TransportCatalogue& db_;
    renderer::MapRender& renderer_;
    const json::Dict requests_;
};

