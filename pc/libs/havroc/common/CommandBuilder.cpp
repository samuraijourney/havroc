#include <havroc/common/CommandBuilder.h>

namespace havroc
{

	void CommandBuilder::build_tracking_command(BYTE*& packet, size_t& size, bool on)
	{
		size = OVERHEAD_BYTES_CNT + 1;
		packet = (BYTE*)malloc(sizeof(BYTE) * size);

		packet[0] = (BYTE)START_SYNC;		  // Start
		packet[1] = (BYTE)TRACKING_MOD;		  // Module
		packet[2] = (BYTE)TRACKING_STATE_CMD; // Command
		packet[3] = (BYTE)0;				  // Number of bytes high
		packet[4] = (BYTE)sizeof(bool);		  // Number of bytes low
		packet[5] = (BYTE)on;				  // Data
	}

	void CommandBuilder::build_tracking_data_sim_command(BYTE*& packet, size_t& size, float angles[ANGLES_PER_ARM], BYTE arm)
	{
		uint16_t data_size = ANGLES_PER_ARM * sizeof(float) + 1;
		size = OVERHEAD_BYTES_CNT + data_size;

		packet = (BYTE*)malloc(sizeof(BYTE) * size);

		packet[0] = (BYTE)START_SYNC;
		packet[1] = (BYTE)TRACKING_MOD;
		packet[2] = (BYTE)TRACKING_DATA_CMD;
		packet[3] = (BYTE)((data_size >> 8) & 0x00FF);
		packet[4] = (BYTE)(data_size & 0x00FF);
		packet[5] = arm;

		for (int i = 0; i < ANGLES_PER_ARM; i++)
		{
			BYTE* p = reinterpret_cast<BYTE*>(&angles[i]);

			for (int j = 0; j < sizeof(float); j++)
			{
				packet[i * sizeof(float) + j + OVERHEAD_BYTES_CNT + 1] = p[j];
			}
		}
	}

	void CommandBuilder::build_kill_system_command(BYTE*& packet, size_t& size)
	{
		size = OVERHEAD_BYTES_CNT;
		packet = (BYTE*)malloc(sizeof(BYTE) * size);

		packet[0] = (BYTE)START_SYNC;
		packet[1] = (BYTE)SYSTEM_MOD;
		packet[2] = (BYTE)SYSTEM_KILL_CMD;
		packet[3] = (BYTE)0;
		packet[4] = (BYTE)0;
	}

	void CommandBuilder::build_motor_command(BYTE*& packet, size_t& size, BYTE* index, BYTE* intensity, int length)
	{
		uint16_t data_size = sizeof(BYTE) * 2 * length + length;
		size = data_size + OVERHEAD_BYTES_CNT;

		packet = (BYTE*)malloc(sizeof(BYTE) * size);

		packet[0] = (BYTE)START_SYNC;
		packet[1] = (BYTE)MOTOR_MOD;
		packet[2] = (BYTE)MOTOR_DATA_CMD;
		packet[3] = (BYTE)((data_size >> 8) & 0x00FF);
		packet[4] = (BYTE)(data_size & 0x00FF);

		for (int i = 0; i < length; i++)
		{
			packet[i * 3 + OVERHEAD_BYTES_CNT + 0] = (BYTE)index[i];
			packet[i * 3 + OVERHEAD_BYTES_CNT + 1] = (BYTE)intensity[i];
			packet[i * 3 + OVERHEAD_BYTES_CNT + 2] = (BYTE)DIV_SYNC;
		}
	}

	void CommandBuilder::build_error_command(BYTE*& packet, size_t& size, uint8_t module, uint8_t error)
	{
		size = OVERHEAD_BYTES_CNT + 1;
		packet = (BYTE*)malloc(sizeof(BYTE) * size);

		packet[0] = (BYTE)START_SYNC;
		packet[1] = (BYTE)module;
		packet[2] = (BYTE)ERROR_CMD;
		packet[3] = (BYTE)0;
		packet[4] = (BYTE)sizeof(uint8_t);
		packet[5] = (BYTE)error;
	}

	void CommandBuilder::build_command(BYTE*& packet, size_t& size, command_pkg*& pkg)
	{
		size = pkg->length + OVERHEAD_BYTES_CNT;
		packet = (BYTE*)malloc(sizeof(BYTE) * size);

		packet[0] = (BYTE)START_SYNC;
		packet[1] = (BYTE)pkg->module;
		packet[2] = (BYTE)pkg->command;
		packet[3] = (BYTE)((pkg->length >> 8) & 0x00FF);
		packet[4] = (BYTE)(pkg->length & 0x00FF);

		for (int i = 0; i < pkg->length; i++)
		{
			packet[i + OVERHEAD_BYTES_CNT] = pkg->data[i];
		}
	}

	void CommandBuilder::parse_command(command_pkg*& pkg, BYTE*& packet, size_t size)
	{
		pkg->module = packet[1];
		pkg->command = packet[2];
		pkg->length = ((((uint16_t)packet[3]) << 8) & 0xFF00) | (((uint16_t)packet[4]) & 0x00FF);
		pkg->data = (BYTE*)malloc(sizeof(BYTE)*(size - OVERHEAD_BYTES_CNT));

		for (int i = 0; i < (int)(size - OVERHEAD_BYTES_CNT); i++)
		{
			pkg->data[i] = packet[i + OVERHEAD_BYTES_CNT];
		}
	}

	bool CommandBuilder::is_command(BYTE*& packet, size_t size)
	{
		bool valid_start_sync   = packet[0] == (BYTE)START_SYNC;
		bool valid_command_type = packet[1] == (BYTE)TRACKING_MOD ||
								  packet[1] == (BYTE)SYSTEM_MOD	  ||
								  packet[1] == (BYTE)MOTOR_MOD;

		return valid_start_sync && valid_command_type;
	}

	void CommandBuilder::print_command(BYTE*& packet, size_t size, int tabs)
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

		uint8_t module_type = packet[1];
		uint8_t command_type = packet[2];
		uint16_t data_size = ((((uint16_t)packet[3]) << 8) & 0xFF00) | (((uint16_t)packet[4]) & 0x00FF);

		for (int j = 0; j < inner_tabs; j++)
		{
			printf("\t");
		}

		printf("Module type: \t0x%02X ", (unsigned)(unsigned char)module_type);
		switch (module_type)
		{
		case(TRACKING_MOD) :
		{
			printf("(Tracking)");
			break;
		}
		case(SYSTEM_MOD) :
		{
			printf("(System)");
			break;
		}
		case(MOTOR_MOD) :
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

		printf("Command type: \t0x%02X ", (unsigned)(unsigned char)command_type);
		switch (command_type)
		{
		case(TRACKING_STATE_CMD) :
		{
			printf("(State)");
			break;
		}
		case(TRACKING_DATA_CMD) :
		{
			printf("(Data)");
			break;
		}
		case(MOTOR_DATA_CMD) :
		{
			printf("(Data)");
			break;
		}
		case(SYSTEM_KILL_CMD) :
		{
			printf("(Kill)");
			break;
		}
		case(ERROR) :
		{
			printf("(Error)");
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

		print_raw_bytes(packet, size, OVERHEAD_BYTES_CNT, inner_tabs + 1);

		for (int j = 0; j < tabs; j++)
		{
			printf("\t");
		}

		printf("Packet end =============================================\n\n");
	}

	void CommandBuilder::print_raw_bytes(BYTE*& packet, size_t size, int offset, int tabs)
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

			if (((i - offset + 1) % 8 == 0) && i > 0)
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

}/* namespace havroc */
