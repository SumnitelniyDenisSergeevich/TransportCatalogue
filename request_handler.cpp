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

void RequestHandler::SetTransportRouter(const TransportRouter& t_router) {
	t_router_ = &t_router;
}

const DirectedWeightedGraph<double>& RequestHandler::GetGraph() const {
	return t_router_->GetGraph();
}
VertexId RequestHandler::GetVertexId(std::string_view stop) const {
	return t_router_->GetVertexId(stop);
}
const std::string_view RequestHandler::GetVertexStop(size_t id) const {
	return t_router_->GetVertexStop(id);
}
const Edge<double>& RequestHandler::GetEdge(EdgeId edge_id) const {
	return t_router_->GetEdge(edge_id);
}
size_t RequestHandler::GetBusWaitTime() const {
	return t_router_->GetBusWaitTime();
}

std::optional<Router<double>::RouteInfo> RequestHandler::BuildRoute(VertexId from, VertexId to) const {
	return t_router_->BuildRoute(from, to);
}