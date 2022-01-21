#pragma once
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_router.h"
#include "json.h"
#include "serialization.h"


class JsonReader {
public:
	JsonReader(std::istream& in);
	void FillCatalogue(transport_catalogue::TransportCatalogue& tc, const transport_catalogue_serialization::BaseRequests& base_requests_ser) const;
	void FillRenderSettings(renderer::MapRender& map, const transport_catalogue_serialization::RenderSettings& render_settings_ser) const;
	void FillRouteSettings(TransportRouter&, const transport_catalogue_serialization::RoutingSettings& routing_settings_ser)const;
	void PrintRequestsAnswer(std::ostream& out) const;
	void SetTransportRouter(const TransportRouter& t_router);
	void SetRequestHandler(RequestHandler& rh);
	const json::Dict& GetRequests() const;
private:
	json::Node GetStatStop(const json::Node& stop_node) const;
	json::Node GetStatRoute(const json::Node& route_node) const;
	json::Node GetRoute(const json::Node& route_node) const;		//new

	json::Document doc_;
	RequestHandler* rh_ = nullptr;
	const TransportRouter* t_router_ = nullptr;
};


namespace transport_catalogue {
	namespace transport_catalogue_input {
		using namespace json;
		void FillCatalog(TransportCatalogue& tc, const transport_catalogue_serialization::BaseRequests& base_requests_ser);
		namespace detail {
			Stop FillStop(const transport_catalogue_serialization::Stop& stop, 
							const std::map<std::uint32_t, std::string>& id_stops_name_std,
							std::unordered_map<std::string, std::unordered_map<std::string, int>>& from_to_dist);
			std::pair<Bus, std::vector<std::string>> FillRoute(const transport_catalogue_serialization::Bus& bus, const std::map<uint32_t, std::string>& id_stops_name_std, TransportCatalogue& tc);
		}
	}
}
