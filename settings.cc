//settings.cc
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

//Last edited: 28-03-2025

#include <iostream>
#include <fstream>
#include <filesystem>
#include "settings.h"

Settings::Settings() 
{
	//open the settings file if it exists
	std::fstream settingsFile;
	settingsFile.open("settings");

	//if it doesnt exist, make a new one
	if (!settingsFile.is_open())
	{		
		//set current settings to a null value
		settingsList[CLIENT] = "None";
		settingsList[SERVER] = "None";
		settingsList[PLAYERINPUT1] = "None";
		settingsList[PLAYERINPUT2] = "None";
		settingsList[SKIPEMULATION] = "n";
		settingsList[SKIPSIMULATION] = "n";
		settingsList[SAVEEMULATION] = "n";
		settingsList[SAVESIMULATION] = "n";

		//load them into the file
		std::ofstream newSettings("settings");

		for (int i = 0; i < settingsCount; i++)
		{
			newSettings << settingsList[i] << std::endl;
		}//for
		newSettings.close();

		return;
	}//if

	//if it does exist, load in the settingss
	settingsList[CLIENT] = loadSetting(CLIENT);
	settingsList[SERVER] = loadSetting(SERVER);
	settingsList[PLAYERINPUT1] = loadSetting(PLAYERINPUT1);
	settingsList[PLAYERINPUT2] = loadSetting(PLAYERINPUT2);
	settingsList[SKIPEMULATION] = loadSetting(SKIPEMULATION);
	settingsList[SKIPSIMULATION] = loadSetting(SKIPSIMULATION);
	settingsList[SAVEEMULATION] = loadSetting(SAVEEMULATION);
	settingsList[SAVESIMULATION] = loadSetting(SAVESIMULATION);
	return;
}//constructor

int Settings::setPath(enum Target file)
{
	//get path to file
	std::cout << "path to file: ";
	std::string path;
	std::cin >> path;

	//switch / with \ for the exists function
	for(unsigned long i = 0; i < path.length(); i++)
	{
		if(path[i] == '\\')
		{
			path[i] = '/';
		}//if
	}//for

	while (!std::filesystem::exists(path))
	{
		std::cerr << "Could not find file, please try again" << std::endl;
		std::cout << "path to file: ";
		std::cin >> path;

		//switch / with \ for the open function
		for(unsigned long i = 0; i < path.length(); i++)
		{
			if(path[i] == '\\')
			{
				path[i] = '/';
			}//if
		}//for
	}//while

	//write to correct target
	settingsList[file] = path;
	saveSetting(file, path);

	return 0;
}//setPath

int Settings::setBool(enum Target option)
{
	std::cout << "[y/n]: ";
	std::string setting;
	std::cin >> setting;

	while(setting != "y" && setting != "n")
	{
		std::cout << "invalid choice, please try again" << std::endl;
		std::cout << "[y/n]: ";
		std::cin >> setting;
	}//while
	
	//write to correct target
	settingsList[option] = setting;
	saveSetting(option, setting);

	return 0;
}//setBool

std::string Settings::getSetting(enum Target option)
{
	//return correct settings
	return settingsList[option];
}//getSetting

int Settings::saveSetting(enum Target option, std::string setting)
{
	//open settings file
	std::ifstream readFile;
	readFile.open("settings");
	if (!readFile.is_open())
	{
		std::cerr << "couldn't find settings file" << std::endl;
		return -1;
	}//if

	//read settings into an array
	std::string editSettings[settingsCount];
	for (int i = 0; i < settingsCount; i++)
	{
		getline(readFile, editSettings[i]);
	}//for
	readFile.close();

	//save correct setting
	editSettings[option] = setting;

	//overwrite old settings file
	std::ofstream writeFile;
	writeFile.open("settings");
	for (int i = 0; i < settingsCount; i++)
	{
		writeFile << editSettings[i] + '\n';
	}//for
	writeFile.close();
	return 0;
}//saveSetting

std::string Settings::loadSetting(enum Target option)
{
	//open settings file
	std::ifstream settingsFile;
	settingsFile.open("settings");
	if (!settingsFile.is_open())
	{
		std::cerr << "couldn't find settings file" << std::endl;
		return NULL;
	}//if

	//read settings into an array
	std::string loadSettings[settingsCount];
	for (int i = 0; i < settingsCount; i++)
	{
		getline(settingsFile, loadSettings[i]);
	}//for
	settingsFile.close();

	//return target option
	return loadSettings[option];
}//loadSettings