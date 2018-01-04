//
//  Player.hpp
//  socketexample
//
//  Created by Bob Polis on 23-11-15.
//  Copyright © 2015 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#ifndef Player_hpp
#define Player_hpp

#include <string>
#include <vector>
#include <memory>
#include "Building.h"
#include "Character.h"

class Socket;
class Game;

class Player {
public:
	Player(const std::string& name);
	~Player();

	std::string get_name() const;
	void set_name(const std::string& new_name);
	void setGame(const std::shared_ptr<Game> game);
	void setPlaying(const bool playing);
	bool isPlaying() const;
	void addCharacter(const Character character);
	bool hasCharacter(const int character);
	void setState(const int state);
	void setKing(const int king);
	bool hasKing();
	bool wasKing() const;
	void showGold(Socket& socket) const;
	void showBuildings(Socket& socket) const;
	void showCards(Socket& socket) const;
	void showChoices(Socket& socket) const;
	void addGold();
	void addGold(const int gold);
	void drawBuilding(const Building card);
	void useCharacter(const int number, Socket& socket);
	Character getCharacter();
	Character getCharacter(const int name);
	std::vector<int> getAliveCharacters(const int startName);
	void killCharacter(const int name);
	void steal();
	bool isStolen() const;
	int giveGold();
	std::vector<Building> getHand();
	void setHand(std::vector<Building> buildings);
	void handleCommand(std::string command) const;
	bool isWaiting() const;

private:
    std::string name_;
	std::weak_ptr<Game> game_;
	bool playing_;
	int gold_;
	int state_;
	bool king_;
	bool stolen_;
	std::vector<Building> buildBuildings_;
	std::vector<Building> handBuildings_;
	std::vector<Building> chooseBuildings_;
	std::vector<Character> characters_;

	void moordenaarAbility(Socket& socket, const int choice = 0) const;
	void diefAbility(Socket& socket, const int choice = 0) const;
	void magierAbility(Socket& socket, const int choice = 0) const;
	void koningAbility(Socket& socket);
	void predikerAbility(Socket& socket);
	void koopmanAbility(Socket& socket);
	void bouwmeesterAbility(Socket& socket);
	void condotierreAbility(Socket& socket);
};

#endif /* Player_hpp */
