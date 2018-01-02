#include "Overzichtskaart.h"
#include <iostream>

Overzichtskaart::Overzichtskaart()
{
	front_ = "Voorkant\n";
	front_ += "Verloop van een speelbeurt\n";
	front_ += "Inkomsten: Neem 2 goudstukken of neem 2 kaarten en leg er 1 af.\n";
	front_ += "Bouwen: Leg 1 bouwkaart neer en betaal de waarde.\n";
	front_ += "Karaktereigenschap: Op elk moment te gebruiken.\n";

	back_ = "Achterkant\n";
	back_ += "1 Moordenaar:\n\t- Vermoordt een ander karakter\n";
	back_ += "2 Dief:\n\t- Steelt van een andere speler\n";
	back_ += "3 Magier:\n\t- Ruilt bouwkaarten om\n";
	back_ += "4 Koning:\n\t- Begint de volgende beurt\n\t- Ontvangt van monumenten\n";
	back_ += "5 Prediker:\n\t- Is beschermd tegen de Condottiere\n\t- Ontvangt van kerkelijke gebouwen\n";
	back_ += "6 Koopman:\n\t- Ontvangt een extra goudstuk\n\t- Ontvangt van commerciele gebouwen\n";
	back_ += "7 Bouwmeester:\n\t- Trekt 2 extra kaarten\n\t- Mag 3 gebouwen bouwen\n";
	back_ += "8 Condotierre:\n\t- Vernietigt een gebouw\n\t- Ontvangt van alle militaire gebouwen\n";
}

std::string Overzichtskaart::getInfo() const
{
	return front_ + "\n" + back_;
}

