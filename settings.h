//settings.h
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

//Last edited: 27-03-2025

#ifndef SettingsH
#define SettingsH

enum Target {
	CLIENT,
	SERVER,
	PLAYERINPUT1,
	PLAYERINPUT2
};//target

class Settings {
	public:
		//constructor, fetches all the saved settings from the settings file
		//and makes one if it doesn't exist yet
		Settings();
		//set path to target, returns -1 if path is invalid
		int setPath(enum Target file);
		//return path to target
		std::string getPath(enum Target file);

	private:
		//save path to target to the settings file, returns -1 on failure
		int savePath(enum Target file, std::string path);
		//load paths to target from the settings file, returns -1 on failure
		std::string loadPath(enum Target file);

		//path to client
		std::string clientPath;
		//path to server
		std::string serverPath;
		//path to input for player 1
		std::string playerInput1Path;
		//path to input for player 2
		std::string playerInput2Path;
		//amount of settings, equals lines in the settings file
		const int settingsCount = 4;
};//settings

#endif