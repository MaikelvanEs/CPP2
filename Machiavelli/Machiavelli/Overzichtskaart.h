#pragma once
#include <string>

class Overzichtskaart
{
public:
	Overzichtskaart();

	std::string getInfo() const;

private:
	std::string front_;
	std::string back_;

};
