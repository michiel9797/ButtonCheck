//benchmark.h
//Made by Michiel van der Bijl
//Bachelor thesis project 2025 Leiden University

//Last edited: 30-03-2025

#ifndef BenchmarkH
#define BenchmarkH

#include "settings.h"

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
		//execute an emulated perfect run to gather correct
		//reference data and the cpu usage baseline
		int emulateRun(Settings &CurrentSettings);
		//execute a simulated run with network simulation
		int simulateRun(Settings &CurrentSettings);
		//compare both runs
		float compareRuns();
		//set up the server and client veth pairs
		int setDevices(std::string ip_path);
		//should the emulation run be removed after a benchmark
		std::string saveEmulation;
		//should the simulation run be removed after a benchmark
		std::string saveSimulation;

};//benchmark

#endif