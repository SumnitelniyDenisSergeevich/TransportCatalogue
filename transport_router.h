#pragma once

#include "transport_catalogue.h"
#include "router.h"

using namespace graph;

struct RouteSettings {
    inline static size_t bus_wait_time;
    inline static double bus_velocity;
};

class TransportRouter {
public:
    TransportRouter(size_t vertex_count) : graph_(vertex_count) {

    }
    double ComputeTravelTime(double distance);
    void AddEdge(const transport_catalogue::TransportCatalogue& catalogue, const std::vector<const Stop*> bus_stops,
        size_t stop_id_from, size_t stop_id_to, const Bus& bus);

    void FillVertexes(const std::deque<Stop>& stops);
    void FillGraphWithRoutes(const transport_catalogue::TransportCatalogue& catalogue);
    void SetRouteSettings(const double velocity, const int bus_wait_time);
    void SetRouter(const Router<double>& router);

    const DirectedWeightedGraph<double>& GetGraph() const;
    VertexId GetVertexId(std::string_view stop) const;
    const std::string_view GetVertexStop(size_t id) const;
    const Edge<double>& GetEdge(EdgeId edge_id) const;
    size_t GetBusWaitTime() const;

    std::optional<Router<double>::RouteInfo> BuildRoute(VertexId from, VertexId to) const;

private:
    DirectedWeightedGraph<double> graph_;
    std::unordered_map<std::string_view, size_t> stop_id_;
    std::unordered_map<size_t, std::string_view> id_stop_;
    RouteSettings route_settings_;
    const Router<double>* router_ = nullptr;
};