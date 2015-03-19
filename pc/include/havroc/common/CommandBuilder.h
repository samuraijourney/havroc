#ifndef COMMANDBUILDER_H_
#define COMMANDBUILDER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <boost/asio.hpp>

#include <havroc/tracking/TrackingDefinitions.h>

#define START_SYNC 255 // 0xFF
#define DIV_SYNC   240 // 0xF0

#define TRACKING_MOD 1
#define SYSTEM_MOD	 2
#define MOTOR_MOD	 3

#define TRACKING_STATE_CMD 10
#define TRACKING_DATA_CMD  11

#define MOTOR_DATA_CMD	   20

#define SYSTEM_KILL_CMD	   30

#define ERROR_CMD		   0

#define OVERHEAD_BYTES_CNT 5

namespace havroc
{
	typedef struct _command_pkg
	{
		uint8_t  command;
		uint8_t  module;
		uint16_t length;
		BYTE*	 data;
	} command_pkg;
	
	class CommandBuilder
	{
	public:
		CommandBuilder();
		~CommandBuilder();

		// Populate packet byte pointer and size from tracking command parameters
		static void build_tracking_command(BYTE*& packet, size_t& size, bool on);

		// Populate packet byte pointer and size from simulated tracking data
		static void build_tracking_data_sim_command(BYTE*& packet, size_t& size, float angles[ANGLES_PER_ARM], BYTE arm);

		// Populate packet byte pointer and size from system command parameters
		static void build_kill_system_command(BYTE*& packet, size_t& size);

		// Populate packet byte pointer and size from motor command parameters
		static void build_motor_command(BYTE*& packet, size_t& size, BYTE* index, BYTE* intensity, int length);

		// Populate packet byte pointer and size from error command and module parameters
		static void build_error_command(BYTE*& packet, size_t& size, uint8_t module, uint8_t error);

		// Populate command_pkg struct with data from a byte representation of a command packet and its size
		static void parse_command(command_pkg*& pkg, BYTE*& packet, size_t size);

		// Populate packet command and size from command_pkg struct data for byte representation of command
		static void build_command(BYTE*& packet, size_t& size, command_pkg*& pkg);

		// Validation function to verify that packet is a valid command
		static bool is_command(BYTE*& packet, size_t size);

		// Print a nicely formatted command from packet*& for easy reading with controllable tab indents
		static void print_command(BYTE*& packet, size_t size, int tabs = 0);

		// Print nicely formatted raw data from packet*& with controllable tab indents
		static void print_raw_bytes(BYTE*& packet, size_t size, int offset, int tabs = 0);
	};

}/* namespace havroc */

#endif /* COMMANDBUILDER_H_ */
