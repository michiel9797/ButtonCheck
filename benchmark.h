//benchmark.h
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

#ifndef BenchmarkH
#define BenchmarkH

#include <vector>
#include <sys/resource.h>

#include "settings.h"
#include "nlohmann-json/json.hpp"

using json = nlohmann::json;

class Benchmark
{
	public:
		//constructor that reads in settings for the deconstructor
		Benchmark(Settings CurrentSettings);
		//deconstructor that deletes any remaining files made at runtime
		~Benchmark();
		//start the benchmark given the selected settings
		int startBenchmark(Settings CurrentSettings);

	private:
		//check if all the required settings are set for the
		//attempted run (may still fail later if values are bad)
		const short checkSettings(Settings CurrentSettings);
		//execute an emulated perfect run to gather correct
		//reference data and the cpu usage baseline
		short emulateRun(Settings CurrentSettings);
		//execute a simulated run with network simulation
		short simulateRun(Settings CurrentSettings);
		//compare both runs, return the accuracy
		const float compareRuns();
		//a helper for migrating to Boost V2
		auto boostCall(const std::string exe, const std::vector<std::string> args);
		//set up the server and client veth pairs
		const short setDevices();
		//calculate the chance of entering the bad state in the
		//Gilbert-Elliott model when using the simplified json structure
		const std::string getEnterBadState(const std::string avgBurstLength, std::string errorRate);
		//calculate the chance of exiting the bad state in the
		//Gilbert-Elliott model when using the simplified json structure
		const std::string getExitBadState(const std::string avgBurstLength);
		//invoke the correct netem
		const void invokeNetem(const short netemCount, const std::string mode);
		//helper for calling tc netem when using Gilbert-Elliott
		const void callGENetem(const std::string mode, const std::string delay, const std::string enterBad,
							   const std::string exitBad, const std::string goodLoss, const std::string badLoss);
		//helper for calling tc netem normally
		const void callNetem(const std::string mode, const std::string delay, const std::string loss);
		//set which frame the next network simulation needs to be called
		const short setNextNetemFrame(const short netemCount);
		//set the state of the network emulation
		const void setNetem(short &netemCount, short &nextNetemFrame);
		//get the total CPU time from rusage
		const double getCPUTime(const rusage usage);
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