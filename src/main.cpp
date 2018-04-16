#include <iostream>
#include <memory>

#include "../thirdparty/integration-services/src/ISManager.h"


int main(int argc, char * argv[]){

    if (!(argc > 1)){
        std::cout << "Usage: firos CONFIG_XML" << std::endl;
        return 0;
    }
    std::string path_to_config = argv[1];

    ISManager firos_manager(path_to_config);

    if (firos_manager.isActive()){
        std::cout << "\n### Firos2 is running, press any key for quit ###" << std::endl;
        fflush(stdout);
        std::cin.ignore();
    }
    else{
        std::cout << "Firos2 error: no active bridges" << std::endl;
    }
    return 0;
}
