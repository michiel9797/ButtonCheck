//benchmark.cc
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

//Last edited: 30-03-2025

#include <fstream>
#include <iostream>
#include <filesystem>
#include <boost/process.hpp>
#include "benchmark.h"
#include "libtuntap/tuntap.h"

namespace bp = boost::process;

Benchmark::~Benchmark()
{
	std::remove("emulatedRun");
}//~Benchmark

int Benchmark::startBenchmark(Settings currentSettings)
{
	return emulateRun(currentSettings);
}//startBenchmark

int Benchmark::emulateRun(Settings currentSettings)
{
	//to read the game state data and other communications from
	bp::ipstream pipe_stream;

	//run the client
	bp::child s(
		currentSettings.getPath(CLIENT), 
		"EMULATE", 
		currentSettings.getPath(PLAYERINPUT1), 
		currentSettings.getPath(PLAYERINPUT2), 
		bp::std_out > pipe_stream
	);

	std::string received;
	std::ofstream output("emulatedRun", std::ios::app); 	//make an output file for the received data
	
	//shows how much time (assuming 60 fps on the underlaying program) has passed so far 
	int timer = 0;
	int frames = 0;
	bool connected = false;

	//as long as the booted process is running, receive its output
	while(std::getline(pipe_stream, received))
	{
		//if this is the first message
		if(!connected)
		{
			//check for the correct identification
			if (received == "EMULATE START")
			{
				connected = true;
				std::cout << "Running emulation" << std::endl;
			} else { //if it fails, terminate it
				s.terminate();
				std::cerr << "Client did not identify correctly\n";
				return -1;
			}//else
		} else { //if the identification went correctly
			if (!received.empty())
			{
				frames++;
				output << received << "\n";
			}//if
		}//else
		if(frames >= 60)
		{
			frames = 0;
			timer++;
			std::cout << "     " << '\r';
			std::cout << timer/60 << 'm' << timer%60 << 's' << '\r';
			std::cout.flush();

		}//if
	}//while

	s.wait();

	std::cout << std::endl;

	return 0;
}//simulatedRun