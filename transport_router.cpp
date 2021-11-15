#include "transport_router.h"

double TransportRouter::ComputeTravelTime(double distance) {
    return distance / route_settings_.bus_velocity * 60.0 + route_settings_.bus_wait_time;
}

void TransportRouter::FillVertexes() {
    size_t counter = 0;
    for (const auto& stop : catalogue_.GetStops()) {
        stop_id_[stop.name] = counter;
        id_stop_[counter] = stop.name;
        ++counter;
    }
}

void TransportRouter::AddEdge(const Bus& bus, size_t from, size_t to, bool last_ring) {
    const std::vector<const Stop*> bus_stops = bus.bus_stops;
    auto [dist, span_count] = GetDistAndSpanCountBetweenStopsInRoute(bus_stops, from, to, last_ring);
    graph_.AddEdge(graph::Edge<double>{ stop_id_.at(bus_stops[from]->name),
        stop_id_.at(bus_stops[to]->name),
        ComputeTravelTime(dist),
        bus.name,
        span_count});
}

void TransportRouter::FillGraphWithRoutes() {
    const std::deque<Bus> buses = catalogue_.GetBuses();
    for (const Bus& bus : buses) {
        const std::vector<const Stop*> bus_stops = bus.bus_stops;
        for (size_t i = 0; i < bus_stops.size(); ++i) {
            if (bus.circle_key) {
                if (i != 0) {
                    AddEdge(bus, i, 0, true);
                }
                for (size_t j = i + 1; j < bus_stops.size(); ++j) {
                    AddEdge(bus, i, j);
                }
            }
            else {
                for (size_t j = 0; j < bus_stops.size(); ++j) {
                    if (i != j) {
                        AddEdge(bus, i, j);
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

std::pair<double, size_t> TransportRouter::GetDistAndSpanCountBetweenStopsInRoute(const std::vector<const Stop*>& route_stops, const size_t from, const size_t to,
                                                          bool last_ring) const {
    auto station_from = route_stops.begin() + from;
    auto station_to = route_stops.begin() + to;
    double dist = 0.0;
    size_t span_count = 0;

    if (last_ring) {
        for (; station_from + 1 < route_stops.end(); ++station_from) {
            dist += catalogue_.GetDistanceBetweenStops(*station_from, *next(station_from, 1));
            ++span_count;
        }
        dist += catalogue_.GetDistanceBetweenStops(*station_from, *route_stops.begin());
        ++span_count;
        return { dist / 1000.0, span_count };
    }

    if (station_from < station_to) {
        for (; station_from < station_to; ++station_from) {
            dist += catalogue_.GetDistanceBetweenStops(*station_from, *next(station_from, 1));
            ++span_count;
        }
    }
    else if (station_from > station_to) {
        for (; station_from > station_to; --station_from) {
            dist += catalogue_.GetDistanceBetweenStops(*station_from, *next(station_from, -1));
            ++span_count;
        }
    }
    return { dist / 1000.0, span_count };
}

std::optional<graph::Router<double>::RouteInfo> TransportRouter::BuildRoute(VertexId from, VertexId to) const {
    return router_->BuildRoute(from, to);
}