#include <iostream>
#include <memory>

#include "RSManager.h"


int main(int argc, char * argv[]){

	if (!(argc > 1)){
		std::cout << "Error: No configuration file path specified" << std::endl;
		return 0;
	}
	std::string path_to_config = argv[1];

	RSManager firos_manager(path_to_config);

	if (firos_manager.isActive()){
		std::cout << "\n### Firos2 is running, press any key for quit ###" << std::endl;
		fflush(stdout);
		std::cin.ignore();
	}
	else{
		std::cout << "Firos2 ERROR" << std::endl;
	}
	std::cout << "closing..." << std::endl;
	return 0;
}
