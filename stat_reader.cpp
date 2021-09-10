#include "stat_reader.h"

#include <iomanip>

using namespace std;

void ProcessingRequests(std::ostream& out, TransportCatalogue& tc) {
	int requests_number;
	cin >> requests_number;
	for (int i = 0; i < requests_number; ++i) {
		string key;
		cin >> key;
		if (key == "Bus"s) {
			getline(cin, key);
			key = key.substr(1);
			if (!tc.FindRoute(key)) {
				out << "Bus "s << key << ": not found"s << '\n';
			}
			else {
				RoutInfoS rout_info = tc.RouteInfo(key);
				out << setprecision(6);
				out << "Bus "s << key << ": "s << rout_info.rout_stops_count << " stops on route, "s << rout_info.unic_rout_stop_count << " unique stops, "s <<
					rout_info.route_length << " route length"s << '\n';
			}
		}
		else if (key == "Stop"s) {
			getline(cin, key);
			key = key.substr(1);

			if (!tc.FindBusStop(key)) {
				out << "Stop "s << key << ": not found"s << '\n';
			}
			else {
				auto busstops = tc.BusStopInfo(key);
				if (!busstops.size()) {
					out << "Stop "s << key << ": no buses"s << '\n';
				}
				else {
					out << "Stop "s << key << ": buses"s;
					for (auto stop : tc.BusStopInfo(key)) {
						cout << ' ' << stop;
					}
					cout << '\n';
				}
			}
		}
	}
}