#include <iostream>

#include <havroc/communications/NetworkManager.h>
#include <havroc/tracking/TrackingManager.h>
#include <havroc/common/CommandManager.h>
#include <havroc/common/CommandBuilder.h>

#include "TrackingSimController.h"

TrackingSimController* sim_controller;

bool start_tracking = false;

int prompt_for_action()
{
	int choice = 0;
	do
	{
		printf("Select action:\t1) Static Anterior Parallel\n\t\t2) Static Anterior Parallel 90\n\t\t3) Static Neutral Boxing\n\t\t");
		printf("4) Static Neutral Side\n\t\t5) Static Lateral Parallel\n\t\t6) Dynamic Boxing\n\t\t7) Cycle All\nChoice: ");

		std::cin >> choice;
	} while (choice > ACTIONS_COUNT+1 || choice < 1);

	std::cout << "Playing tracking data: " << choice << std::endl;

	return choice - 1;
}

void disconnect_handler()
{
	std::cout << "TCP Server stopped" << std::endl;
}

void connect_handler()
{
	std::cout << "TCP Server started" << std::endl;
}

void tracking_start_handler()
{
	if (!start_tracking)
	{
		std::cout << "Tracking service started" << std::endl;

		int choice = prompt_for_action();
		sim_controller->play(true, choice != ACTIONS_COUNT ? choice : -1);

		start_tracking = true;
	}
}

void tracking_end_handler()
{
	if (start_tracking)
	{
		std::cout << "Tracking service ended" << std::endl;

		sim_controller->stop();

		start_tracking = false;
	}
}

int main()
{
	havroc::NetworkManager*  n_manager = havroc::NetworkManager::get();
	havroc::TrackingManager* t_manager = havroc::TrackingManager::get();

	sim_controller = new TrackingSimController("TrackingData_CSV.csv");

	n_manager->set_reconnect(true);

	n_manager->set_connections(TCP_SERVER);
	n_manager->register_connect_callback(&connect_handler);
	n_manager->register_disconnect_callback(&disconnect_handler);

	t_manager->register_start_callback(&tracking_start_handler);
	t_manager->register_end_callback(&tracking_end_handler);

	if(int error = n_manager->start_tcp_server())
	{
		printf("TCP Server failed to start with error code: %d\n", error);
		printf("Terminating program\n");
		
		return -1;
	}

	while (n_manager->get_reconnect() || n_manager->is_active())
	{
		if (start_tracking)
		{
			int choice = prompt_for_action();

			if (start_tracking)
			{
				sim_controller->change(true, choice != ACTIONS_COUNT ? choice : -1);
			}
		}
		else
		{
			std::cout << "Waiting for tracking service to be requested" << std::endl;
			while (!start_tracking)
			{
				boost::this_thread::sleep(boost::posix_time::milliseconds(100));
			}
		}
	}

	delete sim_controller;

	return 0;
}