#include "request_handler.h"

using namespace std;
using namespace ::renderer;

RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue& db, const MapRender& renderer) : db_(db), renderer_(renderer) {
}


svg::Document RequestHandler::RenderMap() const {
	return renderer_.RenderSVG(db_);
}

optional<RouteInfo> RequestHandler::GetBusStat(const string& bus_name) const {
	if (db_.FindRoute(bus_name)) {
		return db_.GetRouteInfo(bus_name);
	}
	return nullopt;
}

optional<set<string_view>> RequestHandler::GetBusesByStop(const string& stop_name) const {
	if (auto stop_it = db_.FindStop(stop_name)) {
		return db_.GetStopInfo(stop_it);
	}
	return nullopt;
}