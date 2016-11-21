#ifndef _UTIL_H_
#define _UTIL_H_

#include <set>
#include <string>
#include <iostream>

#define __class__ typeid(*this).name()

void logMsg(const char * class_name, const char * method, const std::string & message, std::ostream & output = std::cout);

void errorMsg(const char * class_name, const char * method, const char* message, int error_code, std::ostream & output=std::cout);

void warningMsg(const char * class_name, const char * method, const char* message, int warning_code, std::ostream & output=std::cerr);

std::string convertSetToString(std::set<int> & _set);

long hashFunc(std::string _val);

long hashFunc(std::set<int> _vertexSet);

void credits();

void helpUsage();

double getToken();

bool cmdOptionExists(char** begin, char** end, const std::string& option);

char* getCmdOption(char ** begin, char ** end, const std::string & option);

#endif