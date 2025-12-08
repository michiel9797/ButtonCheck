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

Benchmark::Benchmark(Settings &CurrentSettings)
	:	saveEmulation(CurrentSettings.getSetting(SAVEEMULATION)),
		saveSimulation(CurrentSettings.getSetting(SAVESIMULATION))
{
	//no further initialization needed
}//Benchmark

Benchmark::~Benchmark()
{
	if(saveEmulation == "n")
	{
		std::remove("emulatedRun");
	}//if
	if(saveSimulation == "n")
	{
		std::remove("simulatedRun");
	}//if
}//~Benchmark

int Benchmark::startBenchmark(Settings &CurrentSettings)
{
	if(CurrentSettings.getSetting(CLIENT) == "None" ||
	   CurrentSettings.getSetting(SERVER) == "None" ||
	   CurrentSettings.getSetting(PLAYERINPUT1) == "None" ||
	   CurrentSettings.getSetting(PLAYERINPUT2) == "None")
	{
		std::cerr << "Set all paths before running the benchmark" << std::endl;
		return -1;
	}//if

	bool compare = true;

	std::ifstream file1("emulatedRun");

	if(CurrentSettings.getSetting(SKIPEMULATION) != "y")
	{	//remove old run if needed
		if(file1.good())
			std::remove("emulatedRun");
		if(emulateRun(CurrentSettings) == -1)
		{
			std::cerr << "Emulation failed" << std::endl;
			return -1;
		}//if
	}else if(!file1.good()){
		compare = false;
	}//else

	std::ifstream file2("simulatedRun");

	if(CurrentSettings.getSetting(SKIPSIMULATION) != "y")
	{	//remove old run if needed
		if(file2.good())
			std::remove("simulatedRun");
		if(simulateRun(CurrentSettings) == -1)
		{
			std::cerr << "Simulation failed" << std::endl;
			return -1;
		}//if
	}else if(!file2.good()){
		compare = false;
	}//else

	if(compare)
		std::cout << "Accuracy: " << compareRuns()  << "%" << std::endl;

	return 0;
}//startBenchmark

int Benchmark::emulateRun(Settings &CurrentSettings)
{
	//to read the game state data and other communications from
	bp::ipstream pipe_stream;

	//run the client
	bp::child client(
		CurrentSettings.getSetting(CLIENT), 
		"EMULATE", 
		CurrentSettings.getSetting(PLAYERINPUT1), 
		CurrentSettings.getSetting(PLAYERINPUT2), 
		bp::std_out > pipe_stream
	);

	std::string received;
	std::ofstream output("emulatedRun", std::ios::app); 	//make an output file for the received data
	
	//shows how much time (assuming 60 fps on the underlaying program) has passed so far 
	int timer = 0;
	int frames = 0;
	bool connected = false;
	bool first = true;

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
				std::cout << "Game time:" << std::endl;
			} else { //if it fails, terminate it
				client.terminate();
				std::cerr << "Client did not identify correctly\n";
				return -1;
			}//else
		} else { //if the identification went correctly
			if (!received.empty())
			{
				frames++;
				if (first)
				{
					output << received;
					first = false;
				}else{
					output << "\n" << received;
				}//else
			}//if
			if(frames >= 60)
			{
				frames = 0;
				timer++;
				std::cout << "     " << '\r';
				std::cout << timer/60 << 'm' << timer%60 << 's' << '\r';
				std::cout.flush();
			}//if
		}//else
	}//while

	client.wait();

	std::cout << std::endl << "Emulation finished" << std::endl;

	return 0;
}//simulatedRun

