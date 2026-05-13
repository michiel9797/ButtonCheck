//benchmark.cc
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdlib.h>
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <sys/wait.h>

#include "benchmark.h"

namespace bp = boost::process;
namespace ba = boost::asio;
namespace bs = boost::system;
using json = nlohmann::json;

Benchmark::Benchmark(Settings CurrentSettings)
	:	saveEmulation(CurrentSettings.getSetting(SAVEEMULATION)),
		saveSimulation(CurrentSettings.getSetting(SAVESIMULATION)),
		gilbertElliott(CurrentSettings.getSetting(GILBERTELLIOTT)),
		emulationTime(-1),
		simulationTime(-1),
		netemData({})
{
	//no further initialization needed
}//Benchmark

Benchmark::~Benchmark()
{
	if(saveEmulation == "n")
		std::remove("emulatedRun");
	
	if(saveSimulation == "n")
		std::remove("simulatedRun");
}//~Benchmark

int Benchmark::startBenchmark(Settings CurrentSettings)
{
	if(checkSettings(CurrentSettings) == -1)
		return -1;

	bool compare = true;

	std::ifstream file1("emulatedRun");

	if(CurrentSettings.getSetting(SKIPEMULATION) == "n")
	{	//remove old run if needed
		if(file1.good())
			std::remove("emulatedRun");

		//start the emulation
		if(emulateRun(CurrentSettings) == -1)
		{
			std::cerr << "Emulation failed" << std::endl;
			return -1;
		}//if
	//we shouldn't compare runs if we don't have an emulation file
	}else if(!file1.good())
		compare = false;

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
	//we shouldn't compare runs if we don't have a simulation file
	}else if(!file2.good())
		compare = false;

	//if we can compare runs
	if(compare)
	{
		std::cout << "Accuracy: " << compareRuns()  << "%" << std::endl;
		//if we have emulation times to compare, do so. Only works if 
		//both the emulation and simulation were done this run
		if((emulationTime != -1) && (simulationTime != -1))
			std::cout << "Difference in execution time: " 
					  << simulationTime - emulationTime << "s" << std::endl;
	}//if

	return 0;
}//startBenchmark

const short Benchmark::checkSettings(Settings CurrentSettings)
{	//if we're not skipping the emulation but we miss data needed to run it
	if(CurrentSettings.getSetting(SKIPEMULATION) == "n" &&
	   (CurrentSettings.getSetting(CLIENT) == "None" ||
	    CurrentSettings.getSetting(PLAYERINPUT1) == "None" ||
	    CurrentSettings.getSetting(PLAYERINPUT2) == "None"))
	{
		std::cerr << "Set client and input paths when running the emulation" << std::endl;
		return -1;
	}//if

	//if we're not skipping the simulation but we miss data needed to run it
	if(CurrentSettings.getSetting(SKIPSIMULATION) == "n" &&
	   (CurrentSettings.getSetting(CLIENT) == "None" ||
	    CurrentSettings.getSetting(SERVER) == "None" ||
	    CurrentSettings.getSetting(PLAYERINPUT1) == "None" ||
	    CurrentSettings.getSetting(PLAYERINPUT2) == "None"))
	{
		std::cerr << "Set client, server and input paths when running the simulation step" << std::endl;
		return -1;
	}//if

	//if we want to apply netem but we don't have a file specifying how
	if(CurrentSettings.getSetting(APPLYNETEM) == "y" &&
	   CurrentSettings.getSetting(SKIPSIMULATION) == "n" &&
	   CurrentSettings.getSetting(NETEM) == "None")
	{
		std::cerr << "Set network emulation file path when applying network emulation" << std::endl;
		return -1;
	}//if

	return 0;
}//checkSettings

const double Benchmark::getCPUTime(const rusage usage)
{
	double userTime = usage.ru_utime.tv_sec +
                      usage.ru_utime.tv_usec / 1e6;

    double systemTime = usage.ru_stime.tv_sec +
                        usage.ru_stime.tv_usec / 1e6;

	return userTime + systemTime;
}//getCPUTime

