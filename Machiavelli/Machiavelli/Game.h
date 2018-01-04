#pragma once
#include "Player.h"
#include "Building.h"
#include "Character.h"
#include "Deck.h"
#include "FileReader.h"
#include <memory>
#include "Overzichtskaart.h"
#include "ClientInfo.h"

class Game
{
public:
	Game();
	~Game();

	void addToGame(const std::shared_ptr<ClientInfo> client);
	void endTurn();
	void takeCharacter(const int name);
	void discardCharacter(const int name);
	void turnStartChoice(const int choice);
	std::string getHelp() const;

	void killCharacter(const int choice) const;
	void stealFromCharacter(const int choice) const;
	void swapHands() const;
	void showAliveCharacters(Socket& socket, const int startName);
	
private:
	int gold_;
	cards::Deck<Building> buildingDeck_;
	std::vector<Character> characterDrawPile_;
	std::vector<Character> characterDiscardPile_;
	std::shared_ptr<ClientInfo> client1_;
	std::shared_ptr<ClientInfo> client2_;
	int currentCharacter_;
	std::shared_ptr<ClientInfo> currentClient_;
	bool start_;

	void createBuildings();
	void createCharacters();
	void startGame();
	void takeCharacters();
	void discardCharacters();
	void nextCharacter();
	bool findNextCharacter();
	void switchActivePlayer(const int stateCurrentplayer, const int stateNextplayer);
	void newRound();
	void startRound();
	void newTurn() const;
	void showCharacter(Socket& socket) const;
	void showOpponent() const;
	void takeGold();
	void takeBuildings();
	void checkThief() const;
};
