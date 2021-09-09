#include "trasport_catalogue.h"

using namespace std;


void TransportCatalogue::AddRoute(int bus, vector<string>& stops, bool circle_key) {
	bus_to_stops_[bus] = move(stops);
	bus_to_cirkle_[bus] = move(circle_key);
	/*for (auto& [bus, stops] : bus_to_stops_) {
		cout << bus << ": "s;
		for (auto& stop : stops) {
			cout << stop << ", "s;
		}
		cout << ". "s << bus_to_cirkle_[bus] << endl;
	}*/
}

void TransportCatalogue::AddBusStop(std::string bus_stop, Coordinates bus_coordinates){
	stop_to_coordinates_[bus_stop] = move(bus_coordinates);
	/*for (auto& [stop, coord] : stop_to_coordinates_) {
		cout << stop << ':' << coord.lat << ' ' << coord.lng << endl;
	}*/
}

void TransportCatalogue::FindRoute() {

}

void TransportCatalogue::FindBusStop() {

}

void TransportCatalogue::RouteInfo() {

}