// HAVROC_LIBRARY_DLL.h
#ifndef HAVROC_LIBRARY_DLL_H_
#define HAVROC_LIBRARY_DLL_H_

#define CALLBACK_CONV __cdecl	// Callback convention

#ifdef HAVROC_LIBRARY_DLL_EXPORTS
#define HAVROC_LIBRARY_DLL_API __declspec(dllexport) 
#else
#define HAVROC_LIBRARY_DLL_API  __declspec(dllimport) 
#endif

#include <stdint.h>

#include <havroc\common\Logger.h>

// havroc_library_dll.cpp : Defines the exported functions for the DLL application.

extern "C"
{
	typedef havroc::RemotePrintCallback log_callback;
	
	/* Network API callbacks */
	typedef void(*connect_callback)();
	typedef void(*disconnect_callback)();

	/* Tracking API callbacks */
	typedef void(*shoulder_callback)(float, float, float, uint8_t);
	typedef void(*elbow_callback)(float, float, float, uint8_t);
	typedef void(*wrist_callback)(float, float, float, uint8_t);

	/* Logging API functions */
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_set_logging_callback(log_callback callback);

	/* Network API functions */
	extern HAVROC_LIBRARY_DLL_API int  CALLBACK_CONV	hvr_start_connection(char* ip);
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_async_start_connection(char* ip);
	extern HAVROC_LIBRARY_DLL_API int  CALLBACK_CONV	hvr_end_connection();
	extern HAVROC_LIBRARY_DLL_API bool CALLBACK_CONV	hvr_is_network_active();
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_register_connect_callback(connect_callback callback);
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_register_disconnect_callback(disconnect_callback callback);
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_unregister_connect_callback(connect_callback callback);
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_unregister_disconnect_callback(disconnect_callback callback);

	/* Tracking API functions */
	extern HAVROC_LIBRARY_DLL_API int  CALLBACK_CONV	hvr_start_tracking_service();
	extern HAVROC_LIBRARY_DLL_API int  CALLBACK_CONV	hvr_end_tracking_service();
	extern HAVROC_LIBRARY_DLL_API bool CALLBACK_CONV	hvr_is_tracking_active();
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_register_shoulder_callback(shoulder_callback callback);
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_register_elbow_callback(elbow_callback callback);
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_register_wrist_callback(wrist_callback callback);
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_unregister_shoulder_callback(shoulder_callback callback);
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_unregister_elbow_callback(elbow_callback callback);
	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_unregister_wrist_callback(wrist_callback callback);

	/* Motor API functions */
	extern HAVROC_LIBRARY_DLL_API int  CALLBACK_CONV	hvr_send_motor_command(BYTE* index, BYTE* intensity, int length);
}

#endif /* HAVROC_LIBRARY_DLL_H_ */