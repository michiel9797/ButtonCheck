//benchmark.cc
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

//Last edited: 30-03-2025

#include <fstream>
#include <iostream>
#include <filesystem>
#include <boost/process.hpp>
#include "benchmark.h"

namespace bp = boost::process;

int Benchmark::startBenchmark(Settings currentSettings)
{
	if (!simulateRun(currentSettings))
	{
		return -1;
	}//if

}//startBenchmark

int Benchmark::simulateRun(Settings currentSettings)
{
	//to read the game state data and other communications from
	bp::ipstream is;
	boost::process::v1::filesystem::path path = currentSettings.getPath(CLIENT);
	std::error_code error;

	//run the client
	bp::child s(path, "SIMULATE " + currentSettings.getPath(PLAYERINPUT1) + " " + 
	currentSettings.getPath(PLAYERINPUT2), bp::std_out > is, error);

	//in case the client launch failed
	if(!error)
	{
		std::cerr << "Client launch failed with error code: " << error << std::endl;
		return -1;
	}//if

	std::string received;
	bool connected = false;
	std::fstream output("simulatedRun", ios::app); 	//make an output file for the received data

	//as long as the booted process is running, receive its output
	while(s.running() && std::getline(is, received) && !received.empty())
	{
		//if this is the first message
		if(!connected)
		{
			//check for the correct identification
			if (received == "SIMULATE START")
			{
				connected = true;
			} else { //if it fails, terminate it
				s.request_exit();
				s.wait();
				std::cerr << "Client did not identify correctly";
				return -1;
			}//else
		} else { //if the identification went correctly
			output << received << "\n";
		}//else
	}//while

	s.wait();

	return 0;
}//simulatedRun