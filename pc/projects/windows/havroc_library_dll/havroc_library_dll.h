// HAVROC_LIBRARY_DLL.h
#ifndef HAVROC_LIBRARY_DLL_H_
#define HAVROC_LIBRARY_DLL_H_


#ifdef HAVROC_LIBRARY_DLL_EXPORTS
#define HAVROC_LIBRARY_DLL_API __declspec(dllexport) 
#else
#define HAVROC_LIBRARY_DLL_API  __declspec(dllimport) 
#endif


#endif /* HAVROC_LIBRARY_DLL_H_ */