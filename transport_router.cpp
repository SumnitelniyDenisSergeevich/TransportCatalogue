#include "transport_router.h"

double TransportRouter::ComputeTravelTime(double distance) {
    return distance / route_settings_.bus_velocity * 60.0 + route_settings_.bus_wait_time;
}

void TransportRouter::FillVertexes(const std::deque<Stop>& stops) {
    size_t counter = 0;
    for (const auto& stop : stops) {
        stop_id_[stop.name] = counter;
        id_stop_[counter] = stop.name;
        ++counter;
    }
}

void TransportRouter::AddEdge(const transport_catalogue::TransportCatalogue& catalogue, const std::vector<const Stop*> bus_stops,
                                size_t stop_id_from, size_t stop_id_to, const Bus& bus) {
    auto [dist, span_count] = catalogue.GetDistAndSpanCountBetweenStopsInRoute(bus_stops, stop_id_from, stop_id_to);
    graph_.AddEdge(graph::Edge<double>{ stop_id_.at(bus_stops[stop_id_from]->name),
        stop_id_.at(bus_stops[stop_id_to]->name),
        ComputeTravelTime(dist),
        bus.name,
        span_count});
}

void TransportRouter::FillGraphWithRoutes(const transport_catalogue::TransportCatalogue& catalogue) {
    const std::deque<Bus> buses = catalogue.GetBuses();
    for (const Bus& bus : buses) {
        const std::vector<const Stop*> bus_stops = bus.bus_stops;
        if (bus.circle_key) {
            for (size_t i = 0; i < bus_stops.size(); ++i) {
                if (i) {
                    auto [dist, span_count] = catalogue.GetDistAndSpanCountBetweenStopsInRoute(bus_stops, i, 0, true);
                    graph_.AddEdge(graph::Edge<double>{ stop_id_.at(bus_stops[i]->name),
                        stop_id_.at(bus_stops[0]->name),
                        dist / route_settings_.bus_velocity * 60.0 + route_settings_.bus_wait_time,
                        bus.name,
                        span_count});
                }
                for (size_t j = i + 1; j < bus_stops.size(); ++j) {
                    auto [dist, span_count] = catalogue.GetDistAndSpanCountBetweenStopsInRoute(bus_stops, i, j);
                    graph_.AddEdge(graph::Edge<double>{ stop_id_.at(bus_stops[i]->name),
                        stop_id_.at(bus_stops[j]->name),
                        ComputeTravelTime(dist),
                        bus.name,
                        span_count});
                }
            }
        }
        else {
            for (size_t i = 0; i < bus_stops.size(); ++i) {
                for (size_t j = 0; j < bus_stops.size(); ++j) {
                    if (i != j) {
                        auto [dist, span_count] = catalogue.GetDistAndSpanCountBetweenStopsInRoute(bus_stops, i, j);
                        graph_.AddEdge(graph::Edge<double>{ stop_id_.at(bus_stops[i]->name),
                            stop_id_.at(bus_stops[j]->name),
                            ComputeTravelTime(dist),
                            bus.name,
                            span_count});
                    }
                }
            }
        }
    }
}

void TransportRouter::SetRouteSettings(const double velocity, const int bus_wait_time) {
    route_settings_.bus_velocity = velocity;
    route_settings_.bus_wait_time = bus_wait_time;
}

void TransportRouter::SetRouter(const Router<double>& router) {
    router_ = &router;
}

const DirectedWeightedGraph<double>& TransportRouter::GetGraph() const {
    return graph_;
}

VertexId TransportRouter::GetVertexId(std::string_view stop)const {
    return stop_id_.at(stop);
}

const std::string_view TransportRouter::GetVertexStop(size_t id)const {
    return id_stop_.at(id);
}

const Edge<double>& TransportRouter::GetEdge(EdgeId edge_id) const {
    return graph_.GetEdge(edge_id);
}

size_t TransportRouter::GetBusWaitTime() const {
    return route_settings_.bus_wait_time;
}

std::optional<graph::Router<double>::RouteInfo> TransportRouter::BuildRoute(VertexId from, VertexId to) const {
    return router_->BuildRoute(from, to);
}