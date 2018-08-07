#include <iostream>
#include <memory>
#include <fastrtps/Domain.h>
#include "../../thirdparty/routing-service/src/RSManager.h"
#include <unistd.h>

int main(int argc, char * argv[])
{
    RSManager* tis_manager_ptr = nullptr;
    try
    {
        if (!(argc > 1)){
            std::cout << "Usage: " << argv[0] << " CONFIG_XML" << std::endl;
            return 0;
        }
        std::string path_to_config = argv[1];

        RSManager tis_manager(path_to_config);
        tis_manager_ptr = &tis_manager;

        if (tis_manager.isActive()){
            std::cout << "\n### TIS_NGSIv2 is running, press any key for quit ###" << std::endl;
            fflush(stdout);
            std::cin.ignore();
        }
        else{
            std::cout << "TIS_NGSIv2 error: no active bridges" << std::endl;
        }
        tis_manager_ptr = nullptr; // removeNGSIv2Subscriptions will be called due to out of scope
    }
    catch (...)
    {
        if (tis_manager_ptr)
        {
            tis_manager_ptr->onTerminate();
        }
    }
    return 0;
}
