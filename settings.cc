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
		std::ofstream newSettings("settings");

		for (int i = 0; i < settingsCount; i++)
		{
			newSettings << "None\n";
		}//for
		newSettings.close();

		//set current paths to NULL
		clientPath = "None";
		serverPath = "None";
		playerInput1Path = "None";
		playerInput2Path = "None";
		return;
	}

	//if it does exist, load in the paths
	clientPath = loadPath(CLIENT);
	serverPath = loadPath(SERVER);
	playerInput1Path = loadPath(PLAYERINPUT1);
	playerInput2Path = loadPath(PLAYERINPUT2);
	return;
}//constructor

int Settings::setPath(enum Target file)
{
	//get path to file
	std::cout << "path to file: ";
	std::string path;
	std::cin >> path;

	//switch / with \ for the exists function
	for(int i = 0; i < path.length(); i++)
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
		for(int i = 0; i < path.length(); i++)
		{
			if(path[i] == '\\')
			{
				path[i] = '/';
			}//if
		}//for
	}//while

	//write to correct target
	switch(file)
	{
		case CLIENT:
			clientPath = path;
			savePath(file, path);
			break;
		case SERVER:
			serverPath = path;
			savePath(file, path);
			break;
		case PLAYERINPUT1:
			playerInput1Path = path;
			savePath(file, path);
			break;
		case PLAYERINPUT2:
			playerInput2Path = path;
			savePath(file, path);
			break;
	}//switch
	return 0;
}//setPath

std::string Settings::getPath(enum Target file)
{
	//return correct path
	switch(file)
	{
		case CLIENT:
			return clientPath;
			break;
		case SERVER:
			return serverPath;
			break;
		case PLAYERINPUT1:
			return playerInput1Path;
			break;
		case PLAYERINPUT2:
			return playerInput2Path;
			break;
	}//switch
	return NULL;
}//getPath

int Settings::savePath(enum Target file, std::string path)
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

	//save correct path
	editSettings[file] = path;

	//overwrite old settings file
	std::ofstream writeFile;
	writeFile.open("settings");
	for (int i = 0; i < settingsCount; i++)
	{
		writeFile << editSettings[i] + '\n';
	}//for
	writeFile.close();
	return 0;
}//savePath

std::string Settings::loadPath(enum Target file)
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

	//return target path
	return loadSettings[file];
}//loadPath