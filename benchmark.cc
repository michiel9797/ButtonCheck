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
{
	saveEmulation = CurrentSettings.getSetting(SAVEEMULATION);
	saveSimulation = CurrentSettings.getSetting(SAVESIMULATION);
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
		std::cout << "Set all paths before running the benchmark" << std::endl;
		return -1;
	}//if
	if(CurrentSettings.getSetting(SKIPEMULATION) != "y")
		emulateRun(CurrentSettings);
	if(CurrentSettings.getSetting(SKIPSIMULATION) != "y")
		simulateRun(CurrentSettings);
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
				client.terminate();
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

	client.wait();

	std::cout << std::endl;

	return 0;
}//simulatedRun

int Benchmark::setDevices(Settings &CurrentSettings, std::string ip_path)
{
    //clean old connection setups if they exist
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

    //set IP addresses
    if (bp::system(ip_path, "netns", "exec", "nsClient1", "ip", "addr", "add", "10.0.1.2/24", "dev", "veth1") != 0) return -1;
    if (bp::system(ip_path, "netns", "exec", "nsServer",  "ip", "addr", "add", "10.0.1.1/24", "dev", "vethS1") != 0) return -1;

    if (bp::system(ip_path, "netns", "exec", "nsClient2", "ip", "addr", "add", "10.0.2.2/24", "dev", "veth2") != 0) return -1;
    if (bp::system(ip_path, "netns", "exec", "nsServer",  "ip", "addr", "add", "10.0.2.1/24", "dev", "vethS2") != 0) return -1;

    //bring links up
    bp::system(ip_path, "netns", "exec", "nsClient1", "ip", "link", "set", "veth1", "up");
    bp::system(ip_path, "netns", "exec", "nsClient1", "ip", "link", "set", "lo", "up");

    bp::system(ip_path, "netns", "exec", "nsServer", "ip", "link", "set", "vethS1", "up");
    bp::system(ip_path, "netns", "exec", "nsServer", "ip", "link", "set", "vethS2", "up");
    bp::system(ip_path, "netns", "exec", "nsServer", "ip", "link", "set", "lo", "up");

    bp::system(ip_path, "netns", "exec", "nsClient2", "ip", "link", "set", "veth2", "up");
    bp::system(ip_path, "netns", "exec", "nsClient2", "ip", "link", "set", "lo", "up");

    //enable routing for server
    bp::system(ip_path, "netns", "exec", "nsServer", "sysctl", "-w", "net.ipv4.ip_forward=1");

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

	if(setDevices(CurrentSettings, ip_path) == -1)
	{
		std::cout << "Network setup failed" << std::endl;
		return -1;
	}//if

	//to read the server state from
	bp::ipstream server_stream;

	//to read the client state from
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
		"SERVER", 
		"10.0.1.1:40000",					//the IP and port the server must use
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

	//convert relative path to absolute path
	std::string input = std::filesystem::absolute(CurrentSettings.getSetting(PLAYERINPUT1));

	//run client 1
	bp::child client1(
		ip_path,
		"netns",
		"exec",
		"nsClient1",
		exe, 
		"SIMULATE", 
		input,
		"1",
		"10.0.1.2",
		"10.0.1.1:40000",					//the IP and port the server must use
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
		"SIMULATE", 
		input,
		"2",
		"10.0.2.2",
		"10.0.1.1:40000"//,					//the IP and port the server must use
		//bp::std_out > client_stream
	);

	sleep(10);

	return 0;
}//simulateRun