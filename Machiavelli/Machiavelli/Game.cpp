#include "Game.h"

Game::Game()
{
	gold_ = 26;
	firstToEight_ = 0;
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
		client1_->get_player().setPlaying(true);
		client2_->get_player().setPlaying(true);
		startGame();
	}
}

void Game::endGame() const
{
	client1_->get_socket().write("\r\nSpel wordt afgesloten.\r\n");
	client2_->get_socket().write("\r\nSpel wordt afgesloten.\r\n");
	showVictor();
}

void Game::setFirstToEight()
{
	if (firstToEight_ == 0)
	{
		if (currentClient_ == client1_)
			firstToEight_ = 1;
		else
			firstToEight_ = 2;
	}
}

void Game::showVictor() const
{
	const int buildings1 = client1_->get_player().countBuildings();
	const int buildings2 = client2_->get_player().countBuildings();
	int score1 = buildings1;
	int score2 = buildings2;
	score1 += client1_->get_player().checkColours();
	score2 += client2_->get_player().checkColours();
	if (firstToEight_ == 1)
	{
		score1 += 4;
		score2 += client2_->get_player().checkBuildingAmount();
	}
	else if (firstToEight_ == 2)
	{
		score2 += 4;
		score1 += client1_->get_player().checkBuildingAmount();
	}
	if (score1 > score2)
	{
		client1_->get_socket().write("\r\nJe hebt gewonnen.\r\n");
		client2_->get_socket().write("\r\nJe hebt verloren.\r\n");
	}
	else if (score2 > score1)
	{
		client2_->get_socket().write("\r\nJe hebt gewonnen.\r\n");
		client1_->get_socket().write("\r\nJe hebt verloren.\r\n");
	}
	else
	{
		if (buildings1 > buildings2)
		{
			client1_->get_socket().write("\r\nJe hebt gewonnen.\r\n");
			client2_->get_socket().write("\r\nJe hebt verloren.\r\n");
		}
		else if (buildings2 > buildings1)
		{
			client2_->get_socket().write("\r\nJe hebt gewonnen.\r\n");
			client1_->get_socket().write("\r\nJe hebt verloren.\r\n");
		}
		else
		{
			client1_->get_socket().write("\r\nHet is een gelijkspel.\r\n");
			client2_->get_socket().write("\r\nHet is een gelijkspel.\r\n");
		}
	}
}

void Game::endTurn()
{
	nextCharacter();
	if (currentCharacter_ == start)
	{
		newRound();
	}
	else
	{
		while (!findNextCharacter())
		{
			nextCharacter();
			if (currentCharacter_ == start)
			{
				newRound();
			}
		}
		newTurn();
	}
}

void Game::showChoices() const
{

	currentClient_->get_player().showChoices(currentClient_->get_socket());
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
		currentClient_->get_player().setState(eBuild);
		currentClient_->get_player().showChoices(currentClient_->get_socket());
		break;
	case 3:
		takeBuildings();
		currentClient_->get_player().setState(eDiscardBuilding);
		break;
	default:
		break;
	}
}

void Game::buildChoice(const int choice)
{
	switch(choice)
	{
	case 0:
		showOpponent();
		break;
	case 1:
		currentClient_->get_player().useCharacter(currentCharacter_, currentClient_->get_socket());
		break;
	case 2:
		currentClient_->get_socket().write("\r\nKies een gebouw om te bouwen.\r\n");
		currentClient_->get_player().setState(eBuildBuilding);
		currentClient_->get_player().showCards(currentClient_->get_socket());
		break;
	case 9:
		currentClient_->get_socket().write("\r\nBeurt is beeindigd\r\n");
		endTurn();
		break;
	default:
		break;
	}
}

void Game::useCharacter(const int choice) const
{
	currentClient_->get_player().useCharacter(currentCharacter_, currentClient_->get_socket(), choice);
}

