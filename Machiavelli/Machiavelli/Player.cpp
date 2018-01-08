//
//  Player.cpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#include "Player.h"
#include "Game.h"
using namespace std;

Player::Player(const std::string& name)
	: name_(name)
	, playing_(false)
	, gold_(2)
	, state_(eWait)
	, previousState_(eWait)
	, king_(false)
	, stolen_(false)
{
}

Player::~Player()
{
}

std::string Player::getName() const
{
	return name_;
}

void Player::setName(const std::string& new_name)
{
	name_ = new_name;
}

void Player::setGame(const std::shared_ptr<Game> game)
{
	game_ = game;
}

void Player::setPlaying(const bool playing)
{
	playing_ = playing;
	if (playing_)
		state_ = eWait;
}

bool Player::isPlaying() const
{
	return playing_;
}

void Player::addCharacter(const Character character)
{
	characters_.push_back(character);
}

bool Player::hasCharacter(const int character)
{
	for (Character name : characters_)
	{
		if (name.getNumber() == character && name.isAlive())
			return true;
	}
	return false;
}

void Player::setState(const int state)
{
	previousState_ = state_;
	state_ = state;
}

void Player::setKing(const int king)
{
	king_ = king;
}

bool Player::hasKing()
{
	for (Character name : characters_)
	{
		if (name.getNumber() == koning)
			return true;
	}
	return false;
}

bool Player::wasKing() const
{
	return king_;
}

void Player::showGold(Socket& socket) const
{
	socket.write("Goud: " + to_string(gold_) + "\r\n");
}

void Player::showBuildings(Socket& socket) const
{
	socket.write("Gebouwen:\r\n");
	int number = 0;
	for (auto building : buildBuildings_)
		socket.write("\t[" + to_string(number++) + "] " + building.getInfo());
}

void Player::showCards(Socket& socket) const
{
	int number = 0;
	socket.write("Handkaarten:\r\n");
	for (auto building : handBuildings_)
		socket.write("\t[" + to_string(number++) + "] " + building.getInfo());
}

void Player::showChoices(Socket& socket) const
{
	socket.write("\r\n");
	showGold(socket);
	socket.write("\r\n");
	showBuildings(socket);
	socket.write("\r\n");
	showCards(socket);
	socket.write("\r\n");
	socket.write("Maak een keuze:\r\n");
	socket.write("[help] Bekijk de overzichtskaart\r\n");
	socket.write("[0] Bekijk het goud en de gebouwen van de tegenstander (en maak dan de keuze)\r\n");
	socket.write("[1] Maak gebruik van de karaktereigenschap\r\n");
	switch (state_)
	{
	case eTurnStart:
		socket.write("[2] Neem 2 goudstukken\r\n");
		socket.write("[3] Neem 2 bouwkaarten en leg er 1 af\r\n");
		break;
	case eBuild:
		socket.write("[2] Bouw 1 gebouw\r\n");
	case eTurnEnd:
		socket.write("[9] Eindig beurt\r\n");
		break;
	default:
		break;
	}
}

void Player::addGold()
{
	gold_++;
}

void Player::removeGold()
{
	gold_--;
}

void Player::addGold(const int gold)
{
	gold_ += gold;
}

void Player::drawBuilding(const Building card)
{
	chooseBuildings_.push_back(card);
}

void Player::showBuildingChoices(Socket& socket)
{
	socket.write("\r\nKies 1 van deze bouwkaarten om te behouden:\r\n");
	int number = 0;
	for (auto building : chooseBuildings_)
		socket.write("[" + to_string(number++) + "] " + building.getInfo());
}

void Player::useCharacter(const int number, Socket& socket, const int choice)
{
	if (state_ < eUsingCharacterPower)
	{
		for (auto &character : characters_)
		{
			if (character.getNumber() == number)
			{
				if (!character.hasAbility())
				{
					socket.write("\r\nEigenschap is al gebruikt.\r\n");
					return;
				}
				character.setAbility(false);
			}
		}
	}
	switch (number)
	{
	case moordenaar:
		moordenaarAbility(socket, choice);
		break;
	case dief:
		diefAbility(socket, choice);
		break;
	case magier:
		magierAbility(socket, choice);
		break;
	case koning:
		koningAbility(socket);
		break;
	case prediker:
		predikerAbility(socket);
		break;
	case koopman:
		koopmanAbility(socket);
		break;
	case bouwmeester:
		bouwmeesterAbility(socket, choice);
		break;
	case condotierre:
		condotierreAbility(socket, choice);
		break;
	default:
		break;
	}
}

