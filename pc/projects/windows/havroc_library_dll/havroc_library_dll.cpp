// havroc_library_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "havroc_library_dll.h"

#include <havroc\communications\NetworkManager.h>
#include <havroc\tracking\TrackingManager.h>
#include <havroc\common\CommandBuilder.h>

extern "C"
{

	/* Network API functions */

	extern HAVROC_LIBRARY_DLL_API int CALLBACK_CONV		hvr_start_connection(char* ip)
	{
		if (!ip)
		{
			ip = CC3200_IP;
		}

		return havroc::NetworkManager::get()->start_tcp_client(ip);
	}

	extern HAVROC_LIBRARY_DLL_API int CALLBACK_CONV		hvr_end_connection()
	{
		return havroc::NetworkManager::get()->stop_tcp_client();
	}

	extern HAVROC_LIBRARY_DLL_API bool CALLBACK_CONV	hvr_is_active()
	{
		return havroc::NetworkManager::get()->is_tcp_client_active();
	}

	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_register_connect_callback(void(*connect_callback)())
	{
		havroc::NetworkManager::get()->register_connect_callback(connect_callback, TCP_CLIENT);
	}

	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_register_disconnect_callback(void(*disconnect_callback)())
	{
		havroc::NetworkManager::get()->register_disconnect_callback(disconnect_callback, TCP_CLIENT);
	}

	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_unregister_connect_callback(void(*connect_callback)())
	{
		havroc::NetworkManager::get()->unregister_connect_callback(connect_callback, TCP_CLIENT);
	}

	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_unregister_disconnect_callback(void(*disconnect_callback)())
	{
		havroc::NetworkManager::get()->unregister_disconnect_callback(disconnect_callback, TCP_CLIENT);
	}



	/* Tracking API functions */

	extern HAVROC_LIBRARY_DLL_API int CALLBACK_CONV		hvr_start_tracking_service()
	{
		BYTE* msg;
		size_t size;

		havroc::CommandBuilder::build_tracking_command(msg, size, true);

		return havroc::NetworkManager::get()->send(msg, size, TCP_CLIENT, true);
	}

	extern HAVROC_LIBRARY_DLL_API int CALLBACK_CONV		hvr_end_tracking_service()
	{
		BYTE* msg;
		size_t size;

		havroc::CommandBuilder::build_tracking_command(msg, size, false);

		return havroc::NetworkManager::get()->send(msg, size, TCP_CLIENT, true);
	}

	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_register_shoulder_callback(void(*shoulder_callback)(float, float, float, uint8_t))
	{
		shoulder_callback(10.0, 11.0, 12.0, RIGHT_ARM);
		havroc::TrackingManager::get()->register_shoulder_callback(shoulder_callback);
	}

	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_register_elbow_callback(void(*elbow_callback)(float, float, float, uint8_t))
	{
		havroc::TrackingManager::get()->register_elbow_callback(elbow_callback);
	}

	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_register_wrist_callback(void(*wrist_callback)(float, float, float, uint8_t))
	{
		havroc::TrackingManager::get()->register_wrist_callback(wrist_callback);
	}

	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_unregister_shoulder_callback(void(*shoulder_callback)(float, float, float, uint8_t))
	{
		havroc::TrackingManager::get()->unregister_shoulder_callback(shoulder_callback);
	}

	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_unregister_elbow_callback(void(*elbow_callback)(float, float, float, uint8_t))
	{
		havroc::TrackingManager::get()->unregister_elbow_callback(elbow_callback);
	}

	extern HAVROC_LIBRARY_DLL_API void CALLBACK_CONV	hvr_unregister_wrist_callback(void(*wrist_callback)(float, float, float, uint8_t))
	{
		havroc::TrackingManager::get()->unregister_wrist_callback(wrist_callback);
	}



	/* Motor API functions */

	extern HAVROC_LIBRARY_DLL_API int CALLBACK_CONV		hvr_send_motor_command(BYTE* index, BYTE* intensity, int length)
	{
		BYTE* msg;
		size_t size;

		havroc::CommandBuilder::build_motor_command(msg, size, index, intensity, length);

		return havroc::NetworkManager::get()->send(msg, size, TCP_CLIENT, true);
	}
}