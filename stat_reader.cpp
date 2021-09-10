#include "stat_reader.h"

#include <iomanip>

using namespace std;

void ProcessingRequests(std::ostream& out, TransportCatalogue& tc) {
	int requests_number;
	cin >> requests_number;
	string key;
	getline(cin, key);
	for (int i = 0; i < requests_number; ++i) {
		getline(cin, key);
		key = key.substr(4);
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
}