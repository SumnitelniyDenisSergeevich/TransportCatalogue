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
    RequestHandler request_handler(catalogue, map);
    JsonReader json_reader(in, request_handler);

    json_reader.FillCatalogue(catalogue);
    json_reader.FillRenderSettings(map);
    json_reader.PrintRequestsAnswer(out);

    system("pause");
}
