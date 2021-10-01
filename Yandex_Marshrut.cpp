#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

using namespace transport_catalogue;

int main()
{
    TransportCatalogue ob1;
    ::transport_catalogue_input::FillCatalog(std::cin, ob1);
    ::transport_catalogue_output::ProcessingRequests(std::cin, std::cout, ob1);
    
    system("pause");
}
