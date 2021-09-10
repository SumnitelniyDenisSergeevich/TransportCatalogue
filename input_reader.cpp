#include "input_reader.h"

#include <string_view>
#include <tuple>
#include <iostream>

using namespace std;

void FillCatalog(istream& is, TransportCatalogue& tc) {
    int vvod_count;
    vector<string> bus_query_s;

    is >> vvod_count;
    for (int i = 0; i < vvod_count; ++i) {
        string key;
        is >> key;
        if (key == "Stop"s) {
            getline(is, key);
            auto busstop_coord = StopKey(key.substr(1));
            tc.AddBusStop(busstop_coord.first, busstop_coord.second);
        }
        else if (key == "Bus"s) {
            getline(is, key);
            bus_query_s.push_back(key.substr(1));
        }
    }
    for (auto& bus_str : bus_query_s) {
        auto [name, stops, circle_key] = BusKey(bus_str);
        tc.AddRoute(name, stops, circle_key);
    }
}

std::pair<std::string, Coordinates> StopKey(std::string_view query) {
    auto iter = query.find(':');
    string busstop = static_cast<string>(query.substr(0, iter));
    query.remove_prefix(iter + 2);
    iter = query.find(',');
    string coord = static_cast<string>(query.substr(0, iter));
    double coord_first = stod(move(coord));
    query.remove_prefix(iter + 2);
    coord = static_cast<string>(query);
    double coord_second = stod(move(coord));
    return { busstop,{coord_first,coord_second} };
}

std::tuple<string, vector<string>, bool > BusKey(std::string_view query) {
    vector<string> unic_stops;
    bool circle_key = true;
    auto iter = query.find(':');
    string bus = static_cast<string>(query.substr(0, iter));
    query.remove_prefix(iter + 1); \
        iter = query.find('-');
    if (iter != query.npos) {
        circle_key = false;
        unic_stops = UnicStops(query, circle_key);
    }
    else {
        unic_stops = UnicStops(query, circle_key);
        unic_stops.pop_back();
    }
    return { bus,unic_stops, circle_key };
}

std::vector<std::string> UnicStops(std::string_view query, bool circle_flag) {
    vector<string> unic_stops;
    auto iter = (circle_flag ? query.find('>') : query.find('-'));
    while (iter != query.npos) {
        auto station = query.substr(1, iter);
        station.remove_suffix(2);
        query.remove_prefix(iter + 1);
        iter = circle_flag ? query.find('>') : query.find('-');
        unic_stops.push_back(static_cast<string>(station));
    }
    query.remove_prefix(1);
    unic_stops.push_back(static_cast<string>(query));
    return unic_stops;
}