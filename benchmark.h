//benchmark.h
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

//Last edited: 30-03-2025

#ifndef BenchmarkH
#define BenchmarkH

#include <sys/resource.h>
#include "settings.h"
#include "nlohmann-json/json.hpp"

using json = nlohmann::json;

class Benchmark
{
	public:
		//constructor that reads in settings for the deconstructor
		Benchmark(Settings &CurrentSettings);
		//deconstructor that deletes any remaining files made at runtime
		~Benchmark();
		//start the benchmark given the selected settings
		int startBenchmark(Settings &CurrentSettings);

	private:
		//check if all the required settings are set for the
		//attempted run (may still fail later if values are bad)
		int checkSettings(Settings &CurrentSettings);
		//execute an emulated perfect run to gather correct
		//reference data and the cpu usage baseline
		int emulateRun(Settings &CurrentSettings);
		//execute a simulated run with network simulation
		int simulateRun(Settings &CurrentSettings);
		//compare both runs, return the accuracy
		float compareRuns();
		//set up the server and client veth pairs
		int setDevices();
		//calculate the chance of entering the bad state in the
		//Gilbert-Elliott model when using the simplified json structure
		std::string getEnterBadState(std::string avgBurstLength, std::string errorRate);
		//calculate the chance of exiting the bad state in the
		//Gilbert-Elliott model when using the simplified json structure
		std::string getExitBadState(std::string avgBurstLength);
		//invoke the correct netem
		void invokeNetem(int netemCount, std::string mode);
		//helper for calling tc netem when using Gilbert-Elliott
		void callGENetem(std::string mode, std::string delay, std::string enterBad,
						 std::string exitBad, std::string goodLoss, std::string badLoss);
		//helper for calling tc netem normally
		void callNetem(std::string mode, std::string delay, std::string loss);
		//set which frame the next network simulation needs to be called
		int setNextNetemFrame(int netemCount);
		//set the state of the network emulation
		void setNetem(int &netemCount, int &nextNetemFrame);
		//get the total CPU time from rusage
		double getCPUTime(rusage usage);
		//the path to the IP function
		std::string ipPath;
		//should the emulation run be removed after a benchmark
		std::string saveEmulation;
		//should the simulation run be removed after a benchmark
		std::string saveSimulation;
		//should we use the Gilbert-Elliott model
		std::string gilbertElliott;
		//total cpu time the emulation run spanned
		double emulationTime;
		//total cpu time the simulation run spanned
		double simulationTime;
		//the json object with the network emulation data
		json netemData;

};//benchmark

#endif