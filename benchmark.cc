//benchmark.cc
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

//Last edited: 30-03-2025

#include <fstream>
#include <iostream>
#include <filesystem>
#include <boost/process.hpp>
#include <sys/wait.h>
#include "benchmark.h"

namespace bp = boost::process;
using json = nlohmann::json;

Benchmark::Benchmark(Settings &CurrentSettings)
	:	saveEmulation(CurrentSettings.getSetting(SAVEEMULATION)),
		saveSimulation(CurrentSettings.getSetting(SAVESIMULATION)),
		emulationTime(-1),
		simulationTime(-1),
		netemData({})
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
	if(checkSettings(CurrentSettings) == -1)
	{
		return -1;
	}

	bool compare = true;

	std::ifstream file1("emulatedRun");

	if(CurrentSettings.getSetting(SKIPEMULATION) == "n")
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

	if(CurrentSettings.getSetting(SKIPSIMULATION) == "n")
	{	//remove old run if needed
		if(file2.good())
			std::remove("simulatedRun");
		
		//read in the network emulation file if needed
		if(CurrentSettings.getSetting(APPLYNETEM) == "y")
		{
			std::ifstream file(CurrentSettings.getSetting(NETEM));
			try
			{
				netemData = json::parse(file);
			}//try
			catch(...)
			{
				std::cerr << "ERROR: network emulation file provided does not specify a JSON file" << std::endl;
				return -1;
			}//catch
		}//if
		
		if(simulateRun(CurrentSettings) == -1)
		{
			std::cerr << "Simulation failed" << std::endl;
			return -1;
		}//if
	}else if(!file2.good()){
		compare = false;
	}//else

	if(compare)
	{
		std::cout << "Accuracy: " << compareRuns()  << "%" << std::endl;
		if((emulationTime != -1) && (simulationTime != -1))
		{
			std::cout << "Difference in execution time: " 
					  << simulationTime - emulationTime << "s" << std::endl;
		}//if	
	}//if

	return 0;
}//startBenchmark

int Benchmark::checkSettings(Settings &CurrentSettings)
{
	if(CurrentSettings.getSetting(SKIPEMULATION) == "n" &&
	   (CurrentSettings.getSetting(CLIENT) == "None" ||
	    CurrentSettings.getSetting(PLAYERINPUT1) == "None" ||
	    CurrentSettings.getSetting(PLAYERINPUT2) == "None"))
	{
		std::cerr << "Set client and input paths when running the emulation" << std::endl;
		return -1;
	}//if

	if(CurrentSettings.getSetting(SKIPSIMULATION) == "n" &&
	   (CurrentSettings.getSetting(CLIENT) == "None" ||
	    CurrentSettings.getSetting(SERVER) == "None" ||
	    CurrentSettings.getSetting(PLAYERINPUT1) == "None" ||
	    CurrentSettings.getSetting(PLAYERINPUT2) == "None"))
	{
		std::cerr << "Set client, server and input paths when running the simulation step" << std::endl;
		return -1;
	}//if

	if(CurrentSettings.getSetting(APPLYNETEM) == "y" &&
	   CurrentSettings.getSetting(SKIPSIMULATION) == "n" &&
	   CurrentSettings.getSetting(NETEM) == "None")
	{
		std::cerr << "Set network emulation file path when applying network emulation" << std::endl;
		return -1;
	}//if

	return 0;
}//checkSettings

double Benchmark::getCPUTime(rusage usage)
{
	double userTime = usage.ru_utime.tv_sec +
                      usage.ru_utime.tv_usec / 1e6;

    double systemTime = usage.ru_stime.tv_sec +
                        usage.ru_stime.tv_usec / 1e6;

	return userTime + systemTime;
}//getCPUTime

int Benchmark::emulateRun(Settings &CurrentSettings)
{
	//to read the game state data and other communications from
	bp::ipstream pipe_stream;

	std::string received;
	//make an output file for the received data
	std::ofstream output("emulatedRun", std::ios::app);

	//to show how much time has passed so far, initialize now 
	//to reduce time between program launch and output reads
	int timer = 0;
	int frames = 0;
	int framerate = std::stoi(CurrentSettings.getSetting(FPS));
	bool connected = false;
	bool first = true;

	//run the client
	bp::child client(
		CurrentSettings.getSetting(CLIENT), 
		"EMULATE", 
		CurrentSettings.getSetting(PLAYERINPUT1), 
		CurrentSettings.getSetting(PLAYERINPUT2), 
		bp::std_out > pipe_stream
	);

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
			if(frames >= framerate)
			{
				frames = 0;
				timer++;
				std::cout << "     " << '\r';
				std::cout << timer/60 << 'm' << timer%60 << 's' << '\r';
				std::cout.flush();
			}//if
		}//else
	}//while

	struct rusage usage;
	wait4(client.id(), 0, 0, &usage);

	emulationTime = getCPUTime(usage);

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

