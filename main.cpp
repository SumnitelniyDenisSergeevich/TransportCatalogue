#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"

#include <fstream>

using namespace json;
using namespace transport_catalogue;
using namespace renderer;

int main() {
    std::ifstream in("s10_final_opentest_3.json"s);
    std::ofstream out("result.json"s);

    TransportCatalogue catalogue;
    MapRender map;
    JsonReader json_reader(in);

    RequestHandler request_handler(catalogue, map, json_reader.GetRequests());

    json_reader.FillCatalogue(catalogue);
    request_handler.FillRenderSettings();
    request_handler.ProcessRequests(out);

    system("pause");
}
