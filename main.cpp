#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "transport_router.h"

#include <iostream>
#include <filesystem>
#include <string_view>
#include <fstream> //delete

using namespace json;
using namespace transport_catalogue;
using namespace renderer;

using namespace std;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
        std::ifstream in("make_base.json"s);
       
        JsonReader json_reader(in);
        Serialize(json_reader.GetRequests());

        in.close();
    }
    else if (mode == "process_requests"sv) {
        std::ifstream in("process_requests.json"s);
        std::ofstream out("result.json"s);
        TransportCatalogue catalogue;
        MapRender map;
        RequestHandler request_handler(catalogue, map);
        JsonReader json_reader(in);
        
        json_reader.SetRequestHandler(request_handler);//new

        std::optional<transport_catalogue_serialization::TransportCatalogue> t_k = Deserialize(json_reader.GetRequests());
        if (!t_k) {
            cerr << "wrong deserializetion" << endl;
            system("pause");
            return 0;
        }
        transport_catalogue_serialization::BaseRequests base_requests_ser = t_k->base_requests();
        json_reader.FillCatalogue(catalogue, base_requests_ser);
        
        transport_catalogue_serialization::RenderSettings render_settings_ser = t_k->render_settings();
        json_reader.FillRenderSettings(map, render_settings_ser);
        transport_catalogue_serialization::RoutingSettings routing_settings_ser = t_k->routing_settings();
        TransportRouter transport_router(catalogue);
        json_reader.FillRouteSettings(transport_router, routing_settings_ser);

        transport_router.FillVertexes();
        transport_router.FillGraphWithRoutes();
        graph::Router router(transport_router.GetGraph());
        transport_router.SetRouter(router);
        request_handler.SetTransportRouter(transport_router);
        json_reader.SetTransportRouter(transport_router);

        
        json_reader.PrintRequestsAnswer(out);

        in.close();
        out.close();
    }
    else {
        PrintUsage();
        return 1;
    }
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
