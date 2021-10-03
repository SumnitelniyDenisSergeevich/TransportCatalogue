#pragma once
#include "transport_catalogue.h"
#include "json.h"

#include <iostream>

namespace transport_catalogue {
	namespace transport_catalogue_output {
		void ProcessingRequests(json::Node& stat_requests, std::ostream&, const TransportCatalogue&);
		namespace detail {
			json::Node StatStop(json::Node& stop_node, const TransportCatalogue& tc);
			json::Node StatRoute(json::Node& route_node, const TransportCatalogue& tc);
		}
	}
}

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)

//class RequestHandler {
//public:
//    // MapRenderer понадобится в следующей части итогового проекта
//    RequestHandler(const transport_catalogue::TransportCatalogue& db) : db_(db) {
//
//    }
//
//    // Возвращает информацию о маршруте (запрос Bus)
//    std::optional<transport_catalogue::RoutInfoS> GetBusStat(const std::string_view& bus_name) const {
//        return db_.RouteInfo(static_cast<std::string>(bus_name));
//    }
//
//    // Возвращает маршруты, проходящие через
//    const std::unordered_set<transport_catalogue::Bus*>GetBusesByStop(const std::string_view& stop_name) const {
//        return {};
//    }
//
//    // Этот метод будет нужен в следующей части итогового проекта
//    //svg::Document RenderMap() const;
//
//private:
//    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
//    const transport_catalogue::TransportCatalogue& db_;
//    //const renderer::MapRenderer& renderer_;
//};
