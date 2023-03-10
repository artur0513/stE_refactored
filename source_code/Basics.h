#pragma once
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <deque>
#include <iostream>
#include <string>
#include <sstream>

namespace sf {
	using Vector2d = sf::Vector2<double>;
}

template<class T>
std::ostream& operator<< (std::ostream& out, sf::Vector2<T> vec) {
	out << "(" << vec.x << ", " << vec.y << ")";
	return out;
}

template<class T>
std::ostream& operator<< (std::ostream& out, sf::Rect<T> rect) {
	out << "[" << rect.left << ", " << rect.top << ", " << rect.width << ", " << rect.height << "]";
	return out;
}

// Строка с текущей датой
std::string get_date_string();

// Строка с текущим временем
std::string get_time_string();

template <class T>
std::string get_message_prefix(T* t) {
	const void* address = static_cast<const void*>(t);
	static std::stringstream ss;
	ss << address;
	std::string name = ss.str();
	ss.flush();
	return std::string(typeid(*t).name()) + " at " + name + ": ";
}