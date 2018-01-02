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
	
private:
	int gold_;
	cards::Deck<Building> buildingDeck_;
	std::vector<Character> characterDrawPile_;
	std::vector<Character> characterDiscardPile_;
	std::shared_ptr<ClientInfo> client1_;
	std::shared_ptr<ClientInfo> client2_;
	Overzichtskaart oCard_;

	void createBuildings();
	void createCharacters();
	void startGame();
};
