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
		int setDevices(std::string ip_path);
		//helper for calling tc netem
		void invokeNetem(std::string ip_path, std::string mode1, 
						 std::string delay, std::string loss);
		//set which frame the next network simulation needs to be called
		int setNextNetemFrame(int netemCount);
		//set the state of the network emulation
		void setNetem(std::string ip_path, int &netemCount, int &nextNetemFrame);
		//get the total CPU time from rusage
		double getCPUTime(rusage usage);
		//should the emulation run be removed after a benchmark
		std::string saveEmulation;
		//should the simulation run be removed after a benchmark
		std::string saveSimulation;
		//total cpu time the emulation run spanned
		double emulationTime;
		//total cpu time the simulation run spanned
		double simulationTime;
		//the json object with the network emulation data
		json netemData;

};//benchmark

#endif