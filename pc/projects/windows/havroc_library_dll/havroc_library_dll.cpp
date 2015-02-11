// havroc_library_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "havroc_library_dll.h"

#include <havroc\communications\NetworkManager.h>
#include <havroc\common\CommandBuilder.h>

extern "C"
{
	extern HAVROC_LIBRARY_DLL_API int	hvr_start_connection()
	{
		return havroc::NetworkManager::get()->start_tcp_client("127.0.0.1");
	}

	extern HAVROC_LIBRARY_DLL_API int	hvr_end_connection()
	{
		return havroc::NetworkManager::get()->stop_tcp_client();
	}

	extern HAVROC_LIBRARY_DLL_API bool	hvr_is_active()
	{
		return havroc::NetworkManager::get()->is_tcp_client_active();
	}

	extern HAVROC_LIBRARY_DLL_API int	hvr_send_motor_command(char* index, char* intensity, int length)
	{
		char* msg;
		size_t size;

		havroc::CommandBuilder::build_motor_command(msg, size, index, intensity, length);

		return havroc::NetworkManager::get()->send(msg, size, true);
	}
}