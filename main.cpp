#include "json_reader.h"
#include "request_handler.h"
#include "json_builder.h"
#include "serialization.h"

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
        JsonReader json_reader(cin);
        TransportCatalogue catalogue;
        MapRender map;
        json_reader.FillCatalogue(catalogue);
        TransportRouter transport_router(catalogue);
        json_reader.FillRouteSettings(transport_router);
        json_reader.FillRenderSettings(map);
        Serialize(json_reader.GetRequests().at("serialization_settings"s).AsMap().begin()->second.AsString(), catalogue, map, transport_router);
    }else if (mode == "process_requests"sv) {
        JsonReader json_reader(cin);       
        std::optional<transport_catalogue_serialization::TransportCatalogue> t_k = Deserialize(json_reader.GetRequests().at("serialization_settings"s).AsMap().begin()->second.AsString());
        if (!t_k) {
            cerr << "wrong deserializetion" << endl;
            system("pause");
            return 0;
        }
        TransportCatalogue catalogue = DeserializeTransportCatalogue(t_k->base_transport_catalogue());
        MapRender map = DeserializeMapRender(t_k->render_settings());
        TransportRouter transport_router = DeserializeTransportRouter(t_k->routing_settings(), catalogue);
        transport_router.SetTransportCatalogue(catalogue);
        RequestHandler request_handler(catalogue, map);
        transport_router.FillVertexes();
        transport_router.FillGraphWithRoutes();
        request_handler.SetTransportRouter(transport_router);
        json_reader.SetTransportRouter(transport_router);
        json_reader.SetRequestHandler(request_handler);
        json_reader.PrintRequestsAnswer(cout);
    }else {
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
