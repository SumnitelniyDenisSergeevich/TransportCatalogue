#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

using namespace Transport_Catalogue;

int main()
{
    TransportCatalogue ob1;
    ::Transport_Catalogue_Input::FillCatalog(std::cin, ob1);
    ::Transport_Catalogue_Output::ProcessingRequests(std::cin, std::cout, ob1);
    
    system("pause");
}
