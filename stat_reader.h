#pragma once
#include "transport_catalogue.h"

#include <iostream>

namespace transport_catalogue {
namespace transport_catalogue_output {
	void ProcessingRequests(std::istream&, std::ostream&, const TransportCatalogue&);
namespace detail {
	void PrintStop(std::istream& in, std::ostream& out, const TransportCatalogue& tc);
	void PrintRoute(std::istream& in, std::ostream& out, const TransportCatalogue& tc);
}
}
}