#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

int main()
{
    TransportCatalogue ob1;
    FillCatalog(std::cin, ob1);
    ProcessingRequests(std::cout, ob1);
    
    system("pause");
}