Character Player::getCharacter()
{
	auto it = characters_.begin();
	auto character = *it;
	characters_.erase(it);
	return character;
}

Character Player::getCharacter(const int name)
{
	for (auto it = characters_.begin(); it != characters_.end(); it++)
	{
		auto character = *it;
		if (character.getNumber() == name)
		{
			characters_.erase(it);
			character.setAlive(true);
			return character;
		}
	}
}

std::vector<int> Player::getAliveCharacters(const int startName)
{
	std::vector<int> alive;
	for (auto character : characters_)
		if (character.isAlive() && character.getNumber() > startName)
			alive.push_back(character.getNumber());
	return alive;
}

void Player::killCharacter(const int name, Socket& socket)
{
	for (auto &character : characters_)
		if (character.getNumber() == name)
		{
			character.setAlive(false);
			socket.write("\r\nJe " + character.getName() + " is vermoord.\r\n");
		}
}

bool Player::checkPrediker()
{
	for (auto character : characters_)
		if (character.getName() == "prediker" && character.isAlive())
			return true;
	return false;
}

void Player::steal()
{
	stolen_ = true;
}

bool Player::isStolen() const
{
	return stolen_;
}

int Player::giveGold()
{
	const int gold = gold_;
	gold_ = 0;
	return gold;
}

std::vector<Building> Player::getHand()
{
	std::vector<Building> buildings = handBuildings_;
	handBuildings_.clear();
	return buildings;
}

void Player::setHand(std::vector<Building> buildings)
{
	for (auto building : buildings)
		handBuildings_.push_back(building);
}

void Player::convertHand()
{
	for (auto building : chooseBuildings_)
		handBuildings_.push_back(building);
	chooseBuildings_.clear();
}

int Player::getBuildingAmount() const
{
	return buildBuildings_.size();
}

bool Player::checkBuilding(const int choice, const int gold)
{
	auto building = buildBuildings_[choice];
	if (building.getValue() > gold + 1)
		return false;
	return true;
}

Building Player::removeBuilding(const int choice)
{
	auto building = buildBuildings_[choice];
	for (auto it = buildBuildings_.begin(); it != buildBuildings_.end(); it++)
	{
		auto checkBuilding = *it;
		if (checkBuilding.getName() == building.getName())
		{
			buildBuildings_.erase(it);
			break;
		}
	}
	return building;
}

void Player::handleCommand(const std::string command)
{
	const int cmd = stoi(command);
	switch (state_)
	{
	case eTakeCharacter:
		game_._Get()->takeCharacter(cmd);
		break;
	case eDiscardCharacter:
		game_._Get()->discardCharacter(cmd);
		break;
	case eTurnStart:
		game_._Get()->turnStartChoice(cmd);
		break;
	case eDiscardBuilding:
		discardBuilding(cmd);
		break;
	case eBuild:
		game_._Get()->buildChoice(cmd);
		break;
	case eBuildBuilding:
		build(cmd);
		break;
	case eTurnEnd:
		game_._Get()->endChoice(cmd);
		break;
	case eUsingCharacterPower:
		game_._Get()->useCharacter(cmd);
		break;
	case eBuildOneBuilding:
		build(cmd);
		break;
	case eBuildTwoBuildings:
		build(cmd);
		break;
	case eMagier:
		removeCard(cmd);
		break;
	default:
		break;
	}
}

int Player::countBuildings()
{
	int score = 0;
	for (auto building : buildBuildings_)
		score += building.getValue();
	return score;
}

int Player::checkColours()
{
	int yellow = 0;
	int blue = 0;
	int green = 0;
	int red = 0;
	int lila = 0;
	for (auto building : buildBuildings_)
	{
		if (building.getColour() == "geel")
			yellow++;
		else if (building.getColour() == "blauw")
			blue++;
		else if (building.getColour() == "groen")
			green++;
		else if (building.getColour() == "rood")
			red++;
		else if (building.getColour() == "lila")
			lila++;
	}
	if (yellow > 0 && blue > 0 && green > 0 && red > 0 && lila > 0)
		return 3;
	return 0;
}

int Player::checkBuildingAmount() const
{
	if (buildBuildings_.size() >= 8)
		return 2;
	return 0;
}

bool Player::isWaiting() const
{
	if (state_ == eWait)
		return true;
	return false;
}

