#include "Character.h"

Character::Character(const int number, const std::string name)
	: number_(number)
	, name_(name)
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
		case condottiere:
			colour_ = "rood";
			break;
		default:
			colour_ = "grijs";
			break;
	}
}

std::string Character::getInfo() const
{
	return name_;
}
