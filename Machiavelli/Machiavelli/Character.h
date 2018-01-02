#pragma once
#include "Enums.h"
#include <string>

class Character
{
public:
	Character(const int number, const std::string name);

	std::string getInfo() const;

private:
	int number_;
	std::string name_;
	std::string colour_;
};