void Player::moordenaarAbility(Socket& socket, const int choice)
{
	if (state_ == eUsingCharacterPower)
	{
		game_._Get()->killCharacter(choice);
		socket.write("\r\nKarakter is vermoord en komt niet aan de beurt.\r\n");
		state_ = previousState_;
		previousState_ = eUsingCharacterPower;
		showChoices(socket);
	}
	else
	{
		socket.write("\r\nKies een karakter uit om te vermoorden:\r\n");
		game_._Get()->showAliveCharacters(socket, moordenaar);
		previousState_ = state_;
		state_ = eUsingCharacterPower;
	}
}

void Player::diefAbility(Socket& socket, const int choice)
{
	if (state_ == eUsingCharacterPower)
	{
		game_._Get()->stealFromCharacter(choice);
		socket.write("\r\nGeld wordt gestolen als karakter aan de beurt is.\r\n");
		state_ = previousState_;
		previousState_ = eUsingCharacterPower;
		showChoices(socket);
	}
	else
	{
		socket.write("\r\nKies een karakter uit om te bestelen:\r\n");
		game_._Get()->showAliveCharacters(socket, dief);
		previousState_ = state_;
		state_ = eUsingCharacterPower;
	}
}

void Player::magierAbility(Socket& socket, const int choice)
{
	if (state_ == eUsingCharacterPower)
	{
		if (choice == 0)
		{
			game_._Get()->swapHands();
			state_ = previousState_;
			previousState_ = eUsingCharacterPower;
			showChoices(socket);
		}
		else if (choice == 1)
		{
			socket.write("\r\nKies een kaart om weg te leggen of stop:\r\n");
			showCards(socket);
			socket.write("\t[99] Stop\r\n");
			state_ = eMagier;
		}
	}
	else
	{
		socket.write("\r\nKies een van de volgende handelingen:\r\n");
		socket.write("[0] Ruil handkaarten met andere speler\r\n");
		socket.write("[1] Leg aantal handkaarten weg en neem aantal bouwkaarten\r\n");
		previousState_ = state_;
		state_ = eUsingCharacterPower;
	}
}

void Player::koningAbility(Socket& socket)
{
	int yellow = 0;
	for (auto building : buildBuildings_)
		if (building.getColour() == "geel")
			yellow++;
	socket.write("\r\nJe ontvangt " + to_string(yellow) + " goud voor je gele gebouwen.\r\n");
	game_._Get()->getGold(yellow);
	showChoices(socket);
}

void Player::predikerAbility(Socket& socket)
{
	int blue = 0;
	for (auto building : buildBuildings_)
		if (building.getColour() == "blauw")
			blue++;
	socket.write("\r\nJe ontvangt " + to_string(blue) + " goud voor je blauwe gebouwen.\r\n");
	game_._Get()->getGold(blue);
	showChoices(socket);
}

void Player::koopmanAbility(Socket& socket)
{
	int green = 0;
	for (auto building : buildBuildings_)
		if (building.getColour() == "groen")
			green++;
	socket.write("\r\nJe ontvangt " + to_string(green) + " goud voor je groene gebouwen.\r\n");
	socket.write("Je krijgt 1 goud extra.\r\n");
	green++;
	game_._Get()->getGold(green);
	showChoices(socket);
}

void Player::bouwmeesterAbility(Socket& socket, const int choice)
{
	if (state_ == eUsingCharacterPower)
	{
		switch (choice)
		{
		case 0:
			state_ = previousState_;
			previousState_ = eUsingCharacterPower;
			showChoices(socket);
			break;
		case 1:
			socket.write("\r\nBouw 1 gebouw.\r\n");
			state_ = eBuildOneBuilding;
			break;
		case 2:
			socket.write("\r\nBouw 2 gebouwen.\r\n");
			state_ = eBuildTwoBuildings;
			break;
		default:
			socket.write("\r\nGeen geldig aantal.\r\n");
			break;
		}
	}
	else
	{
		socket.write("\r\nJe ontvangt 2 bouwkaarten.\r\n");
		game_._Get()->getBuildings(2);
		socket.write("\r\nHoe veel gebouwen wil je bouwen (maximaal 2):\r\n");
		showCards(socket);
		previousState_ = state_;
		state_ = eUsingCharacterPower;
	}
}

