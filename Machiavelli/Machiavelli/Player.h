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
	Player();
	Player(const std::string& name);

	std::string get_name() const;
	void set_name(const std::string& new_name);
	void setGame(std::shared_ptr<Game> game);
	void endGame();
	void setPlaying(const bool playing);
	bool isPlaying() const;
	void setTurn(const bool turn);
	bool isTurn() const;

private:
    std::string name_;
	std::shared_ptr<Game> game_;
	bool playing_;
	bool turn_;
	int gold_;
	std::vector<Building> buildBuildings_;
	std::vector<Building> handBuildings_;
	std::vector<Character> characters_;
};

#endif /* Player_hpp */
