//settings.cc
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

#include <iostream>
#include <fstream>
#include <filesystem>
#include "settings.h"

Settings::Settings() 
{   //open the settings file if it exists
	std::fstream settingsFile;
	settingsFile.open("settings");

	//if it doesnt exist, make a new one
	if(!settingsFile.is_open())
	{		
		//set current settings to a null value
		settingsList[CLIENT] = "None";
		settingsList[SERVER] = "None";
		settingsList[PLAYERINPUT1] = "None";
		settingsList[PLAYERINPUT2] = "None";
		settingsList[NETEM] = "None";
		settingsList[SKIPEMULATION] = "n";
		settingsList[SKIPSIMULATION] = "n";
		settingsList[SAVEEMULATION] = "n";
		settingsList[SAVESIMULATION] = "n";
		settingsList[APPLYNETEM] = "y";
		settingsList[GILBERTELLIOTT] = "n";
		settingsList[FPS] =  "60";
		settingsList[CUSTOMCLIENT] = "";
		settingsList[CUSTOMSERVER] = "";

		//load them into the file
		std::ofstream newSettings("settings");

		for (short i = 0; i < settingsCount; i++)
			 newSettings << settingsList[i] << std::endl;

		newSettings.close();

		return;
	}//if

	//if it does exist, load in the settings
	settingsList[CLIENT] = loadSetting(CLIENT);
	settingsList[SERVER] = loadSetting(SERVER);
	settingsList[PLAYERINPUT1] = loadSetting(PLAYERINPUT1);
	settingsList[PLAYERINPUT2] = loadSetting(PLAYERINPUT2);
	settingsList[NETEM] = loadSetting(NETEM);
	settingsList[SKIPEMULATION] = loadSetting(SKIPEMULATION);
	settingsList[SKIPSIMULATION] = loadSetting(SKIPSIMULATION);
	settingsList[SAVEEMULATION] = loadSetting(SAVEEMULATION);
	settingsList[SAVESIMULATION] = loadSetting(SAVESIMULATION);
	settingsList[APPLYNETEM] = loadSetting(APPLYNETEM);
	settingsList[GILBERTELLIOTT] = loadSetting(GILBERTELLIOTT);
	settingsList[FPS] =  loadSetting(FPS);
	settingsList[CUSTOMCLIENT] = loadSetting(CUSTOMCLIENT);
	settingsList[CUSTOMSERVER] = loadSetting(CUSTOMSERVER);

	return;
}//settings

void Settings::setPath(const enum Target file)
{   //get path to file
	std::cout << "path to file: ";
	std::string path;
	std::cin >> path;

	//switch / with \ for the exists function
	for(unsigned long i = 0; i < path.length(); i++)
		if(path[i] == '\\')
			path[i] = '/';

	while (!std::filesystem::exists(path))
	{
		std::cerr << "Could not find file, please try again" << std::endl;
		std::cout << "path to file: ";
		std::cin >> path;

		//switch / with \ for the open function
		for(unsigned long i = 0; i < path.length(); i++)
			if(path[i] == '\\')
				path[i] = '/';
	}//while

	//write to correct target
	settingsList[file] = path;
	saveSetting(file, path);
}//setPath

void Settings::setString(const enum Target option)
{   //get string
	std::cout << "string: ";
	std::string input;
	std::cin >> input;

	//write to correct target
	settingsList[option] = input;
	saveSetting(option, input); 
}//setString

void Settings::setBool(const enum Target option)
{   //get bool (yes or no)
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
}//setBool

void Settings::setUInt(const enum Target number)
{   //get uint
	bool badValue = true;
	std::cout << "value: ";
	std::string setting;
	std::cin >> setting;

	while(badValue)
	{
		badValue = false;
		for(unsigned long i = 0; i < setting.length(); i++)
		{
			if(setting[i] < '0' || setting[i] >  '9')
			{
				badValue = true;
				std::cout << "invalid value, please try again" << std::endl;
				std::cout << "value: ";
				std::cin >> setting;
				break;
			}//if
		}//for
	}//while

	//write to correct target
	settingsList[number] = setting;
	saveSetting(number, setting);
}//setBool

std::string Settings::getSetting(const enum Target option)
{  //return corresponding settings
	return settingsList[option];
}//getSetting

const short Settings::saveSetting(const enum Target option, const std::string setting)
{   //open settings file
	std::ifstream readFile;
	readFile.open("settings");
	if(!readFile.is_open())
	{
		std::cerr << "couldn't find settings file" << std::endl;
		return -1;
	}//if

	//read settings into an array
	std::string editSettings[settingsCount];
	for (int i = 0; i < settingsCount; i++)
		getline(readFile, editSettings[i]);

	readFile.close();

	//save correct setting
	editSettings[option] = setting;

	//overwrite old settings file
	std::ofstream writeFile;
	writeFile.open("settings");
	for (int i = 0; i < settingsCount; i++)
		writeFile << editSettings[i] + '\n';

	writeFile.close();
	return 0;
}//saveSetting

const std::string Settings::loadSetting(const enum Target option)
{   //open settings file
	std::ifstream settingsFile;
	settingsFile.open("settings");
	if(!settingsFile.is_open())
	{
		std::cerr << "couldn't find settings file" << std::endl;
		return "";
	}//if

	//read settings into an array
	std::string loadSettings[settingsCount];
	for (int i = 0; i < settingsCount; i++)
		getline(settingsFile, loadSettings[i]);

	settingsFile.close();

	//return target option
	return loadSettings[option];
}//loadSettings