void Player::condotierreAbility(Socket& socket, const int choice)
{
	if (state_ == eUsingCharacterPower)
	{
		if (choice == 99)
		{
			state_ = previousState_;
			previousState_ = eUsingCharacterPower;
			game_._Get()->showChoices();
		}
		else if (game_._Get()->checkRemoveBuilding(choice, gold_))
		{
			const int gold = game_._Get()->removeOpponentBuilding(choice);
			game_._Get()->payGold(gold);
			socket.write("\r\nGebouw is weggehaald. Je betaald " + to_string(gold) + " goud.\r\n");
			state_ = previousState_;
			previousState_ = eUsingCharacterPower;
			showChoices(socket);
		}
		else
		{
			socket.write("\r\nNiet genoeg goud om dit gebouw weg te halen. Kies een ander gebouw.\r\n");
			game_._Get()->showOpponentBuildings();
			socket.write("\t[99] Stop\r\n");
		}
	}
	else
	{
		int red = 0;
		for (auto building : buildBuildings_)
			if (building.getColour() == "rood")
				red++;
		socket.write("\r\nJe ontvangt " + to_string(red) + " goud voor je rode gebouwen.");
		game_._Get()->getGold(red);
		if (game_._Get()->getOpponentBuildings() >= 8)
		{
			socket.write("\r\nTegenstander heeft 8 of meer gebouwen. Je kunt geen gebouwen weghalen.\r\n");
			showChoices(socket);
		}
		else if (!game_._Get()->checkOpponentPrediker())
		{
			socket.write("\r\nTegenstander heeft de prediker. Gebouw weghalen niet mogelijk.\r\n");
			showChoices(socket);
		}
		else
		{
			socket.write("\r\nJe mag 1 gebouw weghalen bij de tegenstander.\r\n");
			socket.write("Het weghalen van een gebouw kost 1 goud minder dan de kosten van dat gebouw.\r\n");
			game_._Get()->showOpponentBuildings();
			socket.write("\t[99] Stop\r\n");
			previousState_ = state_;
			state_ = eUsingCharacterPower;
		}
	}
}

void Player::discardBuilding(const int choice)
{
	if (choice >= chooseBuildings_.size())
		game_._Get()->write("\r\nOngeldige keuze. Kies een andere kaart.\r\n");
	else
	{
		auto building = chooseBuildings_[choice];
		handBuildings_.push_back(building);
		for (auto it = chooseBuildings_.begin(); it != chooseBuildings_.end(); it++)
		{
			auto checkBuilding = *it;
			if (checkBuilding.getName() == building.getName())
				game_._Get()->discardBuilding(checkBuilding);
		}
		chooseBuildings_.clear();
		previousState_ = state_;
		state_ = eBuild;
		game_._Get()->showChoices();
	}
}

void Player::build(const int choice)
{
	auto building = handBuildings_[choice];
	if (building.getValue() > gold_)
	{
		game_._Get()->write("\r\nTe weinig goud om dit gebouw te bouwen. Kies een ander gebouw\r\n");
	}
	else
	{
		buildBuildings_.push_back(building);
		if (buildBuildings_.size() >= 8)
			game_._Get()->setFirstToEight();
		for (auto it = handBuildings_.begin(); it != handBuildings_.end(); it++)
		{
			auto checkBuilding = *it;
			if (checkBuilding.getName() == building.getName())
			{
				handBuildings_.erase(it);
				break;
			}
		}
		game_._Get()->payGold(building.getValue());
		if (state_ == eBuildTwoBuildings)
			state_ = eBuildOneBuilding;
		else if (state_ == eBuildOneBuilding)
		{
			state_ = previousState_;
			previousState_ = eUsingCharacterPower;
		}
		else
		{
			previousState_ = state_;
			state_ = eTurnEnd;
			game_._Get()->showChoices();
		}
	}
}

void Player::removeCard(const int choice)
{
	if (choice == 99)
	{
		for (auto building : chooseBuildings_)
			handBuildings_.push_back(building);
		chooseBuildings_.clear();
		state_ = previousState_;
		previousState_ = eUsingCharacterPower;
		game_._Get()->showChoices();
	}
	else
	{
		auto building = handBuildings_[choice];
		for (auto it = handBuildings_.begin(); it != handBuildings_.end(); it++)
		{
			auto checkBuilding = *it;
			if (checkBuilding.getName() == building.getName())
			{
				game_._Get()->discardBuilding(checkBuilding);
				handBuildings_.erase(it);
				break;
			}
		}
		chooseBuildings_.push_back(game_._Get()->drawBuilding());

		game_._Get()->write("\r\nKies een kaart om weg te leggen of stop:\r\n");
		int number = 0;
		game_._Get()->write("Handkaarten:\r\n");
		for (auto building : handBuildings_)
		{
			game_._Get()->write("\t[" + to_string(number++) + "] " + building.getInfo());
		}
		game_._Get()->write("\t[99] Stop\r\n");
	}
}
