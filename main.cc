//main.cc
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

//Last edited: 28-03-2025

#include <iostream>
#include "settings.h"

//Options menu for the benchmark.
void menu(Settings &CurrentSettings)
{
	//Basic menu loop
	bool runProgram = true;
	std::cout << "| ButtonCheck: Netcode benchmark for competitive multiplayer games" << std::endl
	<< "| Made by Michiel van der Bijl" << std::endl
	<< "|" << std::endl
	<< "| Path to game client:	" << CurrentSettings.getPath(CLIENT) << std::endl
	<< "| Path to game server:	" << CurrentSettings.getPath(SERVER) << std::endl
	<< "| Path to player 1 input: " << CurrentSettings.getPath(PLAYERINPUT1) << std::endl
	<< "| Path to player 2 input: " << CurrentSettings.getPath(PLAYERINPUT2) << std::endl
	<< "__________________________________________________________________" << std::endl
	<< "1: set game client" << std::endl
	<< "2: set game server" << std::endl
	<< "3: set player 1 input" << std::endl
	<< "4: set player 2 input" << std::endl
	<< "5: more options" << std::endl
	<< "6: run benchmark" << std::endl
	<< "7: end program" << std::endl;
	while (runProgram)
	{
		std::string input;
		std::cin >> input;
		if (input == "1")
		{
			CurrentSettings.setPath(CLIENT);
		}else if (input == "2")
		{
			CurrentSettings.setPath(SERVER);
		}else if (input == "3")
		{
			CurrentSettings.setPath(PLAYERINPUT1);
		}else if (input == "4")
		{
			CurrentSettings.setPath(PLAYERINPUT2);
		}else if (input == "5")
		{
			//subMenu
		}else if (input == "6")
		{
			//runBenchmark
		}else if (input == "7")
		{
			runProgram = false;
		}//else if
	}//while	
}//menu

//Do any possibly needed initialization, then start the menu.
int main()
{
	Settings CurrentSettings;
	menu(CurrentSettings);
	return 0;
}//main