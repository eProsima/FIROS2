#include <iostream>
#include "eProsimaDL.h"

#ifdef __WIN32
	#include <Windows.h>
#else
	#include <dlfcn.h>
#endif

void* eProsimaLoadLibrary(const char *filename)
{
	void *libraryHandle = nullptr;

	if(filename != nullptr)
	{
#ifdef __WIN32
		libraryHandle = LoadLibrary(filename);
#else
		libraryHandle = dlopen(filename, RTLD_LAZY);
#endif
	}
	else
	{
		printf("Bad parameter (filename)");
	}

	return libraryHandle;
}

void* eProsimaGetProcAddress(void *libraryHandle, const char *functionName)
{
	void *functionPointer = nullptr;

	if(libraryHandle != nullptr && functionName != nullptr)
	{
#ifdef __WIN32
		functionPointer = GetProcAddress(libraryHandle, functionName);
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
	#ifdef __WIN32
			//ToDo
	#else
			dlclose(libraryHandle);
	#endif
}