short Benchmark::emulateRun(Settings CurrentSettings)
{
    ba::io_context io;
	//to read the game state data and other communications from
    ba::readable_pipe pipe(io);
    ba::streambuf buffer;
    bs::error_code ec;

	//make an output file for the received data
	std::ofstream output("emulatedRun", std::ios::app);

	//to show how much time has passed so far, initialize now 
	//to reduce time between program launch and output reads
	short timer = 0;
	short frames = 0;
	const short framerate = std::stoi(CurrentSettings.getSetting(FPS));
	bool connected = false;
	bool first = true;
	std::string received;

	//run the client
	bp::process client(
		io,
		CurrentSettings.getSetting(CLIENT), 
		{
			"EMULATE", 
			CurrentSettings.getSetting(PLAYERINPUT1), 
			CurrentSettings.getSetting(PLAYERINPUT2)
		},
		bp::process_stdio{nullptr, pipe, nullptr}
	);

	//as long as the booted process is running, receive its output
	while(ba::read_until(pipe, buffer, '\n', ec))
	{	//break in case of an error
		if(ec)
            break; 

		//load output into the received string
        std::istream is(&buffer);
		std::getline(is, received);

		//if this is the first message
		if(!connected)
		{
			//check for the correct identification
			if(received == "EMULATE START")
			{
				connected = true;
				std::cout << "Running emulation" << std::endl;
				std::cout << "Game time:" << std::endl;
			}else{ //if it fails, terminate it
				client.terminate();
				std::cerr << "Client did not identify correctly\n";
				return -1;
			}//else
		}else{ //if the identification went correctly
			if(!received.empty())
			{
				frames++;
				if(first)
				{
					output << received;
					first = false;
				}else
					output << "\n" << received;
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

auto Benchmark::boostCall(const std::string exe, const std::vector<std::string> args)
{
	ba::io_context io;
	return bp::process(io, exe, args);
}//boostCall

const short Benchmark::setDevices()
{
    //clean old connection setups if they exist
	//delete veths 
	boostCall("/usr/bin/ip", {"link", "del", "BCveth1"}).wait();
	boostCall("/usr/bin/ip", {"link", "del", "BCvethS1"}).wait();
	boostCall("/usr/bin/ip", {"link", "del", "BCveth2"}).wait();
	boostCall("/usr/bin/ip", {"link", "del", "BCvethS2"}).wait();

	//delete bridge
	boostCall("/usr/bin/ip", {"netns", "exec", "nsBCServer", "ip", "link", "del", "BCbridge"}).wait();

	//delete namespaces
	boostCall("/usr/bin/ip", {"netns", "del", "nsBCClient1"}).wait();
	boostCall("/usr/bin/ip", {"netns", "del", "nsBCClient2"}).wait();
	boostCall("/usr/bin/ip", {"netns", "del", "nsBCServer"}).wait();

    //create namespaces
    if(boostCall("/usr/bin/ip", {"netns", "add", "nsBCClient1"}).wait() != 0) return -1;
    if(boostCall("/usr/bin/ip", {"netns", "add", "nsBCClient2"}).wait() != 0) return -1;
    if(boostCall("/usr/bin/ip", {"netns", "add", "nsBCServer"}).wait()  != 0) return -1;

    //create veth pairs
    if(boostCall("/usr/bin/ip", {"link", "add", "BCveth1", "type", "veth", "peer", "name", "BCvethS1"}).wait() != 0) return -1;
    if(boostCall("/usr/bin/ip", {"link", "add", "BCveth2", "type", "veth", "peer", "name", "BCvethS2"}).wait() != 0) return -1;

    //move to namespaces
    if(boostCall("/usr/bin/ip", {"link", "set", "BCveth1",  "netns", "nsBCClient1"}).wait() != 0) return -1;
    if(boostCall("/usr/bin/ip", {"link", "set", "BCvethS1", "netns", "nsBCServer"}).wait()  != 0) return -1;

    if(boostCall("/usr/bin/ip", {"link", "set", "BCveth2",  "netns", "nsBCClient2"}).wait() != 0) return -1;
    if(boostCall("/usr/bin/ip", {"link", "set", "BCvethS2", "netns", "nsBCServer"}).wait()  != 0) return -1;

	//create bridge
	if(boostCall("/usr/bin/ip", {"netns", "exec", "nsBCServer", "ip", "link", "add", "name", "BCbridge", "type", "bridge"}).wait() != 0) return -1;

	//Add both server veth interfaces to the bridge
	boostCall("/usr/bin/ip", {"netns", "exec", "nsBCServer", "ip", "link", "set", "BCvethS1", "master", "BCbridge"}).wait();
	boostCall("/usr/bin/ip", {"netns", "exec", "nsBCServer", "ip", "link", "set", "BCvethS2", "master", "BCbridge"}).wait();

    //set IP addresses
    if(boostCall("/usr/bin/ip", {"netns", "exec", "nsBCClient1", "ip", "addr", "add", "10.0.0.2/24", "dev", "BCveth1"}).wait() != 0) return -1;
    if(boostCall("/usr/bin/ip", {"netns", "exec", "nsBCClient2", "ip", "addr", "add", "10.0.0.3/24", "dev", "BCveth2"}).wait() != 0) return -1;
	if(boostCall("/usr/bin/ip", {"netns", "exec", "nsBCServer", "ip", "addr", "add", "10.0.0.1/24", "dev", "BCbridge"}).wait() != 0) return -1;

    //bring links up
    boostCall("/usr/bin/ip", {"netns", "exec", "nsBCClient1", "ip", "link", "set", "BCveth1", "up"}).wait();
    boostCall("/usr/bin/ip", {"netns", "exec", "nsBCClient1", "ip", "link", "set", "lo", "up"}).wait();

    boostCall("/usr/bin/ip", {"netns", "exec", "nsBCClient2", "ip", "link", "set", "BCveth2", "up"}).wait();
    boostCall("/usr/bin/ip", {"netns", "exec", "nsBCClient2", "ip", "link", "set", "lo", "up"}).wait();

	boostCall("/usr/bin/ip", {"netns", "exec", "nsBCServer", "ip", "link", "set", "BCbridge", "up"}).wait();
	boostCall("/usr/bin/ip", {"netns", "exec", "nsBCServer", "ip", "link", "set", "BCvethS1", "up"}).wait();
	boostCall("/usr/bin/ip", {"netns", "exec", "nsBCServer", "ip", "link", "set", "BCvethS2", "up"}).wait();
	boostCall("/usr/bin/ip", {"netns", "exec", "nsBCServer", "ip", "link", "set", "lo", "up"}).wait();

    return 0;
}//setDevices

const void Benchmark::callNetem(const std::string mode, const std::string delay, const std::string packetLoss)
{
	boostCall("/usr/bin/ip", {"netns", "exec", "nsBCClient2", "tc", "qdisc",
			  mode, "dev", "BCveth2", "root", "netem", "delay", delay, 
			  "loss", packetLoss}).wait();
}//invokeNetem

const void Benchmark::callGENetem(const std::string mode, const std::string delay, const std::string enterBad,
							 	  const std::string exitBad, const std::string goodLoss, const std::string badLoss)
{
	boostCall("/usr/bin/ip", {"netns", "exec", "nsBCClient2", "tc", "qdisc",
			  mode, "dev", "BCveth2", "root", "netem", "delay", delay, "loss", 
			  "gemodel", enterBad, exitBad, goodLoss, badLoss}).wait();
}//invokeGENetem

const std::string Benchmark::getEnterBadState(const std::string avgBurstLength, std::string errorRate)
{
	const float burstFloat = stoi(avgBurstLength);
	//remove the %
	errorRate.erase(errorRate.length() - 1);
	float errorFloat = stoi(errorRate);
	errorFloat = errorFloat / 100;
	//calculate the needed value
	float enterBad = errorFloat / (burstFloat * (1 - errorFloat));
	enterBad = enterBad * 100;
	std::string enterBadString = std::to_string(enterBad);
	//add the %
	enterBadString.insert(enterBadString.end(), '%');
	return enterBadString;
}//getEnterGoodState

const std::string Benchmark::getExitBadState(const std::string avgBurstLength)
{
	const float burstFloat = stoi(avgBurstLength);
	//calculate the needed value
	float exitBad = 1 / burstFloat;
	exitBad = exitBad * 100;
	std::string exitBadString = std::to_string(exitBad);
	//add the %
	exitBadString.insert(exitBadString.end(), '%');
	return exitBadString;
}//getExitGoodState

const void Benchmark::invokeNetem(const short netemCount, const std::string mode)
{
	const std::string latency = netemData[netemCount]["Delay"];
	//if we're using the Gilbert-Elliott model
	if(gilbertElliott == "y")
	{	//if we're using the simplified json format
		if(!netemData[netemCount]["AvgBurstLength"].is_null())
		{
			const std::string avgBurstLength = netemData[netemCount]["AvgBurstLength"];
			const std::string errorRate = netemData[netemCount]["ErrorRate"];
			const std::string enterBad = getEnterBadState(avgBurstLength, errorRate);
			const std::string exitBad = getExitBadState(avgBurstLength);
			callGENetem(mode, latency, enterBad, exitBad, "100%", "0%");
		}else{
			const std::string enterBad = netemData[netemCount]["EnterBad"];
			const std::string exitBad = netemData[netemCount]["ExitBad"];
			const std::string goodLoss = netemData[netemCount]["GoodLoss"];
			const std::string badLoss = netemData[netemCount]["BadLoss"];
			callGENetem(mode, latency, enterBad, exitBad, goodLoss, badLoss);
		}//else
	}else{
		const std::string packetLoss = netemData[netemCount]["PacketLoss"];
		callNetem(mode, latency, packetLoss);
	}//else
}//invokeNetem

const short Benchmark::setNextNetemFrame(const short netemCount)
{
	if(!netemData[netemCount]["Frame"].is_null())
	{	//set the next frame we need to invoke it
		const std::string nextNetemFrame = netemData[netemCount]["Frame"];
		return stoi(nextNetemFrame);
	}else{
		//no more netem data do use, so set 
		//next frame to an unreachable value
		return -1;
	}//else
}//setNextNetemFrame

const void Benchmark::setNetem(short &netemCount, short &nextNetemFrame)
{
	std::string netemFrame;
	if(netemCount == 0)
		netemFrame = netemData[0]["Frame"];

	//if this is not the first netem application
	if(netemCount >= 1)
	{	
		invokeNetem(netemCount, "change");
		netemCount++;	
		nextNetemFrame = setNextNetemFrame(netemCount);
	//if this is the first netem application and it needs to be invoked on this frame
	}else if(netemCount == 0 && stoi(netemFrame) == nextNetemFrame)
	{
		invokeNetem(netemCount, "add");
		netemCount++;
		nextNetemFrame = setNextNetemFrame(netemCount);
	//if this is the firsts netem application and it doesn't need to be invoked on this frame
	}else if(netemCount == 0)
		//set the next frame correctly
		nextNetemFrame = setNextNetemFrame(netemCount);
}//setNetemStart

short Benchmark::simulateRun(Settings CurrentSettings)
{	//set up network devices
	if(setDevices() == -1)
	{
		std::cerr << "Network setup failed" << std::endl;
		return -1;
	}//if

	short nextNetemFrame = 0;
	short netemCount = 0;

	//set network emulation from start if needed
	if(CurrentSettings.getSetting(APPLYNETEM) == "y")
		setNetem(netemCount, nextNetemFrame);

	ba::io_context io;
    bs::error_code ec;
	std::string received;

	//to read the server state from
    ba::readable_pipe server_pipe(io);

	//to read the client state from
    ba::readable_pipe client_pipe(io);

	//convert relative path to absolute path
	std::string exe = std::filesystem::absolute(CurrentSettings.getSetting(SERVER));

	//run the server
	bp::process server(
		io,
		"/usr/bin/ip",
		{
			"netns",
			"exec",
			"nsBCServer",
			exe, 
			"SERVER", 			//the mode the executable should run in
			"10.0.0.1:40000",	//the IP and port the server must use
			CurrentSettings.getSetting(CUSTOMSERVER)
		},						
		bp::process_stdio{nullptr, server_pipe, {}}
	);

	ba::streambuf server_buffer;

	ba::read_until(server_pipe, server_buffer, '\n', ec);
	
	//if it threw an error
	if(ec)
	{
		std::cerr << "Server threw an error on startup" << std::endl;
		return -1;
	}

	std::istream server_stream(&server_buffer);
	std::getline(server_stream, received);

	//check for the correct identification
	if(received == "SERVER START")
		std::cout << "Server started" << std::endl;
	else{ //if it fails, terminate it
		server.terminate();
		std::cerr << "Server did not identify correctly\n";
		return -1;
	}//else

	server_pipe.close();

	//make an output file for the received data
	std::ofstream output("simulatedRun", std::ios::app);

	//to hows how much time has passed so far, initialize now
	//to reduce time between program launch and output reads
	short timer = 0;
	short frames = 0;
	short totalFrames = 0;
	const short framerate = std::stoi(CurrentSettings.getSetting(FPS));
	bool first = true;

	ba::streambuf client_buffer;
	
	//convert relative path to absolute path
	std::string input = std::filesystem::absolute(CurrentSettings.getSetting(PLAYERINPUT1));

	exe = std::filesystem::absolute(CurrentSettings.getSetting(CLIENT));

	//run client 1
	bp::process client1(
		io,
		"/usr/bin/ip",
		{
			"netns",
			"exec",
			"nsBCClient1",
			exe, 
			"SIMULATE", 			//the mode the client should be running in
			input,					//the inputs for this client
			"1",					//which player is playing here
			"10.0.0.2",				//the IP that the client must use
			"10.0.0.1:40000",		//the IP and port the server must use
			CurrentSettings.getSetting(CUSTOMCLIENT)
		},
		bp::process_stdio{nullptr, client_pipe, {}}
	);

	//convert relative path to absolute path
	input = std::filesystem::absolute(CurrentSettings.getSetting(PLAYERINPUT2));

	//run client 2
	bp::process client2(
		io,
		"/usr/bin/ip",
		{
			"netns",
			"exec",
			"nsBCClient2",
			exe, 
			"SIMULATE", 				//the mode the client should be running in
			input,						//the inputs for this client
			"2",						//which player is playing here
			"10.0.0.3",					//the IP that the client must use
			"10.0.0.1:40000",			//the IP and port the server must use
			CurrentSettings.getSetting(CUSTOMCLIENT)
		},
		bp::process_stdio{nullptr, nullptr, {}}
	);

	std::cout << "Running simulation" << std::endl;
	std::cout << "Game time:" << std::endl;

	//as long as the booted process is running, receive its output
	while(true)
	{
		ba::read_until(client_pipe, client_buffer, '\n', ec);

		//if the client threw an error or stopped
		if(ec)
			break;

		std::istream is(&client_buffer);
		std::getline(is, received);

		if(!received.empty())
		{
			frames++;
			totalFrames++;
			if(first)
			{
				output << received;
				first = false;
			}else
				output << "\n" << received;

			//set network emulation if needed
			if(CurrentSettings.getSetting(APPLYNETEM) == "y" && totalFrames == nextNetemFrame)
				setNetem(netemCount, nextNetemFrame);
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

	server.terminate();
	server.wait();

	struct rusage usage;
	wait4(client1.id(), 0, 0, &usage);

	client2.wait();

	simulationTime = getCPUTime(usage);

	std::cout << std::endl << "Simulation finished" << std::endl;

	return 0;
}//simulateRun

const float Benchmark::compareRuns()
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
			unequal++;

		frameCount++;
	}//while
	return 100 - ((100/frameCount) * unequal);
}//compare
