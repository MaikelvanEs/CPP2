#include "Building.h"

Building::Building(const std::string name, const int value, const std::string colour, const std::string special)
	: name_(name)
	, value_(value)
	, colour_(colour)
	, special_(special)
{
}

Building::~Building()
{
}

std::string Building::getInfo() const
{
	std::string result = name_;
	result += " (" + colour_;
	result += ", " + value_;
	if (special_.length() != 0)
		result += ", " + special_;
	result += ")\r\n";
	return result;
}
