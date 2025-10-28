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
		//deconstructor that deletes any remaining files made at runtime
		~Benchmark();
		//start the benchmark given the selected settings
		int startBenchmark(Settings currentSettings);

	private:
		//execute a simulated perfect run to gather correct
		//reference data and the cpu usage baseline
		int emulateRun(Settings currentSettings);

};//benchmark

#endif