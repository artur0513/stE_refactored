#pragma once
#include <SFML/System.hpp>
#include <fstream>
#include <deque>
#include <iostream>

namespace sf {
	using Vector2d = sf::Vector2<double>;
}

// Строка с текущей датой
std::string get_date_string();

// Строка с текущим временем
std::string get_time_string();