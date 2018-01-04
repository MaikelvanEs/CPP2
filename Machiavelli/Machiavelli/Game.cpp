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
	if (currentCharacter_ == moordenaar)
	{
		newRound();
	}
	else
	{
		while (!findNextCharacter())
		{
			nextCharacter();
			if (currentCharacter_ == moordenaar)
			{
				newRound();
			}
		}
		newTurn();
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

void Game::turnStartChoice(const int choice)
{
	switch (choice)
	{
	case 0:
		showOpponent();
		break;
	case 1:
		currentClient_->get_player().useCharacter(currentCharacter_, currentClient_->get_socket());
		break;
	case 2:
		takeGold();
		break;
	case 3:
		takeBuildings();
		break;
	default:
		break;
	}
}

std::string Game::getHelp() const
{
	Overzichtskaart oCard;
	return oCard.getInfo();
}

void Game::killCharacter(const int choice) const
{
	if (client1_->get_player().hasCharacter(choice))
		client1_->get_player().killCharacter(choice);
	else if (client2_->get_player().hasCharacter(choice))
		client2_->get_player().killCharacter(choice);
}

void Game::stealFromCharacter(const int choice) const
{
	if (client1_->get_player().hasCharacter(choice))
		client1_->get_player().steal();
	else if (client2_->get_player().hasCharacter(choice))
		client2_->get_player().steal();
}

void Game::swapHands() const
{
	const auto buildings1 = client1_->get_player().getHand();
	const auto buildings2 = client2_->get_player().getHand();
	client1_->get_player().setHand(buildings2);
	client2_->get_player().setHand(buildings1);
}

void Game::showAliveCharacters(Socket& socket, const int startName)
{
	std::vector<int> alive;
	for (auto character : characterDiscardPile_)
		if (character.isAlive() && character.getNumber() > startName)
			alive.push_back(character.getNumber());
	auto client1Alive = client1_->get_player().getAliveCharacters(startName);
	for (auto character : client1Alive)
		alive.push_back(character);
	auto client2Alive = client2_->get_player().getAliveCharacters(startName);
	for (auto character : client2Alive)
		alive.push_back(character);
	sort(alive.begin(), alive.end(), [](int x, int y) { return x < y; });
	for (auto character : alive)
	{
		switch (character)
		{
		case moordenaar:
			socket.write("[0] Moordenaar\r\n");
			break;
		case dief:
			socket.write("[1] Dief\r\n");
			break;
		case magier:
			socket.write("[2] Magier\r\n");
			break;
		case koning:
			socket.write("[3] Koning\r\n");
			break;
		case prediker:
			socket.write("[4] Prediker\r\n");
			break;
		case koopman:
			socket.write("[5] Koopman\r\n");
			break;
		case bouwmeester:
			socket.write("[6] Bouwmeester\r\n");
			break;
		case condotierre:
			socket.write("[7] Condotierre\r\n");
			break;
		default:
			break;
		}
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
	currentCharacter_ = moordenaar;
	currentClient_ = client1_;
	startRound();
}

void Game::takeCharacters()
{
	if (characterDrawPile_.empty())
	{
		currentClient_->get_player().setState(eTurnStart);
		newTurn();
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
		case moordenaar:
			currentCharacter_ = dief;
			break;
		case dief:
			currentCharacter_ = magier;
			break;
		case magier:
			currentCharacter_ = koning;
			break;
		case koning:
			currentCharacter_ = prediker;
			break;
		case prediker:
			currentCharacter_ = koopman;
			break;
		case koopman:
			currentCharacter_ = bouwmeester;
			break;
		case bouwmeester:
			currentCharacter_ = condotierre;
			break;
		case condotierre:
			currentCharacter_ = moordenaar;
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
		client2_->get_player().setState(eWait);
		currentClient_ = client1_;
		player.setState(eTurnStart);
		return true;
	}
	player = client2_->get_player();
	if (player.hasCharacter(currentCharacter_))
	{
		client1_->get_player().setState(eWait);
		currentClient_ = client2_;
		player.setState(eTurnStart);
		return true;
	}
	return false;
}

void Game::switchActivePlayer(const int stateCurrentplayer, const int stateNextplayer)
{
	currentClient_->get_socket().write("\r\nEinde beurt, wacht op andere speler\r\n");
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
	startRound();
}

void Game::startRound()
{
	buildingDeck_.shuffle_stack();
	for (auto character : characterDiscardPile_)
		characterDrawPile_.push_back(character);
	characterDiscardPile_.clear();
	characterDrawPile_.push_back(client1_->get_player().getCharacter());
	characterDrawPile_.push_back(client1_->get_player().getCharacter());
	characterDrawPile_.push_back(client2_->get_player().getCharacter());
	characterDrawPile_.push_back(client2_->get_player().getCharacter());
	random_shuffle(characterDrawPile_.begin(), characterDrawPile_.end());
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

void Game::newTurn() const
{
	checkThief();
	auto &socket = currentClient_->get_socket();
	auto &player = currentClient_->get_player();
	showCharacter(socket);
	socket.write("\r\n");
	player.showGold(socket);
	socket.write("\r\n");
	player.showBuildings(socket);
	socket.write("\r\n");
	player.showCards(socket);
	socket.write("\r\n");
	player.showChoices(socket);
}

void Game::showCharacter(Socket& socket) const
{
	socket.write("Je bent nu de: ");
	switch (currentCharacter_)
	{
	case moordenaar:
		socket.write("Moordenaar");
		break;
	case dief:
		socket.write("Dief");
		break;
	case magier:
		socket.write("Magier");
		break;
	case koning:
		socket.write("Koning");
		break;
	case prediker:
		socket.write("Prediker");
		break;
	case koopman:
		socket.write("Koopman");
		break;
	case bouwmeester:
		socket.write("Bouwmeester");
		break;
	case condotierre:
		socket.write("Condottiere");
		break;
	default: 
		break;
	}
}

void Game::showOpponent() const
{
	auto &socket = currentClient_->get_socket();
	auto &player = client1_->get_player();
	if (currentClient_ == client1_)
	{
		player = client2_->get_player();
	}
	player.showGold(socket);
	socket.write("\r\n");
	player.showBuildings(socket);
}

void Game::takeGold()
{
	for (int get = 2; get > 0 && gold_ != 0; gold_--, get--)
		currentClient_->get_player().addGold();
}

void Game::takeBuildings()
{
	currentClient_->get_player().drawBuilding(buildingDeck_.draw());
	currentClient_->get_player().drawBuilding(buildingDeck_.draw());
}

void Game::checkThief() const
{
	int gold = 0;
	if (client1_->get_player().isStolen())
		gold = client1_->get_player().giveGold();
	else if (client2_->get_player().isStolen())
		gold = client2_->get_player().giveGold();
	if (gold != 0)
	{
		if (client1_->get_player().hasCharacter(dief))
			client1_->get_player().addGold(gold);
		else if (client2_->get_player().hasCharacter(dief))
			client2_->get_player().addGold(gold);
	}
}
