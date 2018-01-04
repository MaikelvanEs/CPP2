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
	, gold_(0)
	, state_(eWait)
	, king_(false)
	, stolen_(false)
{
}

Player::~Player()
{
}

std::string Player::get_name() const
{
	return name_;
}

void Player::set_name(const std::string& new_name)
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
		if (name.getNumber() == character)
			return true;
	}
	return false;
}

void Player::setState(const int state)
{
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
	for (auto building : buildBuildings_)
	{
		socket.write("\t" + building.getInfo());
	}
}

void Player::showCards(Socket& socket) const
{
	socket.write("Handkaarten:\r\n");
	for (auto building : handBuildings_)
	{
		socket.write("\t" + building.getInfo());
	}
}

void Player::showChoices(Socket& socket) const
{
	socket.write("Maak een keuze:\r\n");
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
		break;
	default:
		break;
	}
}

void Player::addGold()
{
	gold_++;
}

void Player::addGold(const int gold)
{
	gold_ += gold;
}

void Player::drawBuilding(const Building card)
{
	chooseBuildings_.push_back(card);
}

void Player::useCharacter(const int number, Socket& socket)
{
	for (auto character : characters_)
		if (character.getNumber() == number)
		{
			if (!character.hasAbility())
			{
				socket.write("Eigenschap is al gebruikt.\r\n");
				return;
			}
			character.setAbility(false);
		}
	switch (number)
	{
	case moordenaar:
		moordenaarAbility(socket);
		break;
	case dief:
		diefAbility(socket);
		break;
	case magier:
		magierAbility(socket);
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
		bouwmeesterAbility(socket);
		break;
	case condotierre:
		condotierreAbility(socket);
		break;
	default:
		break;
	}
	state_ = eUsingCharacterPower;
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

void Player::killCharacter(const int name)
{
	for (auto character : characters_)
		if (character.getNumber() == name)
			character.setAlive(false);
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

void Player::handleCommand(const std::string command) const
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
		//game_._Get()->;
		break;
	case eBuild:
		//game_._Get()->;
		break;
	case eTurnEnd:
		//game_._Get()->;
		break;
	case eUsingCharacterPower:
		//game_._Get()->;
		break;
	default:
		break;
	}
}

bool Player::isWaiting() const
{
	if (state_ == eWait)
		return true;
	return false;
}

void Player::moordenaarAbility(Socket& socket, const int choice = 0) const
{
	if (state_ == eUsingCharacterPower)
	{
		game_._Get()->killCharacter(choice);
	}
	else
	{
		socket.write("\r\nKies een karakter uit om te vermoorden:\r\n");
		game_._Get()->showAliveCharacters(socket, moordenaar);
	}
}

void Player::diefAbility(Socket& socket, const int choice = 0) const
{
	if (state_ == eUsingCharacterPower)
	{
		game_._Get()->stealFromCharacter(choice);
	}
	else
	{
		socket.write("\r\nKies een karakter uit om te bestelen:\r\n");
		game_._Get()->showAliveCharacters(socket, dief);
	}
}

void Player::magierAbility(Socket& socket, const int choice = 0) const
{
	if (state_ == eUsingCharacterPower)
	{
		if (choice == 0)
		{
			game_._Get()->swapHands();
		}
		else if (choice == 1)
		{
			
		}
	}
	else
	{
		socket.write("\r\nKies een van de volgende handelingen:\r\n");
		socket.write("[0] Ruil handkaarten met andere speler\r\n");
		socket.write("[1] Leg aantal handkaarten weg en neem aantal bouwkaarten\r\n");
	}
}

void Player::koningAbility(Socket& socket)
{

}

void Player::predikerAbility(Socket& socket)
{

}

void Player::koopmanAbility(Socket& socket)
{

}

void Player::bouwmeesterAbility(Socket& socket)
{

}

void Player::condotierreAbility(Socket& socket)
{

}
