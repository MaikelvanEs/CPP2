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

void Player::endGame()
{
	game_ = nullptr;
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

void Player::handleCommand(const std::string command) const
{
	const int cmd = stoi(command);
	switch (state_)
	{
	case eTakeCharacter:
		game_->takeCharacter(cmd);
		break;
	case eDiscardCharacter:
		game_->discardCharacter(cmd);
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
