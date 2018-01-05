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

	std::string getName() const;
	void setName(const std::string& new_name);

	void setGame(const std::shared_ptr<Game> game);
	void setPlaying(const bool playing);
	bool isPlaying() const;

	void addCharacter(const Character character);
	bool hasCharacter(const int character);
	void useCharacter(const int number, Socket& socket, const int choice = 0);
	Character getCharacter();
	Character getCharacter(const int name);
	std::vector<int> getAliveCharacters(const int startName);
	void killCharacter(const int name, Socket& socket);

	void setKing(const int king);
	bool hasKing();
	bool wasKing() const;

	void setState(const int state);
	bool isWaiting() const;
	
	void showGold(Socket& socket) const;
	void addGold();
	void addGold(const int gold);
	void removeGold();
	void steal();
	bool isStolen() const;
	int giveGold();

	void showBuildings(Socket& socket) const;
	void drawBuilding(const Building card);
	void showBuildingChoices(Socket& socket);
	int getBuildingAmount() const;
	bool checkBuilding(const int choice, const int gold);
	Building removeBuilding(const int choice);

	void showCards(Socket& socket) const;
	std::vector<Building> getHand();
	void setHand(std::vector<Building> buildings);
	void convertHand();
	
	void showChoices(Socket& socket) const;
	void handleCommand(std::string command);
	
	int countBuildings();
	int checkColours();
	int checkBuildingAmount() const;

private:
    std::string name_;
	std::weak_ptr<Game> game_;
	bool playing_;
	int gold_;
	int state_;
	int previousState_;
	bool king_;
	bool stolen_;
	std::vector<Building> buildBuildings_;
	std::vector<Building> handBuildings_;
	std::vector<Building> chooseBuildings_;
	std::vector<Character> characters_;

	void moordenaarAbility(Socket& socket, const int choice = 0);
	void diefAbility(Socket& socket, const int choice = 0);
	void magierAbility(Socket& socket, const int choice = 0);
	void koningAbility(Socket& socket);
	void predikerAbility(Socket& socket);
	void koopmanAbility(Socket& socket);
	void bouwmeesterAbility(Socket& socket, const int choice = 0);
	void condotierreAbility(Socket& socket, const int choice = 0);

	void discardBuilding(const int choice);
	void build(const int choice);
	void removeCard(const int choice);
};

#endif /* Player_hpp */
