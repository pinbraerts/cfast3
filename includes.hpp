#ifndef CFAST_INCLUDES_HPP
#define CFAST_INCLUDES_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>

using namespace std::string_literals;

class Parser;
struct ErrorProcessor;

template<class T>
auto rule_move(T&& t);

template<class T>
auto rule_cast(T t);

#endif // !CFAST_INCLUDES_HPP
