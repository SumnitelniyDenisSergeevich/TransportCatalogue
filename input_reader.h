#pragma once
#include "trasport_catalogue.h"
#include "geo.h"

#include <string>
#include <vector>

void FillCatalog(std::istream& is, TransportCatalogue& tc);

std::pair<std::string, Coordinates> StopKey(std::string_view query);

std::tuple<std::string, std::vector<std::string>,bool > BusKey(std::string_view query);

std::vector<std::string> UnicStops(std::string_view query,size_t iter);
