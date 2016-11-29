#ifndef _EPROSIMA_C_SYS_EPROSIMADL_H_
#define _EPROSIMA_C_SYS_EPROSIMADL_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

	/**
	 * \brief This function loads a dynamic library.
	 *
	 * \param filename The name of the dynamic library that will be loaded. Cannot be NULL.
	 * \return Pointer to the handle of the dynamic library. In error case NULL pointer is returned.
	 */
	void* eProsimaLoadLibrary(const char *filename);

	/**
	 * \brief This function loads a function pointer that it's in a dynamic library.
	 *
	 * \param libraryHandle The handle of the dynamic library. Cannot be NULL.
	 * \param functionName The name of the function. Cannot be NULL.
	 * \return Pointer to the function. In error case NULL pointer is returned.
	 */
	 void* eProsimaGetProcAddress(void *libraryHandle, const char *functionName);
	 void eProsimaCloseLibrary(void *libraryHandle);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _EPROSIMA_C_SYS_EPROSIMADL_H_
