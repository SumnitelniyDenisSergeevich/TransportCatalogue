#include "transport_router.h"


void TransportRouter::FillGraph(const transport_catalogue::TransportCatalogue& catalogue) {
    size_t counter = 0;
    for (const auto& stop : catalogue.GetStops()) {
        stop_id_[stop.name] = counter;
        id_stop_[counter] = stop.name;
        ++counter;
    }
    const std::deque<Bus> buses = catalogue.GetBuses();
    for (const Bus& bus : buses) {
        const std::vector<const Stop*> bus_stops = bus.bus_stops;
        if (bus.circle_key) {
            for (size_t i = 0; i < bus_stops.size(); ++i) {
                if (i) {
                    auto [dist, span_count] = catalogue.GetDistAndSpanCountBetweenStopsInRoute(bus, i, 0, true);
                    graph_.AddEdge(graph::Edge<double>{ stop_id_.at(bus_stops[i]->name),
                        stop_id_.at(bus_stops[0]->name),
                        dist / route_settings_.bus_velocity * 60.0 + route_settings_.bus_wait_time,
                        bus.name,
                        span_count});
                }
                for (size_t j = i + 1; j < bus_stops.size(); ++j) {
                    auto [dist, span_count] = catalogue.GetDistAndSpanCountBetweenStopsInRoute(bus, i, j);
                    graph_.AddEdge(graph::Edge<double>{ stop_id_.at(bus_stops[i]->name),
                        stop_id_.at(bus_stops[j]->name),
                        dist / route_settings_.bus_velocity * 60.0 + route_settings_.bus_wait_time,
                        bus.name,
                        span_count});
                }
            }
        }
        else {
            for (size_t i = 0; i < bus_stops.size(); ++i) {
                for (size_t j = 0; j < bus_stops.size(); ++j) {
                    if (i != j) {
                        auto [dist, span_count] = catalogue.GetDistAndSpanCountBetweenStopsInRoute(bus, i, j);
                        graph_.AddEdge(graph::Edge<double>{ stop_id_.at(bus_stops[i]->name),
                            stop_id_.at(bus_stops[j]->name),
                            dist / route_settings_.bus_velocity * 60.0 + route_settings_.bus_wait_time,
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