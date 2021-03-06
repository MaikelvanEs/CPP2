#include "Character.h"
#include "Socket.h"

Character::Character(const int number, const std::string name)
	: number_(number)
	, name_(name)
	, ability_(true)
	, alive_(true)
{
	switch (number_)
	{
		case koning:
			colour_ = "geel";
			break;
		case prediker:
			colour_ = "blauw";
			break;
		case koopman:
			colour_ = "groen";
			break;
		case condotierre:
			colour_ = "rood";
			break;
		default:
			colour_ = "grijs";
			break;
	}
}

std::string Character::getInfo() const
{
	return "[" + std::to_string(number_) + "] " + name_ + "\r\n";
}

int Character::getNumber() const
{
	return number_;
}

std::string Character::getName() const
{
	return name_;
}

void Character::setAbility(const bool ability)
{
	ability_ = ability;
}

bool Character::hasAbility() const
{
	return ability_;
}

void Character::setAlive(const bool alive)
{
	alive_ = alive;
}

bool Character::isAlive() const
{
	return alive_;
}
