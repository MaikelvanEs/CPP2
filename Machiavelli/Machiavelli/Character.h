#pragma once
#include "Enums.h"
#include <string>

class Character
{
public:
	Character(const int number, const std::string name);

	std::string getInfo() const;
	int getNumber() const;
	void setAbility(const bool ability);

private:
	int number_;
	std::string name_;
	std::string colour_;
	bool ability_;
};
