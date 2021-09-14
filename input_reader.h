#pragma once
#include "transport_catalogue.h"
#include "geo.h"

#include <vector>

namespace Transport_Catalogue {
namespace Transport_Catalogue_Input {
	void FillCatalog(std::istream& is, TransportCatalogue& tc);
namespace detail {
	Transport_Catalogue::detail::StopFindResult StopKey(std::string_view query);
	std::tuple<std::string, std::vector<std::string>, bool > BusKey(std::string_view query);
	std::vector<std::string> UnicStops(std::string_view query, bool);
}
}
}
