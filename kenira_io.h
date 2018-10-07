#ifndef IO_HEADER
#define IO_HEADER

#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <bitset>
#include <iterator>
#include <limits>
#include <thread>
#include <kenira_types.h>

//TODO: - f_convert_vstring_to_vvint not using delimiters

namespace kenira
{
namespace io
{

void wait_ms(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	return;
}

bool is_key_pressed_windows(int key)
{
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}

void print(const kenira::types::vstr& str)
{
	for (auto&& s : str)
	{
		std::cout << s << std::endl;
	}
	return;
}

int get_max_string_length_in_vstr(const kenira::types::vstr& vstr)
{
	int length = 0;
	for (auto&& str : vstr)
	{
		length = std::max(length, (int)str.size());
	}
	return length;
}

void flush_input_buffer()
{
	const size_t size = 4000;
	char temp[size];
	/*std::cin.getline(temp, size);
	if (std::cin.fail()) {*/
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//}
}

void print(kenira::types::vvstr& vvstr)
{
	for (auto&& vstr : vvstr)
	{
		print(vstr);
	}
	return;
}

bool is_digit_or_minus(char c)
{
	return c == '-' || isdigit(c);
}

std::string read_file(const std::experimental::filesystem::path filepath)
{
	std::experimental::filesystem::path path(filepath);
	std::ifstream ifs(path);
	//std::experimental::filesystem::ifstream ifs{ path };
	std::stringstream sstr;
	sstr << ifs.rdbuf();

	return sstr.str();
}

std::string read_file(const std::experimental::filesystem::path path, bool linebreak = false)
{
	if (std::experimental::filesystem::exists(path))
	{
		std::ifstream infile(path);
		if (infile.is_open())
		{
			std::string input = "";
			std::string line = "";
			while (std::getline(infile, line))
			{
				if (linebreak == true)
				{
					line.append("\n");
				}
				input += line;
			}
			return input;
		}
	}
	else
	{
		std::cout << "ERROR in read_file: Path does not exist!" << std::endl;
		system("pause");
		return 0;//return '\0';
	}
}

std::string read_file_line(const std::experimental::filesystem::path path, int lnr = 0)
{
	if (std::experimental::filesystem::exists(path))
	{
		std::ifstream infile(path);
		if (infile.is_open())
		{
			std::string line = "";
			int i = 1;
			while (std::getline(infile, line))
			{
				if (i == lnr)
				{
					return line;
				}
			}
			std::cout << "ERROR in F_Read_Line_From_File_To_String: File does not contain " << lnr << " lines." << std::endl;
			return 0;
		}
	}
	else
	{
		std::cout << "ERROR in F_Read_Line_From_File_To_String: Path does not exist!" << std::endl;
		system("pause");
		return 0;//return '\0';
	}
}

std::vector<std::string> read_file_array(const std::experimental::filesystem::path path)
{
	if (!std::experimental::filesystem::exists(path))
	{
		std::cout << "ERROR in read_file_array: Path does not exist!" << std::endl;
		return {};
	}

	std::ifstream infile(path);
	if (infile.is_open())
	{
		std::vector<std::string> vstr;

		std::string line = "";
		while (std::getline(infile, line))
		{
			vstr.push_back(line);
		}
		return vstr;
	}
	return {};
}

int write_array_file(const std::experimental::filesystem::path path, const std::vector<std::string>& data, std::string mode = "ow")
{
	if (!std::experimental::filesystem::exists(path))
	{
		std::cout << "WARNING in write_array_file: Path does not exist! Program continues." << std::endl;
		//return 0;
	}

	std::fstream wfile;

	if (mode == "app")
	{
		wfile.open(path, std::fstream::out | std::fstream::app);
	}
	else
	{
		wfile.open(path, std::fstream::out);
	}

	if (wfile.is_open())
	{
		std::string temp;
		for (auto&& string : data)
		{
			temp = string + '\n';
			wfile.write(string.c_str(), string.size());
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

int write_string_file(std::experimental::filesystem::path path, std::string str, bool print = false)
{
	if (1 == write_array_file(path, std::vector<std::string>({ str }), ""))
	{
		if (print)
		{
			std::cout << "Wrote string to " << path << std::endl;
		}
		return 1;
	}
	else
	{
		std::cout << "Error writing string to file " << path << "!" << std::endl;
		return 0;
	}
}

int delete_file(std::experimental::filesystem::path path)
{
	return std::experimental::filesystem::remove(path);
}

bool is_anagram(std::string s1, std::string s2)
{
	std::sort(s1.begin(), s1.end());
	std::sort(s2.begin(), s2.end());
	return s1 == s2;
}

//std::vector<std::string> split_string(std::string str, const char delimiter = ' ')
//{
//	std::vector<std::string> result;
//	std::string temp = "";
//	for (size_t i = 0; i < str.size(); i++)
//	{
//		if (str[i] == delimiter)
//		{
//			result.push_back(temp);
//			temp = "";
//		}
//		else if (i == str.size() - 1)
//		{
//			temp += str[i];
//			result.push_back(temp);
//		}
//		else
//		{
//			temp += str[i];
//			//temp.append(std::to_string(str[i]));
//		}
//	}
//
//	return result;
//}

//void split_string(std::string str, std::vector<std::string>& result, const char delimiter = ' ')
//{
//	std::string temp = "";
//	for (size_t i = 0; i < str.size(); i++)
//	{
//		if (str[i] == delimiter)
//		{
//			result.push_back(temp);
//			temp = "";
//		}
//		else if (i == str.size() - 1)
//		{
//			temp += str[i];
//			result.push_back(temp);
//		}
//		else
//		{
//			temp += str[i];
//			//temp.append(std::to_string(str[i]));
//		}
//	}
//	return;
//}

void split_string(const std::string &s, char delimiter, std::vector<std::string>& result)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delimiter))
	{
		result.push_back(item);
	}
	return;
}

std::vector<std::string> split_string(const std::string &s, char delim)
{
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		result.push_back(item);
	}
	return result;
}

void split_string_vint(const std::string &s, char delim, std::vector<int>& result)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		result.push_back(std::stoi(item));
	}
	return;
}

