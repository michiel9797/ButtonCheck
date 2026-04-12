//settings.h
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

#ifndef SettingsH
#define SettingsH

enum Target 
{
	CLIENT,
	SERVER,
	PLAYERINPUT1,
	PLAYERINPUT2,
	NETEM,
	SKIPEMULATION,
	SKIPSIMULATION,
	SAVEEMULATION,
	SAVESIMULATION,
	APPLYNETEM,
	GILBERTELLIOTT,
	FPS,
	CUSTOMCLIENT,
	CUSTOMSERVER
};//target

class Settings 
{
	public:
		//constructor, fetches all the saved settings from the settings file
		//or makes one if it doesn't exist yet
		Settings();
		//set path to target
		int setPath(enum Target file);
		//set a string
		int setString(enum Target option);
		//set a bool
		int setBool(enum Target option);
		//set a uint
		int setUInt(enum Target number);
		//return path to target
		std::string getSetting(enum Target option);

	private:
		//save path to target to the settings file, returns -1 on failure
		int saveSetting(enum Target option, std::string path);
		//load paths to target from the settings file, returns "" on failure
		std::string loadSetting(enum Target option);
		//amount of settings, equals lines in the settings file
		const static int settingsCount = 14;
		//array containing all settings
		//0:  path to client
		//1:  path to server
		//2:  path to input for player 1
		//3:  path to input for player 2
		//4:  path to netem file
		//5:  should the emulation step be skipped
		//6:  should the simulation step be skipped
		//7:  should the emulation run file be saved
		//8:  should the simulation run file be saved
		//9:  should network emulation be applied
		//10: should the Gilbert-Elliott packet loss model be applied
		//11: fps the client program runs at
		//12: custom client call argument
		//13: custom server call argument
		std::string settingsList[settingsCount];
};//settings

#endif