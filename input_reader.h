#pragma once
#include "transport_catalogue.h"
#include "geo.h"

#include <vector>

namespace Transport_Catalogue {
	namespace Transport_Catalogue_Input {
		void FillCatalog(std::istream& is, TransportCatalogue& tc);
		namespace detail {
			using DistFromToStr = std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>>;
			Transport_Catalogue::detail::StopFindResult FillStop(std::string_view query, DistFromToStr& from_to_dist);
			std::tuple<std::string, std::vector<std::string>, bool > FillRoute(std::string_view query);
			std::vector<std::string> FillUnicStops(std::string_view query, bool);
		}
	}
}