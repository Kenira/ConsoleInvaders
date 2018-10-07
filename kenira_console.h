#ifndef KENIRA_CONSOLE_HEADER
#define KENIRA_CONSOLE_HEADER

//#define NOMINMAX  // needed in main project file to get rid of error with std::min


#include <iostream>
#include <iomanip>
#include <vector>
//#include <algorithm>
//#include <filesystem>
//#include <fstream>
//#include <sstream>
#include <consoleapi.h>
#include <set>
#include <tchar.h>
#include <chrono>
#include <thread>

#include <kenira_types.h>

#define __WAIT_FOR_ANY_KEY _getch();

namespace kenira
{
namespace console
{

void resize_console_window(int width, int height)
{
	if (width < 80)
	{
		width = 80;
	}
	if (height < 40)
	{
		height = 40;
	}

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);

	_COORD coord;
	coord.X = width;
	coord.Y = height;

	_SMALL_RECT rect = {};
	rect.Top = 0;
	rect.Left = 0;
	rect.Bottom = coord.Y - 1;
	rect.Right = coord.X - 1;

	if (!SetConsoleScreenBufferSize(handle_out, coord))
	{
		std::cout << "Error resizing console screen buffer. Code: " << GetLastError() << std::endl;
	}
	if (!SetConsoleWindowInfo(handle_out, TRUE, &rect))
	{
		std::cout << "Error resizing console window. Code: " << GetLastError() << std::endl;
	}
}

std::array<int, 2> get_console_cursor_position()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(out, &info);
	
	return { info.dwCursorPosition.X , info.dwCursorPosition.Y };
}

void set_console_cursor_position(int x, int y)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { (short)x, (short)y };
	SetConsoleCursorPosition(out, pos);
}

std::array<int, 2> get_console_buffer_size()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(out, &info);

	return { info.dwSize.X , info.dwSize.X };
}

void set_console_cursor_visibility(bool visible)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	GetConsoleCursorInfo(out, &info);
	info.bVisible = visible;
	SetConsoleCursorInfo(out, &info);
}

void write_console_at_position(std::string str, int x, int y)
{
	set_console_cursor_position(x, y);

	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD ret;
	WriteConsole(out, str.c_str(), (DWORD)str.size(), &ret, NULL);
}

void write_console_output_block(std::vector<std::string> input, int x, int y, int attribute = 7)
{
	short max_length = kenira::io::get_max_string_length_in_vstr(input);
	short rows = (short)input.size();
	

	PCHAR_INFO buffer = new CHAR_INFO[rows * max_length];

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < max_length; ++j)
		{
			if (j >= input[i].size())
			{
				buffer[i*max_length + j].Char.AsciiChar = ' ';
				buffer[i*max_length + j].Attributes = attribute;
			}
			else
			{
				buffer[i*max_length + j].Char.AsciiChar = input[i][j];
				buffer[i*max_length + j].Attributes = attribute;
				//std::cout << buffer[i*max_length + j].Char.AsciiChar;
			}
		}
	}


	COORD sz = { max_length, rows };
	COORD my_buf_coord = { 0, 0 };
	_SMALL_RECT region;
	region.Top = y;
	region.Left = x;
	region.Bottom = y + rows;
	region.Right = x + max_length;

	WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), buffer, sz, my_buf_coord, &region);

	delete[] buffer;
	return;
}

std::vector<std::string> read_console_output_block(int x, int y, short rows, short columns)
{
	PCHAR_INFO buffer = new CHAR_INFO[rows * columns];
	COORD sz = { columns, rows };
	COORD my_buf_coord = { 0, 0 };
	_SMALL_RECT region;
	region.Top = y;
	region.Left = x;
	region.Bottom = y + rows;
	region.Right = x + columns;

	ReadConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), buffer, sz, my_buf_coord, &region);

	std::vector<std::string> result;
	for (int i = 0; i < rows; ++i)
	{
		std::string str_temp = "";
		for (int j = 0; j < columns; ++j)
		{
			str_temp += buffer[i*columns + j].Char.AsciiChar;
		}
		result.push_back(str_temp);
	}

	delete [] buffer;
	return result;
}



}
}
#endif