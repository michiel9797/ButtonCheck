//settings.h
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

//Last edited: 29-03-2025

#ifndef SettingsH
#define SettingsH

enum Target 
{
	CLIENT,
	SERVER,
	PLAYERINPUT1,
	PLAYERINPUT2,
	SKIPEMULATION,
	SKIPSIMULATION,
	SAVEEMULATION,
	SAVESIMULATION
};//target

class Settings 
{
	public:
		//constructor, fetches all the saved settings from the settings file
		//and makes one if it doesn't exist yet
		Settings();
		//set path to target
		int setPath(enum Target file);
		//set a bool
		int setBool(enum Target option);
		//return path to target
		std::string getSetting(enum Target option);

	private:
		//save path to target to the settings file, returns -1 on failure
		int saveSetting(enum Target option, std::string path);
		//load paths to target from the settings file, returns "" on failure
		std::string loadSetting(enum Target option);
		//amount of settings, equals lines in the settings file
		const static int settingsCount = 8;
		//array containing all settings
		//0: path to client
		//1: path to server
		//2: path to input for player 1
		//3: path to input for player 2
		//4: should the emulation step be skipped
		//5: should the simulation step be skipped
		//6: should the emulation run file be saved
		//7: should the simulation run file be saved
		std::string settingsList[settingsCount];
};//settings

#endif