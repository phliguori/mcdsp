#include "Util.h"

#ifdef WIN
#include <Windows.h>
#endif
#include <locale>
#include <sstream>
#include <ctime>

void logMsg(const char * _class, const char * _method, const std::string & _message, std::ostream & _output)
{
	std::string times;
	time_t timev;
	struct tm now;

	time(&timev);
	localtime_s(&now, &timev);

	times += std::to_string(now.tm_year + 1900);
	times += std::to_string(now.tm_mon + 1);
	times += std::to_string(now.tm_mday);
	times += ":";
	times += std::to_string(now.tm_hour);
	times += std::to_string(now.tm_min);
	times += std::to_string(now.tm_sec);

#ifdef WIN
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
#endif
	_output << "[LOG " + times +"] - ";

#ifdef WIN
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
#endif
	if (_class != NULL) _output << _class; _output << "::";
	if (_method != NULL) _output << _method << ": ";

#ifdef WIN
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
	_output << _message << "\n";

	_flushall();
}

void errorMsg(const char * _class, const char * _method, const char* _message, int _code, std::ostream & _output)
{
#ifdef WIN
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
#endif
	_output << "[ERROR] " << _code << " - ";

#ifdef WIN
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
#endif
	if (_class != NULL) _output << _class; _output << "::";
	if (_method != NULL) _output << _method << ": ";

#ifdef WIN
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
	_output << _message << "\n";

	exit(_code);
}

void warningMsg(const char * _class, const char * _method, const char* _message, int _code, std::ostream & _output)
{
#ifdef WIN
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
#endif
	_output << "[WARNING] " << _code << " - ";

#ifdef WIN
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
#endif
	if (_class != NULL) _output << _class; _output << "::";
	if (_method != NULL) _output << _method << ": ";

#ifdef WIN
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif
	_output << _message << "\n";
}

std::string convertSetToString(std::set<int> & _set)
{
	std::stringstream aux;

	for (std::set<int>::iterator it = _set.begin(); it != _set.end(); ++it)
	{
		aux << *it << " ";
	}

	return aux.str();
}

long hashFunc(std::string _val)
{
	std::locale loc;
	const std::collate<char>& coll = std::use_facet<std::collate<char> >(loc);

	long myHash = coll.hash(_val.data(), _val.data() + _val.length());
	return myHash;
}

long hashFunc(std::set<int> _set)
{
	std::string aux = convertSetToString(_set);
	std::locale loc;
	const std::collate<char>& coll = std::use_facet<std::collate<char> >(loc);

	long myHash = coll.hash(aux.data(), aux.data() + aux.length());

	return myHash;
}

void credits()
{

	return;
}

void helpUsage()
{
	std::cout << std::endl;
	std::cout << "TODO";
	std::cout << std::endl;

	return;
}

double getToken()
{
	time_t timer;
	struct tm y2k = { 0 };
	double seconds;

	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 87; y2k.tm_mon = 4; y2k.tm_mday = 4; // This represents: 4 May 1987

	time(&timer);  /* get current time; same as: timer = time(NULL)  */

	seconds = difftime(timer, mktime(&y2k));

	return seconds;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}

	return 0;
}
