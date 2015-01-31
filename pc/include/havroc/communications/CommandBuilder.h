#ifndef COMMANDBUILDER_H_
#define COMMANDBUILDER_H_

#include <string>
#include <math.h>

#include <boost/lexical_cast.hpp>

#define START_SYNC 0xFF
#define DIV_SYNC   0xF0

#define TRACKING_CMD 0
#define SYSTEM_CMD	 1
#define MOTOR_CMD	 2

namespace havroc
{

	class CommandBuilder
	{
	public:
		CommandBuilder();
		~CommandBuilder();

		static void build_tracking_command(char*& packet, size_t& size, bool on)
		{
			size = 5;
			packet = (char*)malloc(sizeof(char) * size);
			
			packet[0] = (char)START_SYNC;		// Start
			packet[1] = (char)TRACKING_CMD;		// Command
			packet[2] = (char)0;				// Number of bytes high
			packet[3] = (char)sizeof(bool);		// Number of bytes low
			packet[4] = (char)on;				// Data
		}

		static void build_kill_system_command(char*& packet, size_t& size)
		{
			size = 4;
			packet = (char*)malloc(sizeof(char) * size);

			packet[0] = (char)START_SYNC;
			packet[1] = (char)SYSTEM_CMD;
			packet[2] = (char)0;
			packet[3] = (char)0;
		}

		static void build_motor_command(char*& packet, size_t& size, char* index, char* intensity, int length)
		{
			uint16_t data_size = sizeof(char) * 2 * length + length;
			size = data_size + 4;

			packet = (char*)malloc(sizeof(char) * size);

			packet[0] = (char)START_SYNC;
			packet[1] = (char)MOTOR_CMD;
			packet[2] = (char)((data_size >> 8) & 0x00FF);
			packet[3] = (char)(data_size & 0x00FF);

			for (int i = 0; i < length; i++)
			{
				packet[i*3 + 4]	= (char)index[i];
				packet[i*3 + 5] = (char)intensity[i];
				packet[i*3 + 6] = (char)DIV_SYNC;
			}
		}

		static void print_command(char*& packet, size_t size, int tabs = 0)
		{
			if (!is_command(packet, size))
			{
				printf("Invalid command, start_sync or command_type mismatch occurred.\n");

				return;
			}

			int inner_tabs = tabs + 1;

			for (int j = 0; j < tabs; j++)
			{
				printf("\t");
			}

			printf("Packet start ===========================================\n");

			uint8_t command_type = packet[1];
			uint16_t data_size = ((((uint16_t)packet[2]) << 8) & 0xFF00) | (((uint16_t)packet[3]) & 0x00FF);

			for (int j = 0; j < inner_tabs; j++)
			{
				printf("\t");
			}

			printf("Command type: \t0x%02X ", (unsigned)(unsigned char)command_type);
			switch (command_type)
			{
				case(TRACKING_CMD) :
				{
					printf("(Tracking)");
					break;
				}
				case(SYSTEM_CMD) :
				{
					printf("(System)");
					break;
				}
				case(MOTOR_CMD) :
				{
					printf("(Motor)");
					break;
				}
			}
			printf("\n");

			for (int j = 0; j < inner_tabs; j++)
			{
				printf("\t");
			}

			printf("Data size: \t0x%04X (%d)\n", data_size, data_size);

			for (int j = 0; j < inner_tabs; j++)
			{
				printf("\t");
			}

			printf("Data:\n");

			print_raw_bytes(packet, size, 4, inner_tabs + 1);

			for (int j = 0; j < tabs; j++)
			{
				printf("\t");
			}

			printf("Packet end =============================================\n\n");
		}

		static void print_raw_bytes(char*& packet, size_t size, int offset, int tabs = 0)
		{
			if (size - offset > 0)
			{
				for (int j = 0; j < tabs; j++)
				{
					printf("\t");
				}
			}

			int i;
			for (i = offset; i < (int)size; i++)
			{
				printf("0x%02X ", (unsigned)(unsigned char)packet[i]);

				if (((i + offset + 1)% 8 == 0) && i > 0)
				{
					printf("\n");

					if (i != size - 1)
					{
						for (int j = 0; j < tabs; j++)
						{
							printf("\t");
						}
					}
				}	
			}

			if (((size - offset) % 8 != 0) && (size - offset) > 0)
			{
				printf("\n");
			}
		}

		static bool is_command(char*& packet, size_t size)
		{
			bool valid_start_sync   = packet[0] == (char)START_SYNC;
			bool valid_command_type = packet[1] == TRACKING_CMD || 
								      packet[1] == SYSTEM_CMD   || 
									  packet[1] == MOTOR_CMD;

			return valid_start_sync && valid_command_type;
		}
	};

}/* namespace havroc */

#endif /* COMMANDBUILDER_H_ */