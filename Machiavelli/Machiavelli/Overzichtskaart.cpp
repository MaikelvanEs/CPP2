#include "Overzichtskaart.h"
#include <iostream>

Overzichtskaart::Overzichtskaart()
{
	front_ = "Voorkant\r\n";
	front_ += "Verloop van een speelbeurt\r\n";
	front_ += "Inkomsten: Neem 2 goudstukken of neem 2 bouwkaarten en leg er 1 af.\r\n";
	front_ += "Bouwen: Leg 1 bouwkaart neer en betaal de waarde.\r\n";
	front_ += "Karaktereigenschap: Op elk moment te gebruiken.\r\n";

	back_ = "Achterkant\r\n";
	back_ += "1 Moordenaar:\r\n\t- Vermoordt een ander karakter\r\n";
	back_ += "2 Dief:\r\n\t- Steelt van een andere speler\r\n";
	back_ += "3 Magier:\r\n\t- Ruilt bouwkaarten om\r\n";
	back_ += "4 Koning:\r\n\t- Begint de volgende beurt\r\n\t- Ontvangt van monumenten (geel)\r\n";
	back_ += "5 Prediker:\r\n\t- Is beschermd tegen de Condottiere\r\n\t- Ontvangt van kerkelijke gebouwen (blauw)\r\n";
	back_ += "6 Koopman:\r\n\t- Ontvangt een extra goudstuk\r\n\t- Ontvangt van commerciele gebouwen (groen)\r\n";
	back_ += "7 Bouwmeester:\r\n\t- Trekt 2 extra bauwkaarten\r\n\t- Mag 3 gebouwen bouwen\r\n";
	back_ += "8 Condotierre:\r\n\t- Vernietigt een gebouw\r\n\t- Ontvangt van alle militaire gebouwen (rood)\r\n";
}

std::string Overzichtskaart::getInfo() const
{
	return front_ + "\r\n" + back_;
}

