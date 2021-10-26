#include "System.h"
#include <Drivers/Screen.h>
#include <System/IO.h>
#include <Graphics/Graphics.h>
#include <Drivers/Keyboard.h>
#include <util/string.h>
#include <FS/vfs.h>
#include <System/SystemTime.h>
#include <System/BinaryRuntime.h>
#include <Timer/Timer.h>
#include <System/MemoryManager.h>
const int io_address = 0xA00000;
int get_address();
void pass_address(int in);

void restart()
{
	uint8 good = 0x02;
	while (good & 0x02)
		good = inportb(0x64);
	outportb(0x64, 0xFE);
}
void shutdown()
{
}
int run_file_bin(char *line)
{
	char *args[10];
	split_to_args(args, line, ' ', 10);
	if (does_file_exists(args[0]) && equal(get_file_type_of(args[0]), "bin"))
	{
		set_args(args, 10);
		roll_back(10 * 100); //I doubt about this
		run_binary_file(args[0]);
		return 1;
	}
	return 0;
}

void system(char *line)
{
	if (line[0] == 0 || line[0] == '\b')
	{
		return;
	}

	if (equalS(line, "cls", 3))
	{
		clearScreen();
	}
	else if (equalS(line, "echo", 4))
	{
		printC('\n');
		for (int i = 5; line[i] != 0; i++)
		{
			printC(line[i]);
		}
	}
	else if (equalS("time", line, 4))
	{
		printW("\nCurrent BIOS time is: ");
		printW(toString(getHours()));
		printC(':');
		printW(toString(getMinutes()));
		printC(':');
		printW(toString(getSeconds()));
	}
	else if (equalS("read", line, 4))
	{
		printC('\n');
		if (does_file_exists(substring(line, 5, length(line))))
		{
			char *text = read_file(substring(line, 5, length(line)));
			int len = get_file_size(substring(line, 5, length(line)));
			for (int i = 0; i < len; i++)
			{
				printC(text[i]);
			}
		}
	}
	else if (equalS("del", line, 3))
	{
		if (does_file_exists(substring(line, 4, length(line))))
			delete_file(substring(line, 4, length(line)));
		else
			printW("\nFile doesn't exists");
	}
	else if (equalS("ls", line, 2))
	{
		for (int i = 0; i < f_index; i++)
		{
			printC('\n');
			printW(get_file_name_at(i));
			for (int j = 0; j < 40 - length(get_file_name_at(i)); j++)
			{
				printC(' ');
			}
			printW(toString(get_file_size_at(i)));
			printW(" Bytes");
		}
	}
	else if (run_file_bin(line) == 1)
	{
	}
	else
	{
		print("\nerror: ", 0x04);
		printW("invalid command ");
	}
	for (int i = 0; i < 1000; i++)
	{
		line[i] = 0;
	}
}
void write_io(char *in)
{
	char *memr = (char *)io_address;
	for (int i = 0; in[i] != 0; i++)
	{
		memr[i] = in[i];
		memr[i + 1] = 0;
	}
}
void system_handle(char type, char arg1, char arg2, char arg3)
{
	char *mem = (char *)0x0;
	//IO
	//Set Color
	if (type == 0)
		Screen->defaultColor = arg1;
	//Print Char
	else if (type == 1)
		printC(arg1);
	//Set Cursor Position
	else if (type == 2)
	{
		Screen->cursorX = arg1;
		Screen->cursorY = arg2;
	}
	//Read Char
	else if (type == 3)
		mem[83] = readChar();
	//GET cursorX
	else if (type == 4)
		mem[83] = Screen->cursorX;
	//GET cursorY
	else if (type == 5)
		mem[83] = Screen->cursorY;
	//System
	else if (type == 6)
	{
		system((char *)io_address);
	}
	else if (type == 7)
	{
		clearScreen();
	}
	//Read File
	else if (type == 10)
	{
		char *fc = (char *)read_file((char *)io_address);
		int fl = (int)get_file_size((char *)io_address);
		char *memr = (char *)io_address;
		for (int i = 0; i < fl; i++)
		{
			memr[i] = fc[i];
		}
	}
	//Does file exists
	else if (type == 11)
	{
		char *memr = (char *)io_address;
		memr[0] = does_file_exists(memr);
	}
	//File size
	else if (type == 12)
	{
		int fis = get_file_size((char *)io_address);
		char *memr = (char *)io_address;
		memr[0] = 100;
		memr[1] = fis / 100;
		memr[2] = fis % 100;
	}
	else if (type == 13)
	{
		char *name = (char *)io_address;
		char *text = name + 32;
		int len = ((int *)0x0)[0];
		write_file(name, text, len);
	}
	else if (type == 14)
	{
		create_file((char *)io_address, "", 0);
	}
	//Get time from_boot
	else if (type == 20)
	{
		int *mem = (int *)0x0;
		mem[0] = (int)Sleep;
	}
	else if (type == 45)
	{
		int *mem = (int *)0x0;
		mem[0] = (int)malloc(mem[0]);
	}
}