std::vector<int> split_string_vint(const std::string &s, char delim)
{
	std::vector<int> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		result.push_back(std::stoi(item));
	}
	return result;
}

bool find_string_nocase(const std::string& str, const std::string& str_to_find)
{
	auto it = std::search(
		str.begin(), str.end(),
		str_to_find.begin(), str_to_find.end(),
		[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	);
	return (it != str.end());
}

std::vector<std::vector<std::string>> split_vstring(std::vector<std::string>& str, const char delimiter = ' ')
{
	std::vector<std::vector<std::string>> split;
	for (int i = 0; i < str.size(); i++)
	{
		std::stringstream ss;
		ss << str[i];
		std::string line = "";
		std::vector<std::string> temp_vstr;
		while (std::getline(ss, line, delimiter))
		{
			temp_vstr.push_back(line);
		}
		split.push_back(temp_vstr);
	}
	return split;
}

std::vector<std::vector<int>> convert_vstring_vvint(std::vector<std::string> str)
{
	std::vector<std::vector<int>> result;
	for (int i = 0; i < str.size(); i++)
	{
		std::stringstream ss(str[i]);
		std::vector<int> vint_temp;
		std::string t = "";
		while (ss >> t)
		{
			std::istringstream iss(t);
			int i = 0;
			if (iss >> i)
			{
				vint_temp.push_back(i);
			}
		}
		result.push_back(vint_temp);
	}
	return result;
}

std::vector<int> convert_vstring_vint(std::vector<std::string> str)
{
	std::vector<int> v;
	for (int i = 0; i < str.size(); i++)
	{
		std::stringstream ss;
		ss << str[i];
		std::vector<int> pb;
		std::string t = "";
		while (ss >> t)
		{
			std::istringstream iss(t);
			int i = 0;
			if (iss >> i)
			{
				v.push_back(i);
			}
		}
	}
	return v;
}

std::vector<int> convert_string_vint(std::string str)
{
	for (size_t i = 0; i < (str.size() - 1); i++)													// insert blank spaces if not present when either a word or number starts or end
	{																							// so that they can get caught individually by stringstream later
		if ((kenira::io::is_digit_or_minus(str[i]) != kenira::io::is_digit_or_minus(str[i + 1])))
		{
			if (str[i] != ' ' && str[i + 1] != ' ')
			{
				str.insert(i + 1, " ");
			}
		}
	}
	std::vector<int> v;
	std::stringstream ss(str);
	std::string s;
	while (ss >> s)
	{
		if (isdigit(s[0]) || (s.size() > 1 ? kenira::io::is_digit_or_minus(s[0]) && isdigit(s[1]) : false))
		{
			v.push_back(std::stoi(s));
		}
	}
	/*ss << str;
	int t = 0;
	while (ss >> t)
	{
		v.push_back(t);
	}*/
	return v;
}

std::vector<unsigned long long> convert_vstring_vull(std::vector<std::string> str)
{
	std::vector<unsigned long long> v;
	for (int i = 0; i < str.size(); i++)
	{
		v.push_back(std::stoull(str[i]));
	}
	return v;
}

/*
int parse_vstr_words_ints(std::vector<std::string> str, std::vector<std::vector<std::string>>& strvec, std::vector<std::vector<int>>& intvec)
{
	bool debug = false;

	for (auto&& l : str)
	{
		std::vector<std::string> spb;
		std::vector<int> ipb;

		for (size_t i = 0; i < (l.size() - 1); i++)													// insert blank spaces if not present when either a word or number starts or end
		{																							// so that they can get caught individually by stringstream later
			if ((isalpha(l[i]) != isalpha(l[i + 1])) || (isdigit(l[i]) != isdigit(l[i + 1])))
			{
				if (l[i] != ' ' && l[i + 1] != ' ')
				{
					l.insert(i + 1, " ");
				}
			}
		}

		if (debug == true)
		{
			std::cout << l << std::endl;
			system("pause");
		}

		std::stringstream ss(l);
		std::string s;
		while (ss >> s)
		{
			if (isalpha(s[0]))
			{
				if (debug == true)
				{
					std::cout << "spb: " << s << std::endl;
				}
				spb.push_back(s);
			}
			else if (isdigit(s[0]))
			{
				if (debug == true)
				{
					std::cout << "ipb: " << std::stoi(s) << std::endl;
				}
				ipb.push_back(std::stoi(s));
			}
		}

		strvec.push_back(spb);
		intvec.push_back(ipb);

		if (debug == true)
		{
			system("pause");
		}
	}
	return 1;
}
*/

int parse_str_words_ints(std::string str, std::vector<std::string>& strvec, std::vector<int>& intvec)
{
	for (size_t i = 0; i < (str.size() - 1); i++)													// insert blank spaces if not present when either a word or number starts or end
	{																								// so that they can get caught individually by stringstream later
		if ((isalpha(str[i]) != isalpha(str[i + 1])) || (kenira::io::is_digit_or_minus(str[i]) != kenira::io::is_digit_or_minus(str[i + 1])))
		{
			if (str[i] != ' ' && str[i + 1] != ' ')
			{
				str.insert(i + 1, " ");
			}
		}
	}

	std::stringstream ss(str);
	std::string s;
	while (ss >> s)
	{
		if (isalpha(s[0]))
		{
			strvec.push_back(s);
		}
		else if (kenira::io::is_digit_or_minus(s[0]))							// if it starts with a minus sign, make sure a number follows.
		{
			if (s[0] == '-')
			{
				if (s.size() > 1)
				{
					if (isdigit(s[1]))
					{
						intvec.push_back(std::stoi(s));
					}
				}
			}
			else
			{
				intvec.push_back(std::stoi(s));
			}
		}
	}

	return 1;
}

int parse_vstr_words_ints(std::vector<std::string>& input, std::vector<std::vector<std::string>>& vvstr_result, std::vector<std::vector<int>>& vvint_result)
{
	bool debug = false;

	for (auto&& l : input)
	{
		std::vector<std::string> vstr_temp;
		std::vector<int> vint_temp;

		parse_str_words_ints(l, vstr_temp, vint_temp);

		vvstr_result.push_back(vstr_temp);
		vvint_result.push_back(vint_temp);

		if (debug == true)
		{
			system("pause");
		}
	}
	return 1;
}

std::string convert_int_32bitset(int num)
{
	return std::bitset<32>(num).to_string();
}

std::string convert_int_16bitset(int num)
{
	return std::bitset<16>(num).to_string();
}

template<class T>
int print_grid(std::vector<std::vector<T>> grid)
{
	for (auto&& line : grid)
	{
		for (auto&& element : line)
		{
			cout << (element != 0 ? '#' : '.');
		}
		cout << endl;
	}
	return 1;
}



int64_t print_time(std::chrono::high_resolution_clock::time_point t1, std::string str = "", bool micro = false)
{
	int64_t duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t1).count();
	if (str != "")
	{
		std::cout << str << ": " << duration << " us / " << duration / 1000 << " ms." << std::endl;
	}
	return duration / 1000;
}

int64_t time_since_ms(std::chrono::high_resolution_clock::time_point time_point)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time_point).count();
}


}
}
#endif