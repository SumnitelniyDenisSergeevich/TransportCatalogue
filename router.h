#pragma once

#include "graph.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include <iostream> // delete

namespace graph {

    template <typename Weight>
    class Router {
    private:
        using Graph = DirectedWeightedGraph<Weight>;

    public:
        explicit Router(const Graph& graph);

        struct RouteInfo {
            Weight weight;
            std::vector<EdgeId> edges;
        };

        std::optional<RouteInfo> BuildRoute(VertexId from, VertexId to) const;

    private:
        struct RouteInternalData {
            Weight weight;
            std::optional<EdgeId> prev_edge;
        };
        using RoutesInternalData = std::vector<std::vector<std::optional<RouteInternalData>>>;

        void InitializeRoutesInternalData(const Graph& graph) {
            const size_t vertex_count = graph.GetVertexCount();
            for (VertexId vertex = 0; vertex < vertex_count; ++vertex) {
                routes_internal_data_[vertex][vertex] = RouteInternalData{ ZERO_WEIGHT, std::nullopt };
                for (const EdgeId edge_id : graph.GetIncidentEdges(vertex)) {
                    const auto& edge = graph.GetEdge(edge_id);
                    if (edge.weight < ZERO_WEIGHT) {
                        throw std::domain_error("Edges' weights should be non-negative");
                    }
                    auto& route_internal_data = routes_internal_data_[vertex][edge.to];
                    if (!route_internal_data || route_internal_data->weight > edge.weight) {
                        route_internal_data = RouteInternalData{ edge.weight, edge_id };
                    }
                }
            }
        }

        void RelaxRoute(VertexId vertex_from, VertexId vertex_to, const RouteInternalData& route_from,
            const RouteInternalData& route_to) {
            auto& route_relaxing = routes_internal_data_[vertex_from][vertex_to];
            const Weight candidate_weight = route_from.weight + route_to.weight;
            if (!route_relaxing || candidate_weight < route_relaxing->weight) {
                route_relaxing = { candidate_weight,
                                  route_to.prev_edge ? route_to.prev_edge : route_from.prev_edge };
            }
        }

        void RelaxRoutesInternalDataThroughVertex(size_t vertex_count, VertexId vertex_through) {
            for (VertexId vertex_from = 0; vertex_from < vertex_count; ++vertex_from) {
                if (const auto& route_from = routes_internal_data_[vertex_from][vertex_through]) {
                    for (VertexId vertex_to = 0; vertex_to < vertex_count; ++vertex_to) {
                        if (const auto& route_to = routes_internal_data_[vertex_through][vertex_to]) {
                            RelaxRoute(vertex_from, vertex_to, *route_from, *route_to);
                        }
                    }
                }
            }
        }

        static constexpr Weight ZERO_WEIGHT{};
        const Graph& graph_;
        RoutesInternalData routes_internal_data_;
        const std::unordered_map<size_t, std::string_view>* id_stop_;
    };

    template <typename Weight>
    Router<Weight>::Router(const Graph& graph)
        : graph_(graph)
        , routes_internal_data_(graph.GetVertexCount(),
            std::vector<std::optional<RouteInternalData>>(graph.GetVertexCount()))
    {
        InitializeRoutesInternalData(graph);

        const size_t vertex_count = graph.GetVertexCount();
        for (VertexId vertex_through = 0; vertex_through < vertex_count; ++vertex_through) {
            RelaxRoutesInternalDataThroughVertex(vertex_count, vertex_through);
        }
    }

    template <typename Weight>
    std::optional<typename Router<Weight>::RouteInfo> Router<Weight>::BuildRoute(VertexId from,
        VertexId to) const {
        const auto& route_internal_data = routes_internal_data_.at(from).at(to);
        if (!route_internal_data) {
            return std::nullopt;
        }
        const Weight weight = route_internal_data->weight;
        std::vector<EdgeId> edges;
        for (std::optional<EdgeId> edge_id = route_internal_data->prev_edge;
            edge_id;
            edge_id = routes_internal_data_[from][graph_.GetEdge(*edge_id).from]->prev_edge)
        {
            edges.push_back(*edge_id);
        }
        std::reverse(edges.begin(), edges.end());

        return RouteInfo{ weight, std::move(edges) };
    }

    class Graph {
    public:
        Graph(size_t vertex_count) : graph_(vertex_count) {

        }

        void SetVertexId(const std::deque<Stop>& stops) {
            size_t counter = 0;
            for (const auto& stop : stops) {
                stop_id_[stop.name] = counter;
                id_stop_[counter] = stop.name;
                ++counter;
            }
        }
        void SetGraph(const transport_catalogue::TransportCatalogue& catalogue) { // ostalos
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
        void SetBusVelocity(const double velocity) {
            route_settings_.bus_velocity = velocity;
        }
        void SetBusWaitTime(const int bus_wait_time) {
            route_settings_.bus_wait_time = bus_wait_time;
        }
        void SetRouter(const Router<double>& router) {
            router_ = &router;
        }
        const DirectedWeightedGraph<double>& GetGraph() const {
            return graph_;
        }

        size_t GetVertexId(std::string_view stop)const {
            return stop_id_.at(stop);
        }
        const std::string_view GetVertexStop(size_t id)const {
            return id_stop_.at(id);
        }

        std::optional<graph::Router<double>::RouteInfo> BuildRoute(VertexId from, VertexId to) const {
            return router_->BuildRoute(from, to);
        }

        const Edge<double>& GetEdge(size_t edge_id) const {
            return graph_.GetEdge(edge_id);
        }

        size_t GetBusWaitTime() const {
            return route_settings_.bus_wait_time;
        }

    private:
        DirectedWeightedGraph<double> graph_;
        std::unordered_map<std::string_view, size_t> stop_id_;
        std::unordered_map<size_t, std::string_view> id_stop_;
        RouteSettings route_settings_;
        const Router<double>* router_;
    };

}  // namespace graph