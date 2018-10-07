#ifndef KENIRA_TYPES_HEADER
#define KENIRA_TYPES_HEADER

#include <vector>
#include <chrono>

namespace kenira
{
namespace types
{

typedef std::vector<std::vector<std::string>> vvstr;
typedef std::vector<std::string> vstr;
typedef std::vector<std::vector<int>> vvint;
typedef std::vector<int> vint;

typedef std::chrono::high_resolution_clock::time_point time_point;




}
}




#endif