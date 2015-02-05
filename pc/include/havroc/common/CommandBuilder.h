#ifndef COMMANDBUILDER_H_
#define COMMANDBUILDER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define START_SYNC 0xFF
#define DIV_SYNC   0xF0

#define TRACKING_CMD 0
#define SYSTEM_CMD	 1
#define MOTOR_CMD	 2

namespace havroc
{
	typedef struct _command_pkg
	{
		uint8_t  command;
		uint16_t length;
		char*	 data;
	} command_pkg;
	
	class CommandBuilder
	{
	public:
		CommandBuilder();
		~CommandBuilder();

		// Populate packet byte pointer and size from tracking command parameters
		static void build_tracking_command(char*& packet, size_t& size, bool on);

		// Populate packet byte pointer and size from system command parameters
		static void build_kill_system_command(char*& packet, size_t& size);

		// Populate packet byte pointer and size from motor command parameters
		static void build_motor_command(char*& packet, size_t& size, char* index, char* intensity, int length);

		// Populate command_pkg struct with data from a byte representation of a command packet and its size
		static void parse_command(command_pkg*& pkg, char*& packet, size_t size);

		// Populate packet command and size from command_pkg struct data for byte representation of command
		static void build_command(char*& packet, size_t& size, command_pkg*& pkg);

		// Validation function to verify that packet is a valid command
		static bool is_command(char*& packet, size_t size);

		// Print a nicely formatted command from packet*& for easy reading with controllable tab indents
		static void print_command(char*& packet, size_t size, int tabs = 0);

		// Print nicely formatted raw data from packet*& with controllable tab indents
		static void print_raw_bytes(char*& packet, size_t size, int offset, int tabs = 0);
	};

}/* namespace havroc */

#endif /* COMMANDBUILDER_H_ */
