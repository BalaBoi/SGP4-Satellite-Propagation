#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <future>
#include <chrono>

//comment out to disable benchmarking
#define BENCHMARKING

#include "Satellite.h"

static const std::string inputFilename {"27000sats.txt"};
static const LatLongRegion region {{16.66673, 103.58196},
{69.74973,-120.64459},
{-21.09096, -119.71009},
{-31.32309, -147.79778}};   //regions cannot wrap around, regions are defined by lines 
                            //that appear straight in the cylindrical projection of the 
                            //assumed sphere of the Earth

static const int max_group_size = 100;    

std::vector<StateVector> group_task(const Satellite& satellite, const std::vector<double>& time_values) {
    std::vector<StateVector> state_vectors;
    for (auto time: time_values) {
        state_vectors.push_back(satellite.findStateVector(time, region));
    }
    return state_vectors;
}

std::vector<StateVector> satellite_task(const Satellite& satellite) {
    std::vector<std::future<std::vector<StateVector>>> group_futures;
    double max_t = Satellite::time(0,0,5,0,0,0);
    double delta_t = Satellite::time(0,0,0,0,0,1);
    
    std::vector<double> group; //group of time values
    for (double t=0; t<=max_t; t+=delta_t) {
        if (group.size()<=max_group_size) {
            group.push_back(t);
        } else {
            group_futures.push_back(std::async(group_task, satellite, group));
            group.clear();
        }
    }

    std::vector<StateVector> state_vectors;
    for (auto& group_future: group_futures) {
        for (auto& state_vector: group_future.get()) {
            state_vectors.push_back(state_vector);
        }
    }
    return state_vectors;
}

int main(int argc, char const *argv[])
{
    auto satellites = Satellite::readSatelliteFile(inputFilename);

    #ifdef BENCHMARKING
    auto bm_start_time = std::chrono::high_resolution_clock::now();
    #endif

    std::vector<std::future<std::vector<StateVector>>> satellite_futures;
    for (auto& satellite: satellites) {//launching tasks
        satellite_futures.push_back(std::async(satellite_task, satellite));
    }

    std::vector<StateVector> state_vectors;
    for (auto& future: satellite_futures) {
        for (auto& sv: future.get()) {
            state_vectors.push_back(sv);
        }
    }

    #ifdef BENCHMARKING
    auto bm_end_time = std::chrono::high_resolution_clock::now();
    auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(bm_end_time-bm_start_time);
    std::cout << "Total time taken: " << time_taken.count() << std::endl;
    #endif
    
    std::cout << "done" << std::endl;
    return 0;
}
