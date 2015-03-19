// havroc_library_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "havroc_library_dll.h"

#include <havroc\communications\NetworkManager.h>
#include <havroc\tracking\TrackingManager.h>
#include <havroc\common\CommandBuilder.h>

extern "C"
{

	/* Logging API functions */

	void CALLBACK_CONV hvr_set_logging_callback(log_callback callback)
	{
		havroc::Logger::set_remote_print_func(callback);
		havroc::Logger::start_logger();
	}



	/* Network API functions */

	int CALLBACK_CONV hvr_start_connection(char* ip)
	{
		int ret1 = 0;
		int ret2 = 0;

		havroc::NetworkManager::get()->set_reconnect(true);

		if (!ip || ip == "")
		{
			ret1 = havroc::NetworkManager::get()->start_tcp_client_left();
			ret2 = havroc::NetworkManager::get()->start_tcp_client_right();
		}
		else
		{
			ret1 = havroc::NetworkManager::get()->start_tcp_client_left(ip);
			ret2 = havroc::NetworkManager::get()->start_tcp_client_right(ip);
		}

		return ret1 > ret2 ? ret1 : ret2;
	}

	void CALLBACK_CONV hvr_async_start_connection(char* ip)
	{
		havroc::NetworkManager::get()->set_reconnect(true);

		if (!ip || ip == "")
		{
			havroc::NetworkManager::get()->async_start_tcp_client_left();
			havroc::NetworkManager::get()->async_start_tcp_client_right();
		}
		else
		{
			havroc::NetworkManager::get()->async_start_tcp_client_left(ip);
			havroc::NetworkManager::get()->async_start_tcp_client_right(ip);
		}
	}

	int CALLBACK_CONV hvr_end_connection()
	{
		int ret1 = havroc::NetworkManager::get()->stop_tcp_client_left();
		int ret2 = havroc::NetworkManager::get()->stop_tcp_client_right();

		return ret1 > ret2 ? ret1 : ret2;
	}

	bool CALLBACK_CONV hvr_is_network_active()
	{
		return havroc::NetworkManager::get()->is_tcp_client_right_active() && havroc::NetworkManager::get()->is_tcp_client_left_active();
	}

	void CALLBACK_CONV hvr_register_connect_callback(connect_callback callback)
	{
		havroc::NetworkManager::get()->register_connect_callback(callback, TCP_CLIENT_RIGHT | TCP_CLIENT_LEFT);
	}

	void CALLBACK_CONV hvr_register_disconnect_callback(disconnect_callback callback)
	{
		havroc::NetworkManager::get()->register_disconnect_callback(callback, TCP_CLIENT_RIGHT | TCP_CLIENT_LEFT);
	}

	void CALLBACK_CONV hvr_unregister_connect_callback(connect_callback callback)
	{
		havroc::NetworkManager::get()->unregister_connect_callback(callback, TCP_CLIENT_RIGHT | TCP_CLIENT_LEFT);
	}

	void CALLBACK_CONV hvr_unregister_disconnect_callback(disconnect_callback callback)
	{
		havroc::NetworkManager::get()->unregister_disconnect_callback(callback, TCP_CLIENT_RIGHT | TCP_CLIENT_LEFT);
	}



	/* Tracking API functions */

	int CALLBACK_CONV hvr_start_tracking_service()
	{
		BYTE* msg;
		size_t size;

		havroc::CommandBuilder::build_tracking_command(msg, size, true);

		return havroc::NetworkManager::get()->send(msg, size, TCP_CLIENT_RIGHT | TCP_CLIENT_LEFT, true);
	}

	int CALLBACK_CONV hvr_end_tracking_service()
	{
		BYTE* msg;
		size_t size;

		havroc::CommandBuilder::build_tracking_command(msg, size, false);

		return havroc::NetworkManager::get()->send(msg, size, TCP_CLIENT_RIGHT | TCP_CLIENT_LEFT, true);
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

		return havroc::NetworkManager::get()->send(msg, size, TCP_CLIENT_RIGHT | TCP_CLIENT_LEFT, true);
	}

}