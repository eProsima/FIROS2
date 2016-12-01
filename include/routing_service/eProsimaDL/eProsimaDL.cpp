#include <iostream>
#include "eProsimaDL.h"

#ifdef _WIN32
	#include <Windows.h>
#else
	#include <dlfcn.h>
#endif

void* eProsimaLoadLibrary(const char *filename)
{
	void *libraryHandle = nullptr;

	if(filename != nullptr){
#ifdef _WIN32
	  libraryHandle = LoadLibrary(filename);
#else
		libraryHandle = dlopen(filename, RTLD_LAZY);
#endif
	}
	else{
		printf("Bad parameter (filename)\n");
	}

	if(libraryHandle == nullptr){
			printf("load fail\n");
	}


	return libraryHandle;
}

void* eProsimaGetProcAddress(void *libraryHandle, const char *functionName)
{
	void *functionPointer = nullptr;

	if(libraryHandle != nullptr && functionName != nullptr)
	{
#ifdef _WIN32
		functionPointer = GetProcAddress((HMODULE)libraryHandle, functionName);
		if (functionPointer == nullptr) printf("\n\nGAME OVER\n\n");
#else
		functionPointer = dlsym(libraryHandle, functionName);
#endif
	}
	else
	{
		printf("Bad parameters");
	}

	return functionPointer;
}

void eProsimaCloseLibrary(void *libraryHandle){
	#ifdef _WIN32
			//ToDo
	#else
			dlclose(libraryHandle);
	#endif
}