void Benchmark::invokeNetem(std::string ip_path, std::string mode, 
							std::string delay, std::string packetLoss)
{
	bp::system(ip_path, "netns", "exec", "nsClient2", "tc", "qdisc",
			   mode, "dev", "veth2", "root", "netem", "delay", delay, "loss", packetLoss);
}//invokeNetem

int Benchmark::setNextNetemFrame(int netemCount)
{
	if(!netemData[netemCount]["Frame"].is_null())
	{	//set the next frame we need to invoke it
		std::string nextNetemFrame = netemData[netemCount]["Frame"];
		return stoi(nextNetemFrame);
	}else{
		//no more netem data do use, so set 
		//next frame to an unreachable value
		return -1;
	}//else
}//setNextNetemFrame

void Benchmark::setNetem(std::string ip_path, int &netemCount, int &nextNetemFrame)
{
	if(netemData.is_array())
	{	//set up for a later check
		std::string netemFrame;
		if(netemCount == 0)
		{
			netemFrame = netemData[0]["Frame"];
		}//if

		//if this is not the first netem application
		if(netemCount >= 1)
		{	//extract and apply latency and packet loss
			std::string latency = netemData[netemCount]["Delay"];
			std::string packetLoss = netemData[netemCount]["PacketLoss"];
			invokeNetem(ip_path, "change", latency, packetLoss);
			
			netemCount++;	
			nextNetemFrame = setNextNetemFrame(netemCount);
		//if this is the first netem application and it needs to be invoked on this frame
		}else if(netemCount == 0 && stoi(netemFrame) == nextNetemFrame)
		{	//extract and apply latency and packet loss
			std::string latency = netemData[netemCount]["Delay"];
			std::string packetLoss = netemData[netemCount]["PacketLoss"];
			invokeNetem(ip_path, "add", latency, packetLoss);

			netemCount++;
			nextNetemFrame = setNextNetemFrame(netemCount);
		//if this is the firsts netem application and it doesn't need to be invoked on this frame
		}else if(netemCount == 0){
			//set up the qdisc for later
			invokeNetem(ip_path, "add", "0ms", "0%");
			
			nextNetemFrame = setNextNetemFrame(netemCount);
		}//else
	//if we still need to invoke the netem
	}else if(netemCount != -1){
		//set up for the check
		std::string netemFrame = netemData["Frame"];

		//if the netem needs to be invoked on this frame
		if(stoi(netemFrame) == nextNetemFrame)
		{	//exctract and apply latency and packet loss
			std::string latency = netemData["Delay"];
			std::string packetLoss = netemData["PacketLoss"];
			invokeNetem(ip_path, "add", latency, packetLoss);
			netemCount = -1;
		}else{
			//set the next frame we need to invoke it
			nextNetemFrame = stoi(netemFrame);
		}//else
	}//else
}//setNetemStart

int Benchmark::simulateRun(Settings &CurrentSettings)
{
    //locate the ip command
    std::string ip_path = bp::search_path("ip").string();
    if (ip_path.empty()) {
        std::cerr << "Error: could not find the 'ip' command in PATH.\n";
        return -1;
    }

	//set up network devices
	if(setDevices(ip_path) == -1)
	{
		std::cerr << "Network setup failed" << std::endl;
		return -1;
	}//if

	int nextNetemFrame = 0;
	int netemCount = 0;

	//set network emulation from start if needed
	if(CurrentSettings.getSetting(APPLYNETEM) == "y")
	{
		setNetem(ip_path, netemCount, nextNetemFrame);
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

	//make an output file for the received data
	std::ofstream output("simulatedRun", std::ios::app);

	//to hows how much time has passed so far, initialize now
	//to reduce time between program launch and output reads
	int timer = 0;
	int frames = 0;
	int totalFrames = 0;
	int framerate = std::stoi(CurrentSettings.getSetting(FPS));
	bool first = true;

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

	std::cout << "Running simulation" << std::endl;
	std::cout << "Game time:" << std::endl;

	//as long as the booted process is running, receive its output
	while(std::getline(client_stream, received))
	{
		if (!received.empty())
		{
			frames++;
			totalFrames++;
			if (first)
			{
				output << received;
				first = false;
			}else{
				output << "\n" << received;
			}//else

			//set network emulation if needed
			if(CurrentSettings.getSetting(APPLYNETEM) == "y" && totalFrames == nextNetemFrame)
			{
				setNetem(ip_path, netemCount, nextNetemFrame);
			}//if
		}//if
		if(frames >= framerate)
		{
			frames = 0;
			timer++;
			std::cout << "     " << '\r';
			std::cout << timer/60 << 'm' << timer%60 << 's' << '\r';
			std::cout.flush();

		}//if
	}//while

	server.wait();

	struct rusage usage;
	wait4(client1.id(), 0, 0, &usage);

	client2.wait();

	simulationTime = getCPUTime(usage);

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