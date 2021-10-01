#pragma once
#include "transport_catalogue.h"
#include "geo.h"

#include <vector>

namespace transport_catalogue {
namespace transport_catalogue_input {
	void FillCatalog(std::istream& is, TransportCatalogue& tc);
namespace detail {
	using DistFromToStr = std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>>;
	Stop FillStop(std::string_view query, DistFromToStr& from_to_dist);
	std::pair<Bus, std::vector<std::string>> FillRoute(std::string_view query, TransportCatalogue& tc);
	std::vector<std::string> FillUnicStops(std::string_view query, bool);
}
}
}