void Game::endChoice(const int choice)
{
	switch (choice)
	{
	case 0:
		showOpponent();
		break;
	case 1:
		currentClient_->get_player().useCharacter(currentCharacter_, currentClient_->get_socket());
		break;
	case 9:
		currentClient_->get_socket().write("\r\nBeurt is beeindigd\r\n");
		endTurn();
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

void Game::write(const std::string text) const
{
	currentClient_->get_socket().write(text);
}

void Game::killCharacter(const int choice) const
{
	if (client1_->get_player().hasCharacter(choice))
		client1_->get_player().killCharacter(choice, client1_->get_socket());
	else if (client2_->get_player().hasCharacter(choice))
		client2_->get_player().killCharacter(choice, client2_->get_socket());
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
	client1_->get_socket().write("\r\nKaarten zijn omgeruild met de kaarten van de tegenstander.\r\n");
	client2_->get_socket().write("\r\nKaarten zijn omgeruild met de kaarten van de tegenstander.\r\n");
}

void Game::getGold(int gold)
{
	for (; gold > 0 && gold_ != 0; gold_--, gold--)
		currentClient_->get_player().addGold();
}

void Game::payGold(int gold)
{
	for (; gold > 0; gold_++, gold--)
		currentClient_->get_player().removeGold();
}

void Game::getBuildings(int amount)
{
	for (; amount > 0 && !buildingDeck_.stack_empty(); amount--)
		currentClient_->get_player().drawBuilding(buildingDeck_.draw());
}

int Game::getOpponentBuildings() const
{
	if (currentClient_ == client1_)
		return client2_->get_player().getBuildingAmount();
	return client1_->get_player().getBuildingAmount();
}

void Game::showOpponentBuildings() const
{
	currentClient_->get_socket().write("\r\n");
	if (currentClient_ == client1_)
		client2_->get_player().showBuildings(client1_->get_socket());
	else
		client1_->get_player().showBuildings(client2_->get_socket());
}

bool Game::checkRemoveBuilding(const int choice, const int gold) const
{
	if (currentClient_ == client1_)
		return client2_->get_player().checkBuilding(choice, gold);
	return client1_->get_player().checkBuilding(choice, gold);
}

bool Game::checkOpponentPrediker() const
{
	if (currentClient_ == client1_)
		return client2_->get_player().checkPrediker();
	return client1_->get_player().checkPrediker();
}

int Game::removeOpponentBuilding(const int choice)
{
	if (currentClient_ == client1_)
	{
		auto building = client2_->get_player().removeBuilding(choice);
		client2_->get_socket().write("\r\nJe " + building.getName() + " is weggehaald.\r\n");
		buildingDeck_.discard(building);
		return building.getValue() - 1;
	}
	auto building = client1_->get_player().removeBuilding(choice);
	client1_->get_socket().write("\r\nJe " + building.getName() + " is weggehaald.\r\n");
	buildingDeck_.discard(building);
	return building.getValue() - 1;
}

void Game::discardBuilding(const Building building)
{
	buildingDeck_.discard(building);
}

Building Game::drawBuilding()
{
	return buildingDeck_.draw();
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
	sort(alive.begin(), alive.end(), [](int x, int y) { return x < y; }); //lambda expression
	for (auto character : alive)
	{
		switch (character)
		{
		case moordenaar:
			socket.write("[1] Moordenaar\r\n");
			break;
		case dief:
			socket.write("[2] Dief\r\n");
			break;
		case magier:
			socket.write("[3] Magier\r\n");
			break;
		case koning:
			socket.write("[4] Koning\r\n");
			break;
		case prediker:
			socket.write("[5] Prediker\r\n");
			break;
		case koopman:
			socket.write("[6] Koopman\r\n");
			break;
		case bouwmeester:
			socket.write("[7] Bouwmeester\r\n");
			break;
		case condotierre:
			socket.write("[8] Condotierre\r\n");
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
	currentCharacter_ = start;
	currentClient_ = client1_;
	for (int i = 0; i < 4; i++)
	{
		client1_->get_player().drawBuilding(buildingDeck_.draw());
		client2_->get_player().drawBuilding(buildingDeck_.draw());
	}
	client1_->get_player().convertHand();
	client2_->get_player().convertHand();
	startRound();
}

void Game::takeCharacters()
{
	if (characterDrawPile_.empty())
	{
		endTurn();
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
	case start:
		currentCharacter_ = moordenaar;
		break;
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
		currentCharacter_ = start;
		break;
	default:
		break;
	}
}

bool Game::findNextCharacter()
{
	auto &player1 = client1_->get_player();
	if (player1.hasCharacter(currentCharacter_))
	{
		client2_->get_player().setState(eWait);
		currentClient_ = client1_;
		player1.setState(eTurnStart);
		return true;
	}
	auto &player2 = client2_->get_player();
	if (player2.hasCharacter(currentCharacter_))
	{
		client1_->get_player().setState(eWait);
		currentClient_ = client2_;
		player2.setState(eTurnStart);
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
	if (firstToEight_ != 0)
		endGame();
	else
	{
		client1_->get_socket().write("\r\nNieuwe ronde gestart.\r\n");
		client2_->get_socket().write("\r\nNieuwe ronde gestart.\r\n");
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
		characterDrawPile_.push_back(client1_->get_player().getCharacter());
		characterDrawPile_.push_back(client1_->get_player().getCharacter());
		characterDrawPile_.push_back(client2_->get_player().getCharacter());
		characterDrawPile_.push_back(client2_->get_player().getCharacter());
		startRound();
	}
}

void Game::startRound()
{
	buildingDeck_.shuffle_stack();
	for (auto character : characterDiscardPile_)
		characterDrawPile_.push_back(character);
	characterDiscardPile_.clear();
	std::default_random_engine generator;
	generator.seed(time(0));
	std::shuffle(characterDrawPile_.begin(), characterDrawPile_.end(), generator);
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
	player.showChoices(socket);
}

void Game::showCharacter(Socket& socket) const
{
	socket.write("\r\nJe bent nu de: ");
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
	if (currentClient_ == client1_)
	{
		auto &player = client2_->get_player();
		player.showGold(socket);
		socket.write("\r\n");
		player.showBuildings(socket);
	}
	else
	{
		auto &player = client1_->get_player();
		player.showGold(socket);
		socket.write("\r\n");
		player.showBuildings(socket);
	}
}

void Game::takeGold()
{
	int get = 2;
	for (; get > 0 && gold_ != 0; gold_--, get--)
		currentClient_->get_player().addGold();
	currentClient_->get_socket().write("\r\nJe ontvangt " + std::to_string(2-get) +" goudstukken.\r\n");
}

void Game::takeBuildings()
{
	currentClient_->get_player().drawBuilding(buildingDeck_.draw());
	currentClient_->get_player().drawBuilding(buildingDeck_.draw());
	currentClient_->get_player().showBuildingChoices(currentClient_->get_socket());
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
