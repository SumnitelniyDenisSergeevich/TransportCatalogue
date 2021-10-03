#pragma once
#include "transport_catalogue.h"
#include "geo.h"
#include "json.h"

namespace transport_catalogue {
	namespace transport_catalogue_input {
		using namespace json;
		void FillCatalog(Node& base_requests, TransportCatalogue& tc);
		namespace detail {
			Stop FillStop(const Node& node_stop, std::unordered_map<std::string, Dict>& from_to_dist);
			std::pair<Bus, Array> FillRoute(Node& node_bus, TransportCatalogue& tc);
		}
	}
}
