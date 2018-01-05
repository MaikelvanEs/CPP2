#pragma once
#include "Enums.h"
#include <string>

class Socket;

class Character
{
public:
	Character(const int number, const std::string name);

	std::string getInfo() const;
	int getNumber() const;
	std::string getName() const;
	void setAbility(const bool ability);
	bool hasAbility() const;
	void setAlive(const bool alive);
	bool isAlive() const;

private:
	int number_;
	std::string name_;
	std::string colour_;
	bool ability_;
	bool alive_;
};
