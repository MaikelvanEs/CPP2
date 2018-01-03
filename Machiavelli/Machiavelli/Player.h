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

class Game;

class Player {
public:
	Player(const std::string& name);

	std::string get_name() const;
	void set_name(const std::string& new_name);
	void setGame(const std::shared_ptr<Game> game);
	void endGame();
	void setPlaying(const bool playing);
	bool isPlaying() const;
	void addCharacter(const Character character);
	bool hasCharacter(const int character);
	void setState(const int state);
	void setKing(const int king);
	bool hasKing();
	bool wasKing() const;
	void handleCommand(std::string command) const;
	bool isWaiting() const;

private:
    std::string name_;
	std::shared_ptr<Game> game_;
	bool playing_;
	int gold_;
	int state_;
	bool king_;
	std::vector<Building> buildBuildings_;
	std::vector<Building> handBuildings_;
	std::vector<Character> characters_;
};

#endif /* Player_hpp */
