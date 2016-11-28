#include <iostream>
#include <memory>

#include "../include/routing_service/RSManager.h"


int main(int argc, char * argv[]){

	if (!(argc > 1)){
		std::cout << "nope" << std::endl;
		return 0;
	}
	std::string path_to_config = argv[1];

	RSManager firos_manager(path_to_config);

	std::cout << "\n### Firos2 is running, press any key for quit ###" << std::endl;
	fflush(stdout);
	std::cin.ignore();
	std::cout << "closing..." << std::endl;
	return 0;
}