int Benchmark::setDevices(std::string ip_path)
{
    //clean old connection setups if they exist
	// delete veths 
	bp::system(ip_path, "link", "del", "veth1");
	bp::system(ip_path, "link", "del", "vethS1");
	bp::system(ip_path, "link", "del", "veth2");
	bp::system(ip_path, "link", "del", "vethS2");

	// delete bridge
	bp::system(ip_path, "netns", "exec", "nsServer", "ip", "link", "del", "br0");

	// delete namespaces
	bp::system(ip_path, "netns", "del", "nsClient1");
	bp::system(ip_path, "netns", "del", "nsClient2");
	bp::system(ip_path, "netns", "del", "nsServer");

    //create namespaces
    if (bp::system(ip_path, "netns", "add", "nsClient1") != 0) return -1;
    if (bp::system(ip_path, "netns", "add", "nsServer")  != 0) return -1;
    if (bp::system(ip_path, "netns", "add", "nsClient2") != 0) return -1;

    //create veth pairs
    if (bp::system(ip_path, "link", "add", "veth1", "type", "veth", "peer", "name", "vethS1") != 0) return -1;
    if (bp::system(ip_path, "link", "add", "veth2", "type", "veth", "peer", "name", "vethS2") != 0) return -1;

    //move to namespaces
    if (bp::system(ip_path, "link", "set", "veth1",  "netns", "nsClient1") != 0) return -1;
    if (bp::system(ip_path, "link", "set", "vethS1", "netns", "nsServer")  != 0) return -1;

    if (bp::system(ip_path, "link", "set", "veth2",  "netns", "nsClient2") != 0) return -1;
    if (bp::system(ip_path, "link", "set", "vethS2", "netns", "nsServer")  != 0) return -1;

	//create bridge
	if (bp::system(ip_path, "netns", "exec", "nsServer", "ip", "link", "add", "name", "br0", "type", "bridge") != 0) return -1;

	// Add both server veth interfaces to the bridge
	bp::system(ip_path, "netns", "exec", "nsServer", "ip", "link", "set", "vethS1", "master", "br0");
	bp::system(ip_path, "netns", "exec", "nsServer", "ip", "link", "set", "vethS2", "master", "br0");

    //set IP addresses
	if (bp::system(ip_path, "netns", "exec", "nsServer", "ip", "addr", "add", "10.0.0.1/24", "dev", "br0") != 0) return -1;
    if (bp::system(ip_path, "netns", "exec", "nsClient1", "ip", "addr", "add", "10.0.0.2/24", "dev", "veth1") != 0) return -1;
    if (bp::system(ip_path, "netns", "exec", "nsClient2", "ip", "addr", "add", "10.0.0.3/24", "dev", "veth2") != 0) return -1;

    //bring links up
	bp::system(ip_path, "netns", "exec", "nsServer", "ip", "link", "set", "br0", "up");
	bp::system(ip_path, "netns", "exec", "nsServer", "ip", "link", "set", "vethS1", "up");
	bp::system(ip_path, "netns", "exec", "nsServer", "ip", "link", "set", "vethS2", "up");
	bp::system(ip_path, "netns", "exec", "nsServer", "ip", "link", "set", "lo", "up");

    bp::system(ip_path, "netns", "exec", "nsClient1", "ip", "link", "set", "veth1", "up");
    bp::system(ip_path, "netns", "exec", "nsClient1", "ip", "link", "set", "lo", "up");

    bp::system(ip_path, "netns", "exec", "nsClient2", "ip", "link", "set", "veth2", "up");
    bp::system(ip_path, "netns", "exec", "nsClient2", "ip", "link", "set", "lo", "up");

    return 0;
}//setDevices

