#include "input_reader.h"

#include <string_view>
#include <tuple>
#include <iostream>

using namespace std;

namespace Transport_Catalogue {
namespace Transport_Catalogue_Input {
    //using DistFromToStr = unordered_map<string, unordered_map<string, uint32_t>>;
void FillCatalog(istream& is, TransportCatalogue& tc) {
    int counter;
    vector<string> bus_query_s;
    vector<Transport_Catalogue::Stop> stops;

    detail::DistFromToStr from_to_dist;

    is >> counter;
    for (int i = 0; i < counter; ++i) {
        string key, bus_stop;
        is >> key;
        getline(is, bus_stop);
        if (key == "Stop"s) {
            Transport_Catalogue::Stop stop = detail::FillStop(bus_stop.substr(1), from_to_dist);
            stops.push_back(stop);
            tc.AddStop(stop);
        }
        else if (key == "Bus"s) {
            bus_query_s.push_back(bus_stop.substr(1));
        }
    }
    for (auto& bus_str : bus_query_s) {
        auto [bus, stops] = detail::FillRoute(bus_str, tc);
        tc.AddRoute(bus,stops);
    }
    // distance в from_to_dist
    for (auto& [from, to_dist] : from_to_dist) {
        for (auto& [to, dist] : to_dist) {
            tc.SetDistanceBetweenStops(tc.FindStop(from), tc.FindStop(to), dist);
        }
    }
}
namespace detail {
Transport_Catalogue::Stop FillStop(string_view query, DistFromToStr& from_to_dist) {
    Transport_Catalogue::Stop result;
    size_t iter = query.find(':');
    result.name = static_cast<string>(query.substr(0, iter));
    query.remove_prefix(iter + 2);
    iter = query.find(',');
    string str;
    result.coordinates.lat = stod(static_cast<string>(query.substr(0, iter)));
    query.remove_prefix(iter + 2);

    if (iter = query.find(','); iter != query.npos) {
        result.coordinates.lng = stod(static_cast<string>(query.substr(0, iter)));
        query.remove_prefix(iter + 2);

        iter = query.find(',');

        while (iter != query.npos) {
            iter = query.find('m');
            uint32_t dist = stoi(static_cast<string>(query.substr(0, iter)));
            query.remove_prefix(iter + 5);
            iter = query.find(',');
            from_to_dist[result.name][static_cast<string>(query.substr(0, iter))] = dist;
            query.remove_prefix(iter + 2);
            iter = query.find(',');
        }

        iter = query.find('m');
        uint32_t dist = stoi(static_cast<string>(query.substr(0, iter)));
        query.remove_prefix(iter + 5);
        iter = query.find(',');
        from_to_dist[result.name][static_cast<string>(query.substr(0, iter))] = dist;
    }
    else {
        result.coordinates.lng = stod(static_cast<string>(query));
    }
    return result;
}

std::pair<Bus,std::vector<std::string>> FillRoute(string_view query,TransportCatalogue& tc) {
    Bus result;
    vector<string> unic_stops;
    bool circle_key = true;
    size_t iter = query.find(':');
    result.name = static_cast<string>(query.substr(0, iter));
    query.remove_prefix(iter + 1); 
        iter = query.find('-');
    if (iter != query.npos) {
        circle_key = false;
        unic_stops = FillUnicStops(query, circle_key);
    }
    else {
        unic_stops = FillUnicStops(query, circle_key);
        unic_stops.pop_back();
    }
    result.circle_key = circle_key;

    for (auto& stop_str : unic_stops) {
        if (const Stop* stop = tc.FindStop(stop_str); stop) {
            result.bus_stops.push_back(stop);
        }
    }

    return { result, unic_stops };
}

vector<string> FillUnicStops(string_view query, bool circle_flag) {
    vector<string> unic_stops;
    size_t iter = (circle_flag ? query.find('>') : query.find('-'));
    while (iter != query.npos) {
        string_view station = query.substr(1, iter);
        station.remove_suffix(2);
        query.remove_prefix(iter + 1);
        iter = circle_flag ? query.find('>') : query.find('-');
        unic_stops.push_back(static_cast<string>(station));
    }
    query.remove_prefix(1);
    unic_stops.push_back(static_cast<string>(query));
    return unic_stops;
}

}
}
}