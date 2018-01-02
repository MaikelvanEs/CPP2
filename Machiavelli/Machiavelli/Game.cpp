#include "Game.h"

Game::Game()
{
	gold_ = 30;
	createBuildings();
	createCharacters();
}

Game::~Game()
{
}

void Game::addToGame(const std::shared_ptr<ClientInfo> client)
{
	if (client1_ == nullptr)
	{
		client1_ = client;
	}
	else if (client2_ == nullptr)
	{
		client2_ = client;
		auto &player1 = client1_->get_player();
		player1.setPlaying(true);
		player1.setTurn(true);
		auto &player2 = client2_->get_player();
		player2.setPlaying(true);
		startGame();
	}
}

void Game::createBuildings()
{
	FileReader reader;
	auto buildings = reader.readFile("Card Files/Bouwkaarten.csv");
	if (!buildings.empty())
	{
		std::vector<Building> cards;
		for (auto building : buildings)
		{
			auto previous = building.find(";");
			const auto name = building.substr(0, previous);
			previous++;
			const auto value = stoi(building.substr(previous, building.find(";", previous) - (previous)));
			previous = building.find(";", previous) + 1;
			const auto colour = building.substr(previous, building.find(";", previous) - (previous));
			previous = building.find(";", previous) + 1;
			const auto special = building.substr(previous);
			cards.push_back(Building(name, value, colour, special));
		}
		buildingDeck_ = cards;
	}
}

void Game::createCharacters()
{
	FileReader reader;
	auto characters = reader.readFile("Card Files/karakterkaarten.csv");
	if (!characters.empty())
	{
		for (auto character : characters)
		{
			const auto number = stoi(character.substr(0, character.find(";")));
			const auto name = character.substr(character.find(";") + 1);
			characterDrawPile_.push_back(Character(number, name));
		}
	}
}

void Game::startGame()
{
	buildingDeck_.shuffle_stack();
	random_shuffle(characterDrawPile_.begin(), characterDrawPile_.end());
	auto card = characterDrawPile_.back();
	characterDrawPile_.pop_back();
	auto &socket1 = client1_->get_socket();
	socket1.write("Deze kaart wordt weggelegd:\r\n");
	socket1.write(card.getInfo() + "\r\n\r\n");
	socket1.write("Kies een van deze kaarten om te houden:\r\n");
	for (auto card : characterDrawPile_)
	{
		socket1.write(card.getInfo() + "\r\n");
	}
	auto &socket2 = client2_->get_socket();
	socket2.write("Wacht op andere speler!\r\n");
}
