#include "json.h"
#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"

using namespace json;
using namespace transport_catalogue;

using namespace std;
#include <string>
#include <istream>

int main() {
    TransportCatalogue ob1;
    Document doc(Load(std::cin));
    auto iter = doc.GetRoot().AsMap().at("base_requests"s);
    ::transport_catalogue_input::FillCatalog(iter, ob1);
    iter = doc.GetRoot().AsMap().at("stat_requests"s);
    ::transport_catalogue_output::ProcessingRequests(iter, std::cout, ob1);
    system("pause");
}
