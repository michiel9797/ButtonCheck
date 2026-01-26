//main.cc
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

//Last edited: 28-03-2025

#include <iostream>
#include "settings.h"
#include "benchmark.h"

//a list of additional options
void subMenu(Settings &CurrentSettings)
{
	bool runSubMenu = true;
	while(runSubMenu)
	{
		std::cout << std::endl;
		std::cout << "| ButtonCheck: Netcode benchmark for competitive multiplayer games" << std::endl
		<< "| Made by Michiel van der Bijl" << std::endl
		<< "|" << std::endl
		<< "| Skip emulation step?		(y/n)	" << CurrentSettings.getSetting(SKIPEMULATION) << std::endl
		<< "| Skip simulation step?		(y/n)	" << CurrentSettings.getSetting(SKIPSIMULATION) << std::endl
		<< "| Save emulation file?		(y/n)	" << CurrentSettings.getSetting(SAVEEMULATION) << std::endl
		<< "| Save simulation file? 	(y/n)	" << CurrentSettings.getSetting(SAVESIMULATION) << std::endl
		<< "| Apply network emulation?  	(y/n)	" << CurrentSettings.getSetting(APPLYNETEM) << std::endl
		<< "| Use Gilbert-Elliott model?	(y/n)	" << CurrentSettings.getSetting(GILBERTELLIOTT) << std::endl
		<< "| FPS of the client program		" << CurrentSettings.getSetting(FPS) << std::endl
		<< "_____________________________________________________________________________________________________" << std::endl
		<< "1: Set skip emulation step" << std::endl
		<< "2: Set skip simulation step" << std::endl
		<< "3: Set save emulation" << std::endl
		<< "4: Set save simulation" << std::endl
		<< "5: Set apply network emulation" << std::endl
		<< "6: Set Gilbert-Elliott model" << std::endl
		<< "7: Set client FPS" << std::endl
		<< "8: Return to main menu" << std::endl;
		std::cout << "Choose option: ";
		std::string input;
		std::cin >> input;
		if(input == "1")
		{
			std::cout << std::endl;
			CurrentSettings.setBool(SKIPEMULATION);
		}else if(input == "2")
		{
			std::cout << std::endl;
			CurrentSettings.setBool(SKIPSIMULATION);
		}else if(input == "3")
		{
			std::cout << std::endl;
			CurrentSettings.setBool(SAVEEMULATION);
		}else if(input == "4")
		{
			std::cout << std::endl;
			CurrentSettings.setBool(SAVESIMULATION);
		}else if(input == "5")
		{
			std::cout << std::endl;
			CurrentSettings.setBool(APPLYNETEM);
		}else if(input == "6")
		{
			std::cout << std::endl;
			CurrentSettings.setBool(GILBERTELLIOTT);
		}else if(input == "7")
		{
			std::cout << std::endl;
			CurrentSettings.setUInt(FPS);
		}else if(input == "8")
		{
			std::cout << std::endl;
			runSubMenu = false;
		}//else if
	}//while	
}//subMenu

//Options menu for the benchmark.
void menu(Settings &CurrentSettings)
{
	//Basic menu loop
	bool runProgram = true;
	while(runProgram)
	{
		std::cout << std::endl;
		std::cout << "| ButtonCheck: Netcode benchmark for competitive multiplayer games" << std::endl
		<< "| Made by Michiel van der Bijl" << std::endl
		<< "|" << std::endl
		<< "| Path to game client:			" << CurrentSettings.getSetting(CLIENT) << std::endl
		<< "| Path to game server:			" << CurrentSettings.getSetting(SERVER) << std::endl
		<< "| Path to player 1 input:		" << CurrentSettings.getSetting(PLAYERINPUT1) << std::endl
		<< "| Path to player 2 input:		" << CurrentSettings.getSetting(PLAYERINPUT2) << std::endl
		<< "| Path to network emulation file:	" << CurrentSettings.getSetting(NETEM) << std::endl
		<< "_____________________________________________________________________________________________________" << std::endl
		<< "1: Set game client" << std::endl
		<< "2: Set game server" << std::endl
		<< "3: Set player 1 input" << std::endl
		<< "4: Set player 2 input" << std::endl
		<< "5: Set network emulation file" << std::endl
		<< "6: More options" << std::endl
		<< "7: Run benchmark" << std::endl
		<< "8: End program" << std::endl;
		std::cout << "Choose option: ";
		std::string input;
		std::cin >> input;
		if(input == "1")
		{
			std::cout << std::endl;
			CurrentSettings.setPath(CLIENT);
		}else if(input == "2")
		{
			std::cout << std::endl;
			CurrentSettings.setPath(SERVER);
		}else if(input == "3")
		{
			std::cout << std::endl;
			CurrentSettings.setPath(PLAYERINPUT1);
		}else if(input == "4")
		{
			std::cout << std::endl;
			CurrentSettings.setPath(PLAYERINPUT2);
		}else if(input == "5")
		{
			std::cout << std::endl;
			CurrentSettings.setPath(NETEM);
		}else if(input == "6")
		{
			std::cout << std::endl;
			subMenu(CurrentSettings);
		}else if(input == "7")
		{
			std::cout << std::endl;
			Benchmark benchy(CurrentSettings);
			benchy.startBenchmark(CurrentSettings);
		}else if(input == "8")
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