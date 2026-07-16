//main.cc
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

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
		<< "| Display final frame count?    (y/n)   " << CurrentSettings.getSetting(FINALFRAMECOUNT) << std::endl
		<< "| FPS of the client program		" << CurrentSettings.getSetting(FPS) << std::endl
		<< "| Custom client call field		" << CurrentSettings.getSetting(CUSTOMCLIENT) << std::endl
		<< "| Custom server call field		" << CurrentSettings.getSetting(CUSTOMSERVER) << std::endl
		<< "_____________________________________________________________________________________________________" << std::endl
		<< "1: Set skip emulation step" << std::endl
		<< "2: Set skip simulation step" << std::endl
		<< "3: Set save emulation" << std::endl
		<< "4: Set save simulation" << std::endl
		<< "5: Set apply network emulation" << std::endl
		<< "6: Set Gilbert-Elliott model" << std::endl
		<< "7: Set final frame count display" << std::endl
		<< "8: Set client FPS" << std::endl
		<< "9: Set custom client call field" << std::endl
		<< "10: Set custom server call field" << std::endl
		<< "11: Return to main menu" << std::endl;
		std::cout << "Choose option: ";
		std::string input;
		std::cin >> input;
		switch(stoi(input))
		{
			case 1: 
				std::cout << std::endl;
				CurrentSettings.setBool(SKIPEMULATION);
				break;
			case 2:
				std::cout << std::endl;
				CurrentSettings.setBool(SKIPSIMULATION);
				break;
			case 3:
				std::cout << std::endl;
				CurrentSettings.setBool(SAVEEMULATION);
				break;
			case 4:
				std::cout << std::endl;
				CurrentSettings.setBool(SAVESIMULATION);
				break;
			case 5:
				std::cout << std::endl;
				CurrentSettings.setBool(APPLYNETEM);
				break;
			case 6:
				std::cout << std::endl;
				CurrentSettings.setBool(GILBERTELLIOTT);
				break;
			case 7:
			        std::cout << std::endl;
			        CurrentSettings.setBool(FINALFRAMECOUNT);
			        break;
			case 8:
				std::cout << std::endl;
				CurrentSettings.setUInt(FPS);
				break;
			case 9:
				std::cout << std::endl;
				CurrentSettings.setString(CUSTOMCLIENT);
				break;
			case 10:
				std::cout << std::endl;
				CurrentSettings.setString(CUSTOMSERVER);
				break;
			case 11:
				std::cout << std::endl;
				runSubMenu = false;
				break;
		}//switch
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
		switch(stoi(input))
		{
			case 1:
				std::cout << std::endl;
				CurrentSettings.setPath(CLIENT);
				break;
			case 2:
				std::cout << std::endl;
				CurrentSettings.setPath(SERVER);
				break;
			case 3:
				std::cout << std::endl;
				CurrentSettings.setPath(PLAYERINPUT1);
				break;
			case 4:
				std::cout << std::endl;
				CurrentSettings.setPath(PLAYERINPUT2);
				break;
			case 5:
				std::cout << std::endl;
				CurrentSettings.setPath(NETEM);
				break;
			case 6:
				std::cout << std::endl;
				subMenu(CurrentSettings);
				break;
			case 7:
			{
				std::cout << std::endl;
				Benchmark benchy(CurrentSettings);
				benchy.startBenchmark(CurrentSettings);
				break;
			}//case
			case 8:
				runProgram = false;
				break;
		}//switch
	}//while	
}//menu

//Do any possibly needed initialization, then start the menu.
int main(int argc, char * argv[])
{
	Settings CurrentSettings;
	std::string execMode;
	switch(argc)
	{
		case 1:
		{
			menu(CurrentSettings);
			break;
		}//case
		case 2:
		{
			execMode = argv[1];
			if(execMode == "--help")
			{
				std::cout << "./ButtonCheck [-F {count}] " << std::endl;
				std::cout << "-F: force run, runs the benchmark using the settings found in the settings file" << std::endl;
				std::cout << "{count}: the number of runs -F will execute consecutively, defaults to 1 if not specified" << std::endl;
			}else if(execMode == "-F")
			{
				Benchmark benchy(CurrentSettings);
				benchy.startBenchmark(CurrentSettings);
			}else{
				std::cerr << "Incorrect program call, call \"ButtonCheck --help\" for instructions" << std::endl;
				return -1;
			}//else
			break;
		}//case
		case 3:
		{
			execMode = argv[1];
			if(execMode == "-F")
			{
				int runs = std::stoi(argv[2]);
				for(int i = 0; i < runs; i++)
				{
					Benchmark benchy(CurrentSettings);
					benchy.startBenchmark(CurrentSettings);
				}//for
			}else{
				std::cerr << "Incorrect program call, call \"ButtonCheck --help\" for instructions" << std::endl;
				return -1;
			}//else
			break;
		}//case
		default:
		{
			std::cerr << "Incorrect program call, call \"ButtonCheck --help\" for instructions" << std::endl;
			return -1;
		}//default
	}//switch

	return 0;
}//main
