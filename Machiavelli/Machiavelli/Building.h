#pragma once
#include <string>

class Building
{
public:
	Building(const std::string name, const int value, const std::string colour, const std::string special);
	~Building();

	std::string getInfo() const;
	std::string getColour() const;
	std::string getName() const;
	int getValue() const;

private:
	std::string name_;
	int value_;
	std::string colour_;
	std::string special_;
};
