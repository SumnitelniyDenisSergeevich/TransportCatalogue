#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json_builder.h"

#include "router.h" //new

#include <fstream>

using namespace json;
using namespace transport_catalogue;
using namespace renderer;

using namespace std;

struct Waight {
    static size_t bus_wait_time;
    optional<size_t> bus_travel_time;
};

int main() {
    std::ifstream in("requests.json"s);
    std::ofstream out("result.json"s);

    TransportCatalogue catalogue;
    MapRender map;
    RequestHandler request_handler(catalogue, map);
    JsonReader json_reader(in, request_handler);


    json_reader.FillCatalogue(catalogue);
    graph::Graph graph(catalogue.GetStopsCount());
    json_reader.FillRouteSettings(graph);
    graph.SetVertexId(catalogue.GetStops());
    graph.SetGraph(catalogue);
    graph::Router router(graph.GetGraph());
    graph.SetRouter(router);
    json_reader.SetGraph(graph);
    json_reader.FillRenderSettings(map);
    json_reader.PrintRequestsAnswer(out);

    in.close();
    out.close();

    system("pause");
}

//int main() {
//    json::Print(
//        json::Document{
//            json::Builder{}
//            .StartDict()
//                .Key("key1"s).Value(123)
//                .Key("key2"s).Value("value2"s)
//                .Key("key3"s).StartArray()
//                    .Value(456)
//                    .StartDict().EndDict()
//                    .StartDict()
//                        .Key(""s)
//                        .Value(nullptr)
//                    .EndDict()
//                    .Value(""s)
//                .EndArray()
//            .EndDict()
//            .Build()
//        },
//        cout
//    );
//    cout << endl;
//
//    json::Print(
//        json::Document{
//                json::Builder{}
//                        .Value(json::Array{1, 1.23, "Hello"s})
//                        .Build()
//        },
//        cout
//    );
//    cout << endl;
//
//    json::Builder json_responses{};
//    auto var1 = json_responses.StartArray();
//    auto var2 = var1.StartDict();
//    auto var3 = var2.Key("hello"s);
//    auto var4 = var3.Value(123);
//    auto var5 = var4.EndDict();
//    auto var6 = var5.Value("just another string"s);
//    auto var7 = var6.StartArray();
//    auto var8 = var7.Value("123"s);
//    auto var9 = var8.EndArray();
//    auto var10 = var9.EndArray();
//
//
//    json::Print(json::Document{ var10.Build() }, cout);
//
//    system("pause");
//}
