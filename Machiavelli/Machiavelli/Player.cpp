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

Player::Player()
{
}

Player::Player(const std::string& name)
	: name_(name)
	, playing_(false)
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

void Player::setGame(std::shared_ptr<Game> game)
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
}

bool Player::isPlaying() const
{
	return playing_;
}

void Player::setTurn(const bool turn)
{
	turn_ = turn;
}

bool Player::isTurn() const
{
	return turn_;
}
