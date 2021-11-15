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
    TransportRouter(const transport_catalogue::TransportCatalogue& catalogue) : catalogue_(catalogue), graph_(catalogue.GetStopsCount()) {

    }
    double ComputeTravelTime(double distance);

    void FillVertexes();
    void FillGraphWithRoutes();
    void SetRouteSettings(const double velocity, const int bus_wait_time);
    void SetRouter(const Router<double>& router);
    
    void AddEdge(const Bus& bus, size_t from, size_t to, bool last_ring = false);

    const DirectedWeightedGraph<double>& GetGraph() const;
    VertexId GetVertexId(std::string_view stop) const;
    const std::string_view GetVertexStop(size_t id) const;
    const Edge<double>& GetEdge(EdgeId edge_id) const;
    size_t GetBusWaitTime() const;

    std::pair<double, size_t> GetDistAndSpanCountBetweenStopsInRoute(const std::vector<const Stop*>& route_stops,
                                                                     const size_t from, const size_t to,
                                                                     bool last_ring = false) const;

    std::optional<Router<double>::RouteInfo> BuildRoute(VertexId from, VertexId to) const;

private:
    const transport_catalogue::TransportCatalogue& catalogue_;
    DirectedWeightedGraph<double> graph_;
    std::unordered_map<std::string_view, size_t> stop_id_;
    std::unordered_map<size_t, std::string_view> id_stop_;
    RouteSettings route_settings_;
    const Router<double>* router_ = nullptr;
};