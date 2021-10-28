#pragma once
#include "map_renderer.h"
#include "request_handler.h"
#include "json.h"


class JsonReader {
public:
	JsonReader(std::istream& in);
	void FillCatalogue(transport_catalogue::TransportCatalogue& tc);
	json::Dict GetRequests();
private:
	json::Document doc_;
};


namespace transport_catalogue {
	namespace transport_catalogue_input {
		using namespace json;
		void FillCatalog(Node& base_requests, TransportCatalogue& tc);
		namespace detail {
			Stop FillStop(const Node& node_stop, std::unordered_map<std::string, Dict>& from_to_dist);
			std::pair<Bus, std::vector<std::string>> FillRoute(Node& node_bus, TransportCatalogue& tc);
		}
	}
}
