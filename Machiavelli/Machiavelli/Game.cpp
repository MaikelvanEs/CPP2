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
		//client1_->get_player().setGame(std::make_shared<Game>(*this));
	}
	else if (client2_ == nullptr)
	{
		client2_ = client;
		//client2_->get_player().setGame(std::make_shared<Game>(*this));
		auto &player1 = client1_->get_player();
		player1.setPlaying(true);
		auto &player2 = client2_->get_player();
		player2.setPlaying(true);
		startGame();
	}
}

void Game::endTurn()
{
	nextCharacter();
	if (currentCharacter_ == koning)
	{
		newRound();
	}
	else
	{
		while (!findNextCharacter())
		{
			nextCharacter();
			if (currentCharacter_ == koning)
			{
				newRound();
			}
		}
		// TODO next turn start
	}
}

void Game::takeCharacter(const int name)
{
	for (auto it = characterDrawPile_.begin(); it != characterDrawPile_.end(); it++)
	{
		auto character = *it;
		if (character.getNumber() == name)
		{
			auto &player = currentClient_->get_player();
			player.addCharacter(character);
			characterDrawPile_.erase(it);
			auto &socket = currentClient_->get_socket();
			socket.write("\r\nJe hebt nu de kaart:" + character.getInfo());
			if (start_)
			{
				switchActivePlayer(eWait, eTakeCharacter);
				start_ = false;
				takeCharacters();
			}
			else
			{
				player.setState(eDiscardCharacter);
				discardCharacters();
			}
			return;
		}
	}
	auto &socket = currentClient_->get_socket();
	socket.write("Karakter niet gevonden, voer ander nummer in.\r\n");
}

void Game::discardCharacter(const int name)
{
	for (auto it = characterDrawPile_.begin(); it != characterDrawPile_.end(); it++)
	{
		auto character = *it;
		if (character.getNumber() == name)
		{
			characterDiscardPile_.push_back(character);
			characterDrawPile_.erase(it);
			auto &socket = currentClient_->get_socket();
			socket.write("\r\nDeze kaart is weggelegd:" + character.getInfo());
			switchActivePlayer(eWait, eTakeCharacter);
			takeCharacters();
			return;
		}
	}
	auto &socket = currentClient_->get_socket();
	socket.write("\r\nKarakter niet gevonden, voer ander nummer in.\r\n");
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
	currentCharacter_ = koning;
	currentClient_ = client1_;
	start_ = true;
	auto card = characterDrawPile_.back();
	characterDrawPile_.pop_back();
	auto &socket = currentClient_->get_socket();
	auto &player = currentClient_->get_player();
	player.setState(eTakeCharacter);
	player.setKing(true);
	socket.write("\r\nDeze kaart wordt weggelegd:\r\n");
	socket.write(card.getInfo());
	socket.write("\r\nKies een van deze kaarten om te houden:\r\n");
	for (auto card : characterDrawPile_)
	{
		card.setAbility(true);
		socket.write(card.getInfo());
	}
}

void Game::takeCharacters()
{
	if (characterDrawPile_.empty())
	{
		// TODO turn start
		return;
	}
	auto &socket = currentClient_->get_socket();
	socket.write("\r\nKies een van deze kaarten om te houden:\r\n");
	for (auto card : characterDrawPile_)
	{
		socket.write(card.getInfo());
	}
}

void Game::discardCharacters()
{
	auto &socket = currentClient_->get_socket();
	socket.write("\r\nKies een van deze kaarten om af te leggen:\r\n");
	for (auto card : characterDrawPile_)
	{
		socket.write(card.getInfo());
	}
}

void Game::nextCharacter()
{
	switch(currentCharacter_)
	{
		case koning:
			currentCharacter_ = moordenaar;
			break;
		case moordenaar:
			currentCharacter_ = dief;
			break;
		case dief:
			currentCharacter_ = magier;
			break;
		case magier:
			currentCharacter_ = prediker;
			break;
		case prediker:
			currentCharacter_ = koopman;
			break;
		case koopman:
			currentCharacter_ = bouwmeester;
			break;
		case bouwmeester:
			currentCharacter_ = condottiere;
			break;
		case condottiere:
			currentCharacter_ = koning;
			break;
		default:
			break;
	}
}

bool Game::findNextCharacter()
{
	auto &player = client1_->get_player();
	if (player.hasCharacter(currentCharacter_))
	{
		currentClient_ = client1_;
		return true;
	}
	player = client2_->get_player();
	if (player.hasCharacter(currentCharacter_))
	{
		currentClient_ = client2_;
		return true;
	}
	return false;
}

void Game::switchActivePlayer(const int stateCurrentplayer, const int stateNextplayer)
{
	if (currentClient_ == client1_)
	{
		currentClient_->get_player().setState(stateCurrentplayer);
		currentClient_ = client2_;
		currentClient_->get_player().setState(stateNextplayer);
	}
	else
	{
		currentClient_->get_player().setState(stateCurrentplayer);
		currentClient_ = client1_;
		currentClient_->get_player().setState(stateNextplayer);
	}
}

void Game::newRound()
{
	auto &player1 = client1_->get_player();
	auto &player2 = client2_->get_player();
	if (player1.hasKing())
	{
		player1.setKing(true);
		player2.setKing(false);
		currentClient_ = client1_;
	}
	else if (player2.hasKing())
	{
		player2.setKing(true);
		player1.setKing(false);
		currentClient_ = client2_;
	}
	else if (player1.wasKing())
		currentClient_ = client1_;
	else
		currentClient_ = client2_;
}
