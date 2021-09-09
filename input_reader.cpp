#include "input_reader.h"


#include <string_view>
#include <iomanip>
#include <tuple>

using namespace std;

void FillCatalog(istream& is, TransportCatalogue& tc) {
    int vvod_count;

    std::deque<Bus> bus_query;
    std::deque<BusStop> busstop_query;

    cin >> vvod_count;
    for (int i = 0; i < vvod_count; ++i) {
        string key;
        is >> key;
        if (key == "Stop"s) {
            getline(is, key);
            string_view query = key;
            auto busstop_coord = StopKey(query);
            busstop_query.push_back({busstop_coord.first, busstop_coord.second});
            //tc.AddBusStop(busstop_coord.first, busstop_coord.second);
        }
        else if (key == "Bus"s) {
            getline(is, key);
            string_view query = key;
            auto [name, stops, circle_key] = BusKey(query);
            bus_query.push_back({ name,circle_key,stops });
            //tc.AddRoute(get<0>(bus_stops), get<1>(bus_stops), get<2>(bus_stops));
        }
    }

    // тут deque  очередей! 
}

std::pair<std::string, Coordinates> StopKey(std::string_view query) {
    query.remove_prefix(1);
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
    query.remove_prefix(1);
    auto iter = query.find(':');
    string bus = static_cast<string>(query.substr(0, iter));
    query.remove_prefix(iter+1);\
    iter = query.find('-');
    if (iter != query.npos) {
        unic_stops = UnicStops(query, iter);
        circle_key = false;
    }
    else {
        iter = query.find('>');
        unic_stops = UnicStops(query, iter);
        unic_stops.pop_back();
    }
    return { bus,unic_stops, circle_key };
}

std::vector<std::string> UnicStops(std::string_view query,size_t iter) {
    vector<string> unic_stops;
    while (iter != query.npos) {
        auto station = query.substr(1, iter);
        station.remove_suffix(2);
        query.remove_prefix(iter + 1);
        iter = query.find('>');
        unic_stops.push_back(static_cast<string>(station));
    }
    query.remove_prefix(1);
    unic_stops.push_back(static_cast<string>(query));
    return unic_stops;
}