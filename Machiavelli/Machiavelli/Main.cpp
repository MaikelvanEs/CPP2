//
//  main.cpp
//  socketexample
//
//  Created by Bob Polis on 16/09/14.
//  Revised by Jeroen de Haas on 22/11/2016
//  Copyright (c) 2014 Avans Hogeschool, 's-Hertogenbosch. All rights reserved.
//

#include <thread>
#include <iostream>
#include <exception>
#include <memory>
#include <utility>
#include <chrono>
#include "Game.h"
using namespace std;

#include "Socket.h"
#include "Sync_queue.h"
#include "ClientCommand.h"
#include "Player.h"
#include "ClientInfo.h"

namespace machiavelli {
    const int tcp_port {1080};
    const string prompt {"machiavelli> "};
}

static bool running = true;

static Sync_queue<ClientCommand> queue;

std::shared_ptr<Game> game;

void consume_command() // runs in its own thread
{
    try {
        while (running) {
            ClientCommand command {queue.get()}; // will block here unless there are still command objects in the queue
            if (auto clientInfo = command.get_client_info().lock()) {
                auto &client = clientInfo->get_socket();
                auto &player = clientInfo->get_player();
                try {
					if (command.get_cmd() == "help")
					{
						client.write("\r\n" + game->getHelp());
					}
					else if (player.isWaiting())
						client.write("\r\nWacht op andere speler, het is niet jouw beurt.\r\n");
                	else
                		player.handleCommand(command.get_cmd());
                } catch (const exception& ex) {
                    cerr << "*** exception in consumer thread for player " << player.getName() << ": " << ex.what() << '\n';
                    if (client.is_open()) {
                        client.write("\r\nOngeldige keuze gemaakt. Probeer opnieuw.\r\n");
                    }
                } catch (...) {
                    cerr << "*** exception in consumer thread for player " << player.getName() << '\n';
                    if (client.is_open()) {
                        client.write("Sorry, something went wrong during handling of your request.\r\n");
                    }
                }
            }
        }
    } catch (...) {
        cerr << "consume_command crashed\n";
    }
}

std::shared_ptr<ClientInfo> init_client_session(Socket client) {
    client.write("Welcome to Server 1.0! To quit, type 'quit'.\r\n");
    client.write("What's your name?\r\n");
    client.write(machiavelli::prompt);
    string name;
    bool done { false };
    while(!done) {
        done = client.readline([&name](std::string input) {
            name = input;
        });
    }
    return make_shared<ClientInfo>(move(client), Player { name });
}

void handle_client(Socket client) // this function runs in a separate thread
{
    try {
        auto client_info = init_client_session(move(client));
        auto &socket = client_info->get_socket();
        auto &player = client_info->get_player();
        socket << "Welcome, " << player.getName() << ", have fun playing our game!\r\n" << machiavelli::prompt;

		socket.write("Wait for other player!\r\n");
		game->addToGame(client_info);
		player.setGame(game);

        while (running) { // game loop
            try {
                // read first line of request
                std::string cmd;
                if (socket.readline([&cmd](std::string input) { cmd=input; })) {
                    cerr << '[' << socket.get_dotted_ip() << " (" << socket.get_socket() << ") " << player.getName() << "] " << cmd << "\r\n";

                    if (cmd == "quit") {
                        socket.write("Bye!\r\n");
						game->endGame();
						running = false;
                        //break; // out of game loop, will end this thread and close connection
                    }
                    else if (cmd == "quit_server") {
                        running = false;
                    }
					ClientCommand command{ cmd, client_info };
					queue.put(command);
                };

            } catch (const exception& ex) {
                socket << "ERROR: " << ex.what() << "\r\n";
            } catch (...) {
                socket.write("ERROR: something went wrong during handling of your request. Sorry!\r\n");
            }
        }
        // close weg
    } 
    catch(std::exception &ex) {
        cerr << "handle_client " << ex.what() << "\n";
    }
    catch (...) {
        cerr << "handle_client crashed\n";
    }
}

int main(int argc, const char * argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	game = make_shared<Game>();

    // start command consumer thread
    vector<thread> all_threads;
    all_threads.emplace_back(consume_command);

    // create a server socket
    ServerSocket server {machiavelli::tcp_port};

    try {
        cerr << "server listening" << '\n';
        while (running) {
            // eWait for connection from client; will create new socket
            server.accept([&all_threads](Socket client) {
                std::cerr << "Connection accepted from " << client.get_dotted_ip() << "\n";
                all_threads.emplace_back(handle_client, move(client));
            });
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    } catch (const exception& ex) {
        cerr << ex.what() << ", resuming..." << '\n';
    } catch (...) {
        cerr << "problems, problems, but: keep calm and carry on!\n";
    }

    for (auto &t : all_threads) {
        t.join();
    }

    return 0;
}