int Benchmark::simulateRun(Settings &CurrentSettings)
{
    // Locate the ip command
    std::string ip_path = bp::search_path("ip").string();
    if (ip_path.empty()) {
        std::cerr << "Error: could not find the 'ip' command in PATH.\n";
        return -1;
    }

	if(setDevices(ip_path) == -1)
	{
		std::cerr << "Network setup failed" << std::endl;
		return -1;
	}//if

	//to read the server state from
	bp::ipstream server_stream;

	//to read the server state from
	bp::ipstream client_stream;


	//convert relative path to absolute path
	std::string exe = std::filesystem::absolute(CurrentSettings.getSetting(SERVER));

	//run the server
	bp::child server(
		ip_path,
		"netns",
		"exec",
		"nsServer",
		exe, 
		"SERVER", 							//the mode the executable should run in
		"10.0.0.1:40000",					//the IP and port the server must use
		bp::std_out > server_stream
	);

	std::string received;
	std::getline(server_stream, received);

	//check for the correct identification
	if (received == "SERVER START")
	{
		std::cout << "Server started" << std::endl;
	} else { //if it fails, terminate it
		server.terminate();
		std::cerr << "Server did not identify correctly\n";
		return -1;
	}//else

	server_stream.pipe().close();

	//convert relative path to absolute path
	std::string input = std::filesystem::absolute(CurrentSettings.getSetting(PLAYERINPUT1));

	//run client 1
	bp::child client1(
		ip_path,
		"netns",
		"exec",
		"nsClient1",
		exe, 
		"SIMULATE", 						//the mode the client should be running in
		input,								//the inputs for this client
		"1",								//which player is playing here
		"10.0.0.2",							//the IP that the client must use
		"10.0.0.1:40000",					//the IP and port the server must use
		bp::std_out > client_stream
	);

	//convert relative path to absolute path
	input = std::filesystem::absolute(CurrentSettings.getSetting(PLAYERINPUT2));

	//run client 2
	bp::child client2(
		ip_path,
		"netns",
		"exec",
		"nsClient2",
		exe, 
		"SIMULATE", 						//the mode the client should be running in
		input,								//the inputs for this client
		"2",								//which player is playing here
		"10.0.0.3",							//the IP that the client must use
		"10.0.0.1:40000",					//the IP and port the server must use
		bp::std_out > bp::null
	);

	std::ofstream output("simulatedRun", std::ios::app); 	//make an output file for the received data
	
	//shows how much time (assuming 60 fps on the underlaying program) has passed so far 
	int timer = 0;
	int frames = 0;
	bool first = true;

	std::cout << "Running simulation" << std::endl;
	std::cout << "Game time:" << std::endl;

	//as long as the booted process is running, receive its output
	while(std::getline(client_stream, received))
	{
		if (!received.empty())
		{
			frames++;
			if (first)
			{
				output << received;
				first = false;
			}else{
				output << "\n" << received;
			}//else
		}//if
		if(frames >= 60)
		{
			frames = 0;
			timer++;
			std::cout << "     " << '\r';
			std::cout << timer/60 << 'm' << timer%60 << 's' << '\r';
			std::cout.flush();

		}//if
	}//while

	server.wait();
	client1.wait();
	client2.wait();

	std::cout << std::endl << "Simulation finished" << std::endl;

	return 0;
}//simulateRun

float Benchmark::compareRuns()
{
	std::ifstream emulatedRun("emulatedRun");
	std::ifstream simulatedRun("simulatedRun");
	std::string emulatedLine;
	std::string simulatedLine;

	bool emulatedEmpty = false;
	bool simulatedEmpty = false;

	float frameCount = 0;
	float unequal = 0;

	while(!emulatedEmpty || !simulatedEmpty)
	{	
		if(!emulatedEmpty)
		{
			if(!getline(emulatedRun, emulatedLine))
			{
				emulatedEmpty = true;
				emulatedLine = "";
			}//if
		}//if

		if(!simulatedEmpty)
		{
			if(!getline(simulatedRun, simulatedLine))
			{
				simulatedEmpty = true;
				simulatedLine = "";
			}//if
		}//if

		if(simulatedEmpty && emulatedEmpty)
			break;

		if(emulatedLine != simulatedLine)
		{
			unequal++;
		}//if
		frameCount++;
	}//while
	return 100 - ((100/frameCount) * unequal);
}//compare