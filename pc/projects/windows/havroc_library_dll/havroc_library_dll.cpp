// havroc_library_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "havroc_library_dll.h"

#include <havroc\communications\NetworkManager.h>
#include <havroc\tracking\TrackingManager.h>
#include <havroc\common\CommandBuilder.h>

extern "C"
{

	void CALLBACK_CONV hvr_register_mirror_callback(mirror_callback callback)
	{
		callback(1.123f, 2.345f, 3.456f, (BYTE)RIGHT_ARM);
		callback(4.567f, 5.678f, 6.789f, (BYTE)RIGHT_ARM);
		callback(7.890f, 8.909f, 9.098f, (BYTE)RIGHT_ARM);
		callback(0.987f, 9.876f, 8.765f, (BYTE)LEFT_ARM);
		callback(7.654f, 6.543f, 5.432f, (BYTE)LEFT_ARM);

		havroc::TrackingManager::get()->register_shoulder_callback(callback);
	}

	/* Network API functions */

	int CALLBACK_CONV hvr_start_connection(char* ip)
	{
		if (!ip)
		{
			ip = CC3200_IP;
		}

		return havroc::NetworkManager::get()->start_tcp_client(ip);
	}

	int CALLBACK_CONV hvr_end_connection()
	{
		return havroc::NetworkManager::get()->stop_tcp_client();
	}

	bool CALLBACK_CONV hvr_is_network_active()
	{
		return havroc::NetworkManager::get()->is_tcp_client_active();
	}

	void CALLBACK_CONV hvr_register_connect_callback(connect_callback callback)
	{
		havroc::NetworkManager::get()->register_connect_callback(callback, TCP_CLIENT);
	}

	void CALLBACK_CONV hvr_register_disconnect_callback(disconnect_callback callback)
	{
		havroc::NetworkManager::get()->register_disconnect_callback(callback, TCP_CLIENT);
	}

	void CALLBACK_CONV hvr_unregister_connect_callback(connect_callback callback)
	{
		havroc::NetworkManager::get()->unregister_connect_callback(callback, TCP_CLIENT);
	}

	void CALLBACK_CONV hvr_unregister_disconnect_callback(disconnect_callback callback)
	{
		havroc::NetworkManager::get()->unregister_disconnect_callback(callback, TCP_CLIENT);
	}



	/* Tracking API functions */

	int CALLBACK_CONV hvr_start_tracking_service()
	{
		BYTE* msg;
		size_t size;

		havroc::CommandBuilder::build_tracking_command(msg, size, true);

		return havroc::NetworkManager::get()->send(msg, size, TCP_CLIENT, true);
	}

	int CALLBACK_CONV hvr_end_tracking_service()
	{
		BYTE* msg;
		size_t size;

		havroc::CommandBuilder::build_tracking_command(msg, size, false);

		return havroc::NetworkManager::get()->send(msg, size, TCP_CLIENT, true);
	}

	bool CALLBACK_CONV hvr_is_tracking_active()
	{
		return havroc::TrackingManager::get()->is_active();
	}

	void CALLBACK_CONV hvr_register_shoulder_callback(shoulder_callback callback)
	{
		havroc::TrackingManager::get()->register_shoulder_callback(callback);
	}

	void CALLBACK_CONV hvr_register_elbow_callback(elbow_callback callback)
	{
		havroc::TrackingManager::get()->register_elbow_callback(callback);
	}

	void CALLBACK_CONV hvr_register_wrist_callback(wrist_callback callback)
	{
		havroc::TrackingManager::get()->register_wrist_callback(callback);
	}

	void CALLBACK_CONV hvr_unregister_shoulder_callback(shoulder_callback callback)
	{
		havroc::TrackingManager::get()->unregister_shoulder_callback(callback);
	}

	void CALLBACK_CONV hvr_unregister_elbow_callback(elbow_callback callback)
	{
		havroc::TrackingManager::get()->unregister_elbow_callback(callback);
	}

	void CALLBACK_CONV hvr_unregister_wrist_callback(wrist_callback callback)
	{
		havroc::TrackingManager::get()->unregister_wrist_callback(callback);
	}



	/* Motor API functions */

	int CALLBACK_CONV hvr_send_motor_command(BYTE* index, BYTE* intensity, int length)
	{
		BYTE* msg;
		size_t size;

		havroc::CommandBuilder::build_motor_command(msg, size, index, intensity, length);

		return havroc::NetworkManager::get()->send(msg, size, TCP_CLIENT, true);
	}
